
#include "LED_controller.h"

LEDController::LEDController(stick_status* stick_status_request):
m_stick_status_request(stick_status_request)
{
  /* Create static effects object */
  m_static_effects = new LEDStaticEffects(this, m_leds);
  /* Create music effects object */
  m_music_effects = new LEDMusicEffects(this, m_leds);
  /* Create animated effects object */
  m_animated_effects = new LEDAnimatedEffects(this, m_leds);
}

LEDController::~LEDController()
{
  /* Deallocate memory */
  delete m_static_effects;
  delete m_music_effects;
  delete m_animated_effects;
}

/* Hardware setup */
void LEDController::setup()
{
  /* Configure LED stripe */
  FastLED.addLeds<WS2812, LED_PIN, GRB>(m_leds, NUM_LEDS);
  
  /* Reset color */
  setAllLeds( (RGBcolor){0,0,0}, 0 );
}

/* Set all leds with the same color */
void LEDController::setAllLeds(RGBcolor color, unsigned long delay_ms)
{
  setLeds(color,delay_ms,NUM_LEDS);
}

/* Set a defined number of LEDs with the same color, with or without erasing the non-set leds */
void LEDController::setLeds(RGBcolor color, unsigned long delay_ms, uint8_t num_leds, bool erase_others)
{
  /* Clip value */
  color.R > 255 ? color.R = 255 : color.R = color.R;
  color.G > 255 ? color.G = 255 : color.G = color.G;
  color.B > 255 ? color.B = 255 : color.B = color.B;
  /* Set requested leds */
  for(uint8_t i = 0; i < num_leds; i++ )
  {
    m_leds[i] = CRGB(color.R, color.G, color.B);
    FastLED.show();
    delay(delay_ms); 
  }
  /* Eras non-requested led */
  if(erase_others)
  {
    for(uint8_t i = num_leds; i < NUM_LEDS; i++ )
    {
      m_leds[i] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }
}

/* Update the color for all leds */
void LEDController::update_color()
{
  setAllLeds(m_stick_status_request->color,0);
} 

/* Update the LED stripe for static and dynamic effects */
void LEDController::feed()
{
  /* Static effects - Range [10,99] */
  if(m_mode >= 10 && m_mode < 99)
  {
    switch(m_mode - 10)
    {
      case RGB_LOOP:
        m_static_effects->RGBLoop();
        break;
      case FADE_IN_OUT:
        m_static_effects->FadeInOut(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B);
        break;
      case STROBE:
        m_static_effects->Strobe(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B,50, m_stick_status_request->effect_speed, m_stick_status_request->effect_delay);
        break;
      case FIRE:
        m_static_effects->Fire(55, 120,m_stick_status_request->effect_speed);
        break;
      case HALLOWEEN_EYES:
        m_static_effects->HalloweenEyes(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, NUM_LEDS/5, 2, true, 1, m_stick_status_request->effect_speed,  m_stick_status_request->effect_delay);
        break;         
      case CYCLON_BOUNCE:
        m_static_effects->CylonBounce(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, NUM_LEDS/5, m_stick_status_request->effect_speed, m_stick_status_request->effect_delay);
        break;
      case TWINKLE:
        m_static_effects->Twinkle(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, 16, m_stick_status_request->effect_speed, false);
        break;
      case TWINKLE_RANDOM:
        m_static_effects->TwinkleRandom(16, m_stick_status_request->effect_speed, false);
        break;
      case SPARKLE:
        m_static_effects->Sparkle(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->effect_speed);
        break;
      case SNOW_SPARKLE:
        m_static_effects->SnowSparkle(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->effect_speed, m_stick_status_request->effect_delay);
        break;
      case RUNNING_LIGHTS:
        m_static_effects->RunningLights(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->effect_speed);
        break;
      case RAINBOW_CYCLE:
         m_static_effects->rainbowCycle(m_stick_status_request->effect_speed);
         break;
      case BOUNCING_COLORED_BALLS:
      {
         byte color[10][3] = { {m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B},
                                    {m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->color.R},
                                    {m_stick_status_request->color.B, m_stick_status_request->color.R, m_stick_status_request->color.G},
                                    {m_stick_status_request->color.R, m_stick_status_request->color.R, m_stick_status_request->color.G},
                                    {m_stick_status_request->color.G, m_stick_status_request->color.G, m_stick_status_request->color.R},
                                    {m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.R},
                                    {m_stick_status_request->color.B, m_stick_status_request->color.B, m_stick_status_request->color.G},
                                    {m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.R},
                                    {m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->color.G},
                                    {m_stick_status_request->color.R, m_stick_status_request->color.B, m_stick_status_request->color.B}};
         m_static_effects->BouncingColoredBalls(m_stick_status_request->effect_amount, color, true, 0);
         break;
      }    
    }
  }
  /* Music effects - Range >= 100 */
  else if(m_mode >= 100 && m_mode < 200 )
  {
     switch(m_mode - 100)
    {
      case BUBBLE:
        m_music_effects->bubble_effect(m_stick_status_request->effect_delay, m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->amplitude, m_stick_status_request->effect_direction,m_stick_status_request->new_payload);
        break;
      case ENERGY_BAR:
        m_music_effects->power_bars_effect(m_stick_status_request->effect_delay, m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->amplitude, m_stick_status_request->effect_direction, 0, 0);
        break;
      case ENERGY_BAR_COLOR:
        m_music_effects->power_bars_effect(m_stick_status_request->effect_delay, m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->amplitude, m_stick_status_request->effect_direction, 1, m_stick_status_request->color_increment);
        break;
    }
  }
  /* Music effects - Range >= 100 */
  else if(m_mode >= 200 || m_mode < 300 )
  {
     switch(m_mode)
    {
      case 200:
         m_static_effects->rainbowCycle(m_stick_status_request->effect_speed);
         break;
      case FADE_IN_OUT_POSITION:
        m_animated_effects->FadeInOut(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->imuData );
        break;
      case RAINBOW_FADE_POSITION_SPEED:       
        m_animated_effects->RainbowFadeInOut(m_stick_status_request->imuData);
        break;
      case COLOR_CHANGE_WITH_POSITION:       
        m_animated_effects->ColorChangingWithPosition(m_stick_status_request->imuData);
        break;      
      case TWINKLE_WITH_SPEED:
        m_animated_effects->TwinkleAnim(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, 16, m_stick_status_request->imuData);
        break;
      case TWINKLE_RANDOM_WITH_SPEED:
        m_animated_effects->TwinkleRandomAnim(16, m_stick_status_request->imuData);
        break;
      case FIRE_ANIM:
       m_animated_effects->FireAnim(m_stick_status_request->imuData);
        break;
       case STROBE_ANIM:
        m_animated_effects->StrobeAnim(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->imuData);
        break;
       case CAMP_MONITOR:
        m_animated_effects->CampMonitor(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->imuData);
        break;
       case NUMBER_LEDS_POSITION: 
        m_animated_effects->LEDNumberChangingWithPosition(m_stick_status_request->color.R, m_stick_status_request->color.G, m_stick_status_request->color.B, m_stick_status_request->imuData);
        break;
    }
  }  
}
  
/* Update the LED mode */
void LEDController::update_mode()
{
  /* Set local mode */
  m_mode = m_stick_status_request->stick_mode;

  /* Handle special cases (no effect) - Range[0,9] */
  switch(m_mode)
  {
    case 0: //Switch Off
      setAllLeds( (RGBcolor){0,0,0}, 0 );
      break;
    case 1: //Switch On (with brightness
      setAllLeds( (RGBcolor){m_stick_status_request->color.R / m_stick_status_request->brightness,m_stick_status_request->color.G / m_stick_status_request->brightness,m_stick_status_request->color.B / m_stick_status_request->brightness}, 0 );
      break;    
  }
}

/* Cleanup and finish current effect */
void LEDController::end_effect()
{
  m_static_effects->end_effect();
  m_music_effects->end_effect();
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
