
#if !defined LEDMUSICCEFFECTS_H
#define LEDMUSICCEFFECTS_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "config.h"
#include <FastLED.h>
#include "LED_controller.h"

#define MUSIC_EFFECTS_MEMSIZE 4
#define MAX_PAYLOAD_LOSS 10

/* Forward delcaratio of LEDController class */
class LEDController;

class LEDMusicEffects
{

private:

  /* Member variables */
  LEDController* m_led_controller; //Access to the LED controller functionality
  CRGB* m_leds; //Access to the LED shared memory
  uint8_t music_effect_mem[MUSIC_EFFECTS_MEMSIZE]; //Local memory placeholder to store state information between function calls
  CRGB m_static_color[NUM_LEDS]; //Local LED memory to store static effects
  unsigned long m_last_iteration; //Keep track of the last iteration of a function
  uint8_t m_no_payload_counter;
  float m_smooth_amplitude;
  
  /* Structure to describe a color in HSV format */
  typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
  } hsv;
    
  /* Member private functions */
  void shift_leds(uint8_t led_start, uint8_t led_end, uint8_t positions, const bool top, const uint8_t delay_ms, const uint8_t R_in, const uint8_t G_in, const uint8_t B_in);
  void print_amplitude_color(uint8_t led_start, uint8_t led_end, const bool top, uint8_t amplitude, uint8_t r, uint8_t g, uint8_t b);
  void print_amplitude_static(uint8_t led_start, uint8_t led_end, const bool top, uint8_t amplitude, uint8_t r_base, uint8_t g_base, uint8_t b_base, uint8_t increment);
  void generate_static_colors(uint8_t r_base, uint8_t g_base, uint8_t b_base, uint8_t increment);
  bool is_update(uint8_t r, uint8_t g, uint8_t b, uint8_t amplitude);
  uint8_t compute_bubble_amplitude(uint8_t amplitude);
  RGBcolor hsv2rgb(hsv in);
  hsv rgb2hsv(RGBcolor in_rgb);
  
public:

  LEDMusicEffects(LEDController* led_controller, CRGB* leds_ptr);
  ~LEDMusicEffects();

  /* General purpose functions */
  void end_effect();

  /* Music effects */
  void bubble_effect(uint32_t refresh_rate, uint8_t r, uint8_t g, uint8_t b, uint8_t amplitude, uint8_t direction, bool& isNewPayload);
  void power_bars_effect(uint32_t refresh_rate, uint8_t r, uint8_t g, uint8_t b, uint8_t& amplitude,uint8_t direction, uint8_t effect_type, uint8_t increment);
};

#endif

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
