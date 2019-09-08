
#include "LED_Uti.h"


LEDUti::LEDUti(LEDController* led_controller, CRGB* leds_ptr):
m_led_controller(led_controller),
m_leds(leds_ptr),
m_last_iteration(0),
m_no_payload_counter(0),
m_smooth_amplitude(0),
m_smooth_speed(0)
{  
}

LEDUti::~LEDUti()
{
  end_effect();
}

/* Finish the current effect and cleanup */
void LEDUti::end_effect()
{
  /* TBD: any action needed? */  
}

/* Color conversion from RGB to HSV */
LEDUti::hsv LEDUti::rgb2hsv(RGBcolor in_rgb)
{

    hsv         in;
    hsv         out; //Re-use of hsv structure to avoid redefining another structure with similar content only for this
    double      min, max, delta;

    /* Adapt format */
    in.h = ((double)in_rgb.R) / 255;
    in.s = ((double)in_rgb.G) / 255;
    in.v = ((double)in_rgb.B) / 255;

    min = in.h < in.s ? in.h : in.s;
    min = min  < in.v ? min  : in.v;

    max = in.h > in.s ? in.h : in.s;
    max = max  > in.v ? max  : in.v;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.h >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.s - in.v ) / delta;        // between yellow & magenta
    else
    if( in.s >= max )
        out.h = 2.0 + ( in.v - in.h ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.h - in.s ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

/* Color conversion from HSV to RGB */
RGBcolor LEDUti::hsv2rgb(LEDUti::hsv in)
{
  double      hh, p, q, t, ff;
  long        i;
  hsv         out; //Re-use of hsv structure to avoid redefining another structure with similar content only for this
  RGBcolor retVal;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.h = in.v;
        out.s = in.v;
        out.v = in.v;

        retVal.R = (uint8_t)(255*out.h);
        retVal.G = (uint8_t)(255*out.s);
        retVal.B = (uint8_t)(255*out.v);
        
        return retVal;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.h = in.v;
        out.s = t;
        out.v = p;
        break;
    case 1:
        out.h = q;
        out.s = in.v;
        out.v = p;
        break;
    case 2:
        out.h = p;
        out.s = in.v;
        out.v = t;
        break;

    case 3:
        out.h = p;
        out.s = q;
        out.v = in.v;
        break;
    case 4:
        out.h = t;
        out.s = p;
        out.v = in.v;
        break;
    case 5:
    default:
        out.h = in.v;
        out.s = p;
        out.v = q;
        break;
    }

    retVal.R = (uint8_t)(255*out.h);
    retVal.G = (uint8_t)(255*out.s);
    retVal.B = (uint8_t)(255*out.v);
    
    return retVal;     
}

/* Shift leds a defined number of positions with a given input color */
void LEDUti::shift_leds(
  uint8_t led_start,  //start position (always < led_end)
  uint8_t led_end,  //end position
  uint8_t positions,  //number of leds to shift
  const bool top,  //shift direction true-->upwards, false-->downwards
  const uint8_t delay_ms, //delay (in ms) after shifting the leds. TODO: Needed?
  const uint8_t R_in, //input color
  const uint8_t G_in, //input color
  const uint8_t B_in  //input color
)
{
  /* Prevent wrong order */
  if(led_end <= led_start) return;  
  /* Minimum number of positions to shift must be 1 */
  if(positions == 0) positions = 1;
  /* Clip the number of leds to shift */
  if( (led_end - led_start) < positions ) positions = led_end - led_start;
  
  /* Upwards direction */
  if(top)
  {
    /* Shift existing colors */
    for(uint8_t j = led_end - 1; j >= (led_start + positions) ; j--)
    {
      m_leds[j] = m_leds[j-positions];     
    }
    /* Introduce input color */
    for(uint8_t j = led_start; j < (led_start + positions); j++)
    {
      m_leds[j] = CRGB(R_in,G_in,B_in);
    }
  }
  /* Downwards direction */
  else 
  {
    /* Shift existing colors */
    for(uint8_t j = led_start; j < (led_end - positions); j++)
    {
      m_leds[j] = m_leds[j+positions];
    }
    /* Introduce input color */
    for(uint8_t j = led_end - positions; j < led_end; j++)
    {
      m_leds[j] = CRGB(R_in,G_in,B_in);
    }
  }

  FastLED.show();
  delay(delay_ms);
}

