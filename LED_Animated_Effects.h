
#if !defined LEDANIMATEDEFFECTS_H
#define LEDANIMATEDEFFECTS_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "config.h"
#include <FastLED.h>
#include "LED_controller.h"
#include "LED_Uti.h"
#include "LED_Static_Effects.h"
#include "common_datatypes.h"

class LEDController;

class LEDAnimatedEffects
{

private:

  LEDController* m_led_controller;
  CRGB* leds;

  int m_effect_state[3];
  float m_effect_state_float[3];
  bool start_sequence;
  unsigned long m_last_iteration;

  // Apply LED color changes
  void showStrip();
  // Set a LED color (not yet visible)
  void setPixel(int Pixel, byte red, byte green, byte blue);
  // Set all LEDs to a given color and apply it (visible)
  void setAll(byte red, byte green, byte blue);
  void setPixelHeatColor (int Pixel, byte temperature);

  void setLeds(RGBcolor, unsigned long delay_ms,uint8_t num_leds, bool erase_others = true);

  byte * Wheel(byte WheelPos);


public:

  LEDAnimatedEffects(LEDController* led_controller, CRGB* leds_ptr);
  ~LEDAnimatedEffects();

  void end_effect();

  void fade_to_color(RGBcolor target_color, uint8_t delay_ms);

  void RGBLoop();
  void FadeInOut(byte red, byte green, byte blue, IMUData& imuData);
  void RainbowFadeInOut(IMUData& imuData);
  void ColorChangingWithPosition(IMUData& imuData);
  void TwinkleAnim(byte red, byte green, byte blue, int Count, IMUData& imuData);
  void TwinkleRandomAnim(int Count, IMUData& imuData);
  void FireAnim(IMUData& imuData);
  void CampMonitor(byte red, byte green, byte blue, IMUData& imuData);
  void StrobeAnim(byte red, byte green, byte blue, IMUData& imuData);
   

  void HalloweenEyes(byte red, byte green, byte blue, int EyeWidth, int EyeSpace, bool Fade, int Steps, int FadeDelay, int EndPause);
  void Twinkle(byte red, byte green, byte blue, int Count, int SpeedDelay, bool OnlyOne);

  void SnowSparkle(byte red, byte green, byte blue, int SparkleDelay, int SpeedDelay);
  void rainbowCycle(int SpeedDelay, IMUData& imuData);
  void fadeToBlack(int ledNo, byte fadeValue, IMUData& imuData);
  void setPixelHeatColorAnim (int Pixel, byte temperature);

};

#endif

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
