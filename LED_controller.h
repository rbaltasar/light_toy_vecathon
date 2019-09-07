#if !defined LEDCONTROLLER_H
#define LEDCONTROLLER_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "config.h"
#include <FastLED.h>
#include "common_datatypes.h"
#include "LED_Static_Effects.h"
#include "LED_Music_Effects.h"

class LEDStaticEffects;
class LEDMusicEffects;

class LEDController
{

private:

  CRGB m_leds[NUM_LEDS];
  stick_status* m_stick_status_request;
  uint8_t m_mode;

  LEDStaticEffects* m_static_effects;
  LEDMusicEffects* m_music_effects;

public: 

  LEDController(stick_status* stick_status_request);
  ~LEDController();
  void setup(); 

  void update_color();
  void update_brightness();
  void update_mode();

  void feed();
  void resync();

  void setRGB(RGBcolor color);
  void setRGB(uint8_t R, uint8_t G, uint8_t B);
  void setAllLeds(RGBcolor, unsigned long delay_ms);
  void setLeds(RGBcolor, unsigned long delay_ms,uint8_t num_leds, bool erase_others = true);

  void end_effect();

};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