/* Translate from a relative amplitude [0-100] to an absolute suitable amplitude for bubble effect */
/* Apply smooth transition from old amplitude towards new amplitude */
uint8_t LEDUti::compute_bubble_amplitude(uint8_t amplitude)
{
  uint8_t targetAmplitude;

  /* Compute target amplitude */
  if(amplitude > 75) targetAmplitude = 5;
  else if(amplitude > 50) targetAmplitude = 4;
  else if(amplitude > 40) targetAmplitude = 3;
  else if(amplitude > 25) targetAmplitude = 2;
  else targetAmplitude = 1;

  /* Compute a smoothed new amplitude */
  m_smooth_amplitude = ((float)targetAmplitude*3 + m_smooth_amplitude) / 4;
  targetAmplitude = (uint8_t)round(m_smooth_amplitude);
  return targetAmplitude;
}

/* Translate from a relative speed [0-255] to an absolute suitable amplitude for bubble effect */
/* Apply smooth transition from old amplitude towards new amplitude */
uint8_t LEDUti::compute_speed(uint16_t speedS)
{
  uint8_t targetSpeed;

  /* Compute target amplitude */
  if(speedS > 200) targetSpeed = 5;
  else if(speedS > 150) targetSpeed = 4;
  else if(speedS > 100) targetSpeed = 3;
  else if(speedS > 50) targetSpeed = 2;
  else targetSpeed = 1;

  /* Compute a smoothed new amplitude */
  m_smooth_speed = ((float)targetSpeed*3 + m_smooth_speed) / 4;
  targetSpeed = (uint8_t)round(m_smooth_speed);
  return targetSpeed;
}


/* Adapt the value or the RGB according to the speed.
 * The slowest the les bright
 * The fastest the brightest until reaching white
 */
byte* compute_saturation(uint8_t r, uint8_t g, uint8_t b, uint16_t speedS)
{
  int16_t speedFactor;
  static byte c[3];
  int16_t colors[3] = {(int16_t)r,(int16_t)g,(int16_t)b};

 
  /* Compute target amplitude */
  if(speedS > 190) speedFactor = speedS;
  else if(speedS > 180) speedFactor = 140;
  else if(speedS > 170) speedFactor = 130;
  else if(speedS > 160) speedFactor = 120;
  else if(speedS > 150) speedFactor = 110;
  else if(speedS > 140) speedFactor = 100;
  else if(speedS > 130) speedFactor = 90;
  else if(speedS > 160) speedFactor = 80;
  else if(speedS > 150) speedFactor = 70;
  else if(speedS > 140) speedFactor = 60;
  else if(speedS > 130) speedFactor = 50; 
  else if(speedS > 120) speedFactor = 40;
  else if(speedS > 110) speedFactor = 30;
  else if(speedS > 100) speedFactor = 20;
  else if(speedS > 90) speedFactor = 10;
  else if(speedS > 80) speedFactor = 0;
  else if(speedS > 70) speedFactor = -10;
  else if(speedS > 60) speedFactor = -20;
  else if(speedS > 50) speedFactor = -30;
  else if(speedS > 40) speedFactor = -40;
  else if(speedS > 30) speedFactor = -50;
  else if(speedS > 20) speedFactor = -60;
  else if(speedS > 10) speedFactor = -70;
  else speedFactor = -80;

  /* Compute a smoothed speed TODO */
  //m_smooth_speed = ((float)targetSpeed*3 + m_smooth_speed) / 4;
  // targetSpeed = (uint8_t)round(m_smooth_speed);
  
  for(int i = 0 ; i < 3; i++)
  {
	  if((colors[i] + speedFactor) < 0)
	  {
		  c[i] = 0;
	  }
	  else if ((colors[i] + speedFactor) > 255)
	  {
		c[i] = 255;
	  }
	  else
	  {
		  c[i] =+speedFactor;
	  }
  }

  return c;
}

/* Print a number of leds with a given color */
void LEDUti::print_amplitude_color(
  uint8_t led_start, //start position (must be < led_end)
  uint8_t led_end, //end position
  const bool top, //effect direction
  uint8_t amplitude, //number of leds to print
  uint8_t r, //color
  uint8_t g, //color
  uint8_t b  //color
)
{
  /* Upwards direction */
  if(top)
  { 
    /* Print requested amplitude */
    for(uint8_t i = led_start; i < led_start + amplitude; i++)
    {
      m_leds[i] = CRGB(r,g,b);
    }
    /* Erase rest */
    for(uint8_t i = led_start + amplitude; i < led_end; i++)
    {
      m_leds[i] = CRGB(0,0,0);
    }
  }
  /* Downwards direction */
  else
  {
    /* Print requested amplitude */
    for(uint8_t i = (led_end - amplitude); i < led_end; i++)
    {
      m_leds[i] = CRGB(r,g,b);
    } 
    /* Erase rest */
    for(uint8_t i = led_start; i < (led_end - amplitude); i++)
    {
      m_leds[i] = CRGB(0,0,0);
    }
       
  }
   FastLED.show();  
}

