#include <math.h>       /* pow */
#include "LED_Animated_Effects.h"


LEDAnimatedEffects::LEDAnimatedEffects(LEDController* led_controller, CRGB* leds_ptr):
m_led_controller(led_controller),
leds(leds_ptr)
{
  start_sequence = true;
  m_last_iteration = 0;
}

LEDAnimatedEffects::~LEDAnimatedEffects()
{
  end_effect();
}

void LEDAnimatedEffects::end_effect()
{
  start_sequence = true;
}

/* 
 *  Depending on the position the choosen RGB color will fade in or out: 
 *  The greater the position, the brighter it gets.
 */
void LEDAnimatedEffects::FadeInOut(byte red, byte green, byte blue, IMUData imuData)
{

  int k,order;
  uint16_t postitonS = imuData.xPos;
  Serial.print("PostitonS: ");
  Serial.println(postitonS);
   
  if(start_sequence)
  {
    k = 0;
    order = 0;
    start_sequence = false;
  }
  else
  {
    k = m_effect_state[0];
    order = m_effect_state[1];
  }
  
  float r, g, b, relativePostion;
  /* Exponencial change */
  relativePostion = (pow(postitonS, 2))/8; //*(postitonS / 1.42);
  
  k = relativePostion;
  
  r = int((float)((k/256.0))*red);
  g = int((float)((k/256.0))*green);
  b = int((float)((k/256.0))*blue);

  Serial.print("r ");
  Serial.println(r);
  Serial.print("g ");
  Serial.println(g);
  Serial.print("b ");
  Serial.println(b);
  
  setAll(r,g,b);
  showStrip();

  m_effect_state[0] = k;
  m_effect_state[1] = order;
}

/*
 * The colors of the rainbow will be displayed acording to the position
 * The brightness of the color depends on the speed of the stick.
 */
void LEDAnimatedEffects::RainbowFadeInOut(IMUData imuData)
{
  int s,p, rIntensity, gIntensity, bIntensity, rSaturation, gSaturation, bSaturation, rPixel, gPixel, bPixel;
  uint16_t postitonS = imuData.xPos;
  uint16_t accX = imuData.xAcc;
  byte *c, *cs; 
  
  if(start_sequence)
  {
    p = 0;
    s = 0;
    start_sequence = false;
  }
  else
  {
    p = m_effect_state[0];
    s = m_effect_state[1];
  }
  
  float r, g, b, relativePostion;
  
  relativePostion = postitonS / 1.42;
  s = accX;
 
  if(p < 256*5)
  {
    p =+ relativePostion;
    for(int i=0; i< NUM_LEDS; i++) {
     c = Wheel(((i * 256 / NUM_LEDS) + p) & 255);
     cs = compute_saturation(*c, *(c+1), *(c+2), s );
     
     setPixel(i, *cs , *(cs+1) , *(cs+2));
    }
    showStrip();
  }
  else
  {
    p = 0;
  }
  
  m_effect_state[0] = p;
  m_effect_state[1] = s;
}

void LEDAnimatedEffects::ColorChangingWithPosition(IMUData imuData)
{
  int s,p, rIntensity, gIntensity, bIntensity, rSaturation, gSaturation, bSaturation, rPixel, gPixel, bPixel;
  uint16_t postitonS = imuData.yPos + imuData.xPos;
  uint16_t zAcc = imuData.zAcc;
  byte *c, *cs; 
  
  if(start_sequence)
  {
    p = 0;
    s = 0;
    start_sequence = false;
  }
  else
  {
    p = m_effect_state[0];
    s = m_effect_state[1];
  }
  
  float r, g, b, relativePostion;
  
  relativePostion = postitonS / 1.42;
  s = zAcc;
 
  if(p < 256*5)
  {
    p =+ relativePostion;

    c = Wheel((( 256 / NUM_LEDS) + p) & 255);
    cs = compute_saturation(*c, *(c+1), *(c+2), s );    
    setAll(*cs , *(cs+1) , *(cs+2));   
    showStrip();
  }
  else
  {
    p = 0;
  }
  
  m_effect_state[0] = p;
  m_effect_state[1] = s;
}



void LEDAnimatedEffects::TwinkleAnim(byte red, byte green, byte blue, int Count, IMUData imuData)
{

  unsigned long now = millis();
  int i;
  if(start_sequence)
  {
    i = 0;
    start_sequence = false;
  }
  else
  {
    i = m_effect_state[0];
  }

  if ( ((now - m_last_iteration) > imuData.yAcc))
  {
    m_last_iteration = now;
    setAll(0,0,0);

    if(i < imuData.xPos)
    {
      setPixel(random(NUM_LEDS),red,green,blue);
      showStrip();
      i++;
    }
    else
    {
      i = 0;
    }
  }

  m_effect_state[0] = i;

}


