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
void LEDAnimatedEffects::FadeInOut(byte red, byte green, byte blue, IMUData& imuData)
{
  int k,order;
  int16_t postitonS = imuData.xPos ;
  Serial.print("PostitonS: ");
  Serial.println(postitonS);
   
  float r, g, b, relativePostion;
  /* Exponencial change */
  relativePostion = (pow(postitonS, 3))/400; //(postitonS / 1.42);
  
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

}

void LEDAnimatedEffects::StrobeAnim(byte red, byte green, byte blue, IMUData& imuData)
{

  unsigned long now = millis();
  int j,order;
  int16_t StrobeCount = imuData.yPos;
  float flashCalc = (float)((imuData.xAcc + imuData.yAcc + imuData.zAcc));
  int16_t FlashDelay = int16_t (1000 - (float)flashCalc);

  Serial.print("FlashDelay ");
  Serial.println(FlashDelay);

  
  if(start_sequence)
  {
    j = 0;
    order = 0;
    start_sequence = false;
  }
  else
  {
    j = m_effect_state[0];
    order = m_effect_state[1];
  }

  if( ((now - m_last_iteration) > FlashDelay) && ( (order == 0) || (order == 1) ) )
  {
    m_last_iteration = now;

    if(order == 0)
    {
      setAll(red,green,blue);
      showStrip();
      order = 1;
    }
    else if(order == 1)
    {
      setAll(0,0,0);
      showStrip();
      if(++j < StrobeCount) order = 0;
      else
      {
        j = 0;
        order = 2;
      }
    }
  }

  else if ( ((now - m_last_iteration) > 30) && (order == 2)  )
  {
    Serial.println("Running Strobe");
    Serial.println(StrobeCount);
    Serial.println(FlashDelay);
    order = 0;
  }


  m_effect_state[0] = j;
  m_effect_state[1] = order;
}


/*
 * The colors of the rainbow will be displayed acording to the position
 * The brightness of the color depends on the speed of the stick.
 */
void LEDAnimatedEffects::RainbowFadeInOut(IMUData& imuData)
{
  int s,p, rIntensity, gIntensity, bIntensity, rSaturation, gSaturation, bSaturation, rPixel, gPixel, bPixel;
  int16_t postitonS = imuData.xPos + imuData.yPos;
  //float velocity = (imuData.xVelocity + imuData.yVelocity )/2;
  float acc = (imuData.xAcc + imuData.yAcc )/2;
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
  s = acc;
 
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

void LEDAnimatedEffects::ColorChangingWithPosition(IMUData& imuData)
{
  uint8_t red = (imuData.xPos <= 255) ? (uint8_t)imuData.xPos : 255;
  uint8_t green = (imuData.yPos <= 255) ? (uint8_t)imuData.yPos : 255;
  uint8_t blue = (imuData.zPos <= 255) ? (uint8_t)imuData.zPos : 255;

  setAll(red , green , blue);   
  showStrip();
}

/* Set a defined number of LEDs with the same color, with or without erasing the non-set leds */
void LEDAnimatedEffects::setLeds(RGBcolor color, unsigned long delay_ms, uint8_t num_leds, bool erase_others)
{
  /* Clip value */
  color.R > 255 ? color.R = 255 : color.R = color.R;
  color.G > 255 ? color.G = 255 : color.G = color.G;
  color.B > 255 ? color.B = 255 : color.B = color.B;
  /* Set requested leds */
  for(uint8_t i = 0; i < num_leds; i++ )
  {
    leds[i] = CRGB(color.R, color.G, color.B);
    FastLED.show();
    delay(delay_ms); 
  }
  /* Eras non-requested led */
  if(erase_others)
  {
    for(uint8_t i = num_leds; i < NUM_LEDS; i++ )
    {
      leds[i] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }
}

void LEDAnimatedEffects::CampMonitor(byte red, byte green, byte blue, IMUData& imuData)
{

  Serial.println("Looping Camp Monitor");

  float last_acc_y;
  float last_acc_x;
  uint8_t num_leds;
  uint8_t divider;
  
  if(start_sequence)
  {
    start_sequence = false;
    num_leds = 0;
    divider = 10;
    last_acc_x = 0;
    last_acc_y = 0;
  }
  else
  {
    num_leds = m_effect_state[0];
    divider = m_effect_state[1];
    last_acc_y = m_effect_state_float[0];
    last_acc_x = m_effect_state_float[1];
  }

  /* Get current IMU data */
  float acc_x = imuData.xAcc;
  float acc_y = imuData.yAcc;

  Serial.print("Acc x: ");
  Serial.println(acc_x);
  Serial.print("Acc y ");
  Serial.println(acc_y);

  /* Check the highest contribution */
  if(abs(acc_x) > abs(acc_y))
  {
    /* Biggest contribution in X --> fade color */
    if( abs(acc_x - last_acc_x) > (acc_x * 0.1) )
    {
      /* Reduce divider */
      if(divider > 2) divider-= 2;
      else divider = 1;
    }
    else
    {
      /* Increment divider */
      if(divider < 20) divider+= 2;
      else divider = 20;
    }
  }
  else
  {
    /* Biggest contribution in Y --> change led amount */
    /* Biggest contribution in X --> fade color */
    if( abs(acc_y - last_acc_y) > abs(acc_y * 0.1) )
    {
      /* Reduce divider */
      if(num_leds < (NUM_LEDS-2)) num_leds+=2;
      else num_leds = NUM_LEDS - 1;
    }
    else
    {
      /* Increment divider */
      if(num_leds > 2) num_leds-=2;
      else num_leds = 0;
    }
  }

  RGBcolor mycolor;
  mycolor.R = red/divider;
  mycolor.G = green/divider;
  mycolor.B = blue/divider;

  Serial.print("Num leds: ");
  Serial.println(num_leds);
  Serial.print("Color ");
  Serial.print(mycolor.R);
  Serial.print(" / ");
  Serial.print(mycolor.G);
  Serial.print(" / ");
  Serial.println(mycolor.B);
  
  setLeds(mycolor,0,num_leds,true);

  m_effect_state_float[0] = acc_y;
  m_effect_state_float[1] = acc_x;
  m_effect_state[0] = num_leds;
  m_effect_state[1] = divider;

}

void LEDAnimatedEffects::TwinkleAnim(byte red, byte green, byte blue, int Count, IMUData& imuData)
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

void LEDAnimatedEffects::TwinkleRandomAnim(int Count, IMUData& imuData)
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

void LEDAnimatedEffects::FireAnim(IMUData& imuData)
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


void LEDAnimatedEffects::rainbowCycle(int SpeedDelay, IMUData& imuData)
{
  unsigned long now = millis();
  byte *c;
  int j;
  uint8_t pos = imuData.xPos;
  uint8_t speed = imuData.xVelocity;

  
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
void LEDAnimatedEffects::fadeToBlack(int ledNo, byte fadeValue, IMUData& imuData) {
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