/* Generate a list of colors based on a given base color and a Hue increment */
void LEDUti::generate_static_colors(
  uint8_t r_base, //base color
  uint8_t g_base, //base color
  uint8_t b_base, //base color
  uint8_t increment //Increment step (in degrees) for each single generated color
)
{
  
  hsv hsv_color;
  RGBcolor rgb_color;

  rgb_color.R = r_base;
  rgb_color.G = g_base;
  rgb_color.B = b_base;
  
  /* Translate the base color from RGB to HSV */
  hsv_color = rgb2hsv(rgb_color);

  /* Increment the Hue for each led */
  for(uint8_t i = 0; i < NUM_LEDS; i++)
  {
    hsv_color.h += increment;
    if(hsv_color.h > 360.0) hsv_color.h = 0.0; //Circular increment between [0,360]
    /* Convert to RGB */
    rgb_color = hsv2rgb(hsv_color);
   
    /* Add color to the led shadow memory */
    m_static_color[i] = CRGB(rgb_color.R,rgb_color.G,rgb_color.B);

    Serial.print(rgb_color.R);
    Serial.print("/");
    Serial.print(rgb_color.G);
    Serial.print("/");
    Serial.println(rgb_color.B);

  }  
}

/* Check if any of the input parameters have been updated since the last iteration/function call */
bool LEDUti::is_update(uint8_t r, uint8_t g, uint8_t b, uint8_t amplitude)
{
  bool retVal = false;
  
  /* Get last iteration values from object generic memory */
  uint8_t r_old = music_effect_mem[0];
  uint8_t g_old = music_effect_mem[1];
  uint8_t b_old = music_effect_mem[2];
  uint8_t ampl_req = music_effect_mem[3];
  
  /* Check for updates in color */
  if( (r_old != r) || (g_old != g) || (b_old != b) )
  {
    /* Update effect memory */
    music_effect_mem[0] = r;
    music_effect_mem[1] = g;
    music_effect_mem[2] = b;    
    retVal = true;
  } 
  /* Check for updates in amplitude (step ignored when generic amplitude 0xFF given) */
  else if( (amplitude != 0xFF) && (ampl_req != amplitude) )
  {
    /* Update effect memory */
    music_effect_mem[3] = amplitude;    
    retVal = true;
  }
  
  return retVal;
}

/* Print a number of leds with a given base color and amplitude
*  When the given base color is changed, a new static color list will be generated
*/
void LEDUti::print_amplitude_static(
  uint8_t led_start, //start position (must be < led_end)
  uint8_t led_end,  //end position
  const bool top, //effect direction
  uint8_t amplitude,  //number of leds to print
  uint8_t r_base,  //base color
  uint8_t g_base,  //base color
  uint8_t b_base,  //base color
  uint8_t increment //unit increment for new color list generation
)
{
  
  /* Check if the base color has been changed  */
  if(is_update(r_base,g_base,b_base,0xFF))
  {
    Serial.println("New color update. Generating static colors");
    /* Generate new static colors based on the base color */
     generate_static_colors(r_base,g_base,b_base,increment);
  }
 
  /* Upwards direction */
  if(top)
  {  
    /* Print requested amplitude from generated color */
    for(uint8_t i = led_start; i < led_start + amplitude; i++)
    {
      m_leds[i] = m_static_color[i];
    }
    /* Erase rest */
    for(uint8_t i = led_start + amplitude; i < led_end; i++)
    {
      m_leds[i] = CRGB(0,0,0);
    }
  }
  else
  {     
    /* Print requested amplitude from generated color */
    for(uint8_t i = (led_end - amplitude); i < led_end; i++)
    {
      m_leds[i] = m_static_color[i];
    } 
    /* Erase rest */
    for(uint8_t i = led_start; i < (led_end - amplitude); i++)
    {
      m_leds[i] = CRGB(0,0,0);
    }       
  }
   FastLED.show();  
} 
  

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
