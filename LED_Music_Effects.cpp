
#include "LED_Music_Effects.h"


LEDMusicEffects::LEDMusicEffects(LEDController* led_controller, CRGB* leds_ptr):
m_led_controller(led_controller),
m_leds(leds_ptr),
m_last_iteration(0),
m_no_payload_counter(0),
m_smooth_amplitude(0)
{  
}

LEDMusicEffects::~LEDMusicEffects()
{
  end_effect();
}

/* Finish the current effect and cleanup */
void LEDMusicEffects::end_effect()
{
  /* TBD: any action needed? */  
}

/* Color conversion from RGB to HSV */
LEDMusicEffects::hsv LEDMusicEffects::rgb2hsv(RGBcolor in_rgb)
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
RGBcolor LEDMusicEffects::hsv2rgb(LEDMusicEffects::hsv in)
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
void LEDMusicEffects::shift_leds(
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
uint8_t LEDMusicEffects::compute_bubble_amplitude(uint8_t amplitude)
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

/* Bubble effect */
void LEDMusicEffects::bubble_effect(
  uint32_t refresh_rate, //effect refresh rate
  uint8_t r, //new color
  uint8_t g, //new color
  uint8_t b, //new color
  uint8_t amplitude, //effect amplitude. Range [0-100]
  uint8_t direction, //effect direction
  bool& isNewPayload //new payload received indication
)
{
  unsigned long now = millis();

  if( (now - m_last_iteration) > refresh_rate )
  {
    m_last_iteration = now;

    /* Adapt the amplitude for this effect */
    amplitude = compute_bubble_amplitude(amplitude);

    /* Increment non-received payload counter */
    if(!isNewPayload)
    {
      m_no_payload_counter++;
    }
    else
    {
      m_no_payload_counter = 0;
    }

    /* Set color to 0 if too many iterations without new payload */
    if(m_no_payload_counter > MAX_PAYLOAD_LOSS)
    {
      m_no_payload_counter = MAX_PAYLOAD_LOSS; //Avoid overflow
      r = 0;
      g = 0;
      b = 0;
      Serial.println("Excessive message loss. Forcing color to 0");
    }

    /* Delete new payload flag */
    isNewPayload = false;
    
    if(direction == 0) //Up 
    {
      shift_leds(0, NUM_LEDS, amplitude, true, 0, r, g, b);
    }
    else if(direction == 1) //Down
    {
      shift_leds(0, NUM_LEDS, amplitude, false, 0, r, g, b);
    }
    else if(direction == 2) //Middle-out
    {
      shift_leds(0, NUM_LEDS / 2 , amplitude, false, 0, r, g, b);
      shift_leds(NUM_LEDS / 2, NUM_LEDS, amplitude, true, 0, r, g, b); 
    }
    else if(direction == 3) //Out-middle
    {
      shift_leds(0, NUM_LEDS / 2 , amplitude, true, 0, r, g, b);
      shift_leds(NUM_LEDS / 2, NUM_LEDS, amplitude, false, 0, r, g, b);      
    }
  }
}

/* Print a number of leds with a given color */
void LEDMusicEffects::print_amplitude_color(
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
void LEDMusicEffects::generate_static_colors(
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
bool LEDMusicEffects::is_update(uint8_t r, uint8_t g, uint8_t b, uint8_t amplitude)
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
void LEDMusicEffects::print_amplitude_static(
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
  
/* Power bars effect */
void LEDMusicEffects::power_bars_effect(
  uint32_t refresh_rate, //effect refresh rate
  uint8_t r, //effect color
  uint8_t g, //effect color
  uint8_t b, //effect color
  uint8_t& amplitude, //number of leds to print. This value can be modified by the function
  uint8_t direction, //effect direction (top, down, middle-out, out-middle)
  uint8_t effect_type, //effect type (static color, given color)
  uint8_t increment //color unit increment (only for static color type)
)
{
  unsigned long now = millis();

  /* Time to update the LED status */
  if( (now - m_last_iteration) > refresh_rate )
  {    
    /* Reset timer */
    m_last_iteration = now;

    /* Given color effect */
    if(effect_type == 0)
    {
      if(direction == 0) //Up
      {
        print_amplitude_color(0, NUM_LEDS, true, amplitude, r, g, b);
      }
      else if(direction == 1) //Down
      {
        print_amplitude_color(0, NUM_LEDS, false, amplitude, r, g, b);
      }
      else if(direction == 2) //Middle-out
      {
        print_amplitude_color(NUM_LEDS / 2, NUM_LEDS, true, amplitude / 2, r, g, b);
        print_amplitude_color(0, NUM_LEDS / 2, false , amplitude / 2, r, g, b);
      }
      else if(direction == 3) //Out-middle
      {
        print_amplitude_color(NUM_LEDS / 2, NUM_LEDS, false, amplitude / 2, r, g, b);
        print_amplitude_color(0, NUM_LEDS / 2, true , amplitude / 2, r, g, b);
      }
    }
    /* Static color effect */
    else if(effect_type == 1)
    {
      if(direction == 0) //Up
      {
        print_amplitude_static(0, NUM_LEDS, true, amplitude, r, g, b, increment);
      }
      else if(direction == 1) //Down
      {
        print_amplitude_static(0, NUM_LEDS, false, amplitude, r, g, b, increment);
      }
      else if(direction == 2) //Middle-out
      {
        print_amplitude_static(NUM_LEDS / 2, NUM_LEDS, true, amplitude / 2, r, g, b, increment);
        print_amplitude_static(0, NUM_LEDS / 2, false , amplitude / 2, r, g, b, increment);
      }
      else if(direction == 3) //Out-middle
      {
        print_amplitude_static(NUM_LEDS / 2, NUM_LEDS, false, amplitude / 2, r, g, b, increment);
        print_amplitude_static(0, NUM_LEDS / 2, true , amplitude / 2, r, g, b, increment);
      }
    }

    /* If there is no update since last iteration, perform a decay effect by reducing the requested amplitude one unit */
    /* This helps to make the effect smoother in case of high network load, where no new UDP packets are received between iterations */
    if( !is_update(r,g,b,amplitude) )
    {
      if(amplitude != 0) amplitude--;     
    }
  }
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