void LEDAnimatedEffects::TwinkleRandomAnim(int Count, IMUData imuData)
{
  unsigned long now = millis();
  int i;
  if(start_sequence)
  {
    i = 0;
    start_sequence = false;
  }
  else
  {
    i = m_effect_state[0];
  }

  if ( ((now - m_last_iteration) > imuData.xAcc))
  {
    m_last_iteration = now;
    setAll(0,0,0);

    if(i < imuData.zPos)
    {
      setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));
      showStrip();
      i++;
    }
    else
    {
      i = 0;
    }
  }
  m_effect_state[0] = i;
}

void LEDAnimatedEffects::FireAnim(IMUData imuData)
{

  static byte heat[NUM_LEDS];

  unsigned long now = millis();

  if((now - m_last_iteration) > imuData.yAcc)
  {

    m_last_iteration = now;
    int cooldown;

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      cooldown = random(0, ((imuData.xPos * 10) / NUM_LEDS) + 2);

      if(cooldown>heat[i]) {
        heat[i]=0;
      } else {
        heat[i]=heat[i]-cooldown;
      }
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' near the bottom
    if( random(255) < imuData.yPos ) {
      int y = random(7);
      heat[y] = heat[y] + random(160,255);
      //heat[y] = random(160,255);
    }

    // Step 4.  Convert heat to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      setPixelHeatColorAnim(j, heat[j] );
    }

    showStrip();
  }

}

void LEDAnimatedEffects::setPixelHeatColorAnim (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}
void LEDAnimatedEffects::SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay)
{

  unsigned long now = millis();
  int order;
  int Pixel = random(NUM_LEDS);
  
  if(start_sequence)
  {
    order = 0;
    start_sequence = false;
  }
  else
  {
    order = m_effect_state[0];
  }

  if (order == 0)
  {
    setAll(red,green,blue);
    setPixel(Pixel,0xff,0xff,0xff);
    showStrip();

    order = 1;
  }
  else if ( ((now - m_last_iteration) > SparkleDelay) && (order == 1) )
  {
    m_last_iteration = now;
    order = 2;
    setPixel(Pixel,red,green,blue);
    showStrip();
  }
  else if ( ((now - m_last_iteration) > SpeedDelay) && (order == 2) )
  {
    order = 0;
  }

  m_effect_state[0] = order;
}


void LEDAnimatedEffects::rainbowCycle(int SpeedDelay, IMUData imuData)
{
  unsigned long now = millis();
  byte *c;
  int j;
  uint8_t pos = imuData.xPos;
  uint8_t speed = imuData.xSpeed;

  
  if(start_sequence)
  {
    j = 0;
    start_sequence = false;
  }
  else
  {
    j = m_effect_state[0];
  }

  if ( ((now - m_last_iteration) > SpeedDelay) )
  {
    m_last_iteration = now;

    if(j < 256*5)
    {
      j++;
      for(int i=0; i< NUM_LEDS; i++) {
        c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
        setPixel(i, *c, *(c+1), *(c+2));
      }
      showStrip();
    }
    else
    {
      j = 0;
    }
  }
  m_effect_state[0] = j;
}

// used by rainbowCycle and theaterChaseRainbow
byte * LEDAnimatedEffects::Wheel(byte WheelPos)
{
  static byte c[3];

  if(WheelPos < 85)
  {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  }
  else if(WheelPos < 170)
  {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  }
  else
  {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}


// used by meteorrain
void LEDAnimatedEffects::fadeToBlack(int ledNo, byte fadeValue, IMUData imuData) {
 #ifdef ADAFRUIT_NEOPIXEL_H
    // NeoPixel
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;

    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);

    strip.setPixelColor(ledNo, r,g,b);
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[ledNo].fadeToBlackBy( fadeValue );
 #endif
}

// Apply LED color changes
void LEDAnimatedEffects::showStrip()
{
#ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.show();
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
#endif
}
// Set a LED color (not yet visible)
void LEDAnimatedEffects::setPixel(int Pixel, byte red, byte green, byte blue)
{
  if(Pixel >= NUM_LEDS)
  {
    Serial.println("ERROR: Led memory size exceeded");
    Serial.print("Idx ");
    Serial.println(Pixel);
    Pixel = NUM_LEDS - 1;
  }

 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

// Set all LEDs to a given color and apply it (visible)
void LEDAnimatedEffects::setAll(byte red, byte green, byte blue)
{

  for(int i = 0; i < NUM_LEDS; i++ )
  {
    setPixel(i, red, green, blue);
  }
  showStrip();
}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
