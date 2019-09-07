#if !defined COMON_DATATYPES_H
#define COMON_DATATYPES_H

#include "config.h"

/* Logical states */
template <typename T>
struct state_tracker
{
  T val;
  T old;
};

/* ------- ENUMS -------- */
/* Status of the system */
enum system_state_var
{
  STARTUP = 0, // Initial communication handshake (MQTT)
  NORMAL = 1 // Normal mode
};

/* Communication type */
enum COMM_TYPE
{
  MQTT = 0,
  UDP = 1
};

/* Music effects */
enum MusicMode
{
  BUBBLE = 0,
  ENERGY_BAR = 1,
  ENERGY_BAR_COLOR = 2
};

/* Static effects */
enum StaticMode
{
  RGB_LOOP = 0,
  FADE_IN_OUT = 1,
  STROBE = 2,
  FIRE = 3,
  HALLOWEEN_EYES = 4,
  CYCLON_BOUNCE = 5,
  TWINKLE = 6,
  TWINKLE_RANDOM = 7,
  SPARKLE = 8,
  SNOW_SPARKLE = 9,
  RUNNING_LIGHTS = 10,
  RAINBOW_CYCLE = 11,
  BOUNCING_COLORED_BALLS = 12
};



/* ------- STRUCTS ------- */
/* Initial communication handshake */
struct init_struct
{
  bool hasStarted; //The handshake has started
  bool isCompleted; //The handshake is completed
  const unsigned long timeout = INIT_COMM_TIMEOUT; //Handshake timeout
  unsigned long elapsed_time; //Handshake timer
};

/* Color info encapsulation */
struct RGBcolor
{
  uint8_t R;
  uint8_t G;
  uint8_t B;
};

/* Gyroscope info encapsulation */
struct IMUData
{
  uint8_t x;
  uint8_t y;
  uint8_t z;
};

/* Lamp main shared memory */
struct stick_status
{
  uint8_t stick_mode; //Current lamp mode
  uint8_t amplitude; //Effect amplitude
  uint8_t brightness; //Effect brightness
  uint8_t deviceID; //Lamp ID
  uint8_t effect_direction = 0;
  uint8_t effect_type = 0;
  uint8_t color_increment = 0;
  RGBcolor color; //Lamp color
  bool resync; //Resynchronization request pending
  bool new_payload; //New message received
  uint32_t effect_delay; //Effect delay
  uint32_t effect_speed; //Effect speed
  uint32_t effect_amount; //Generic configuration parameter available for different effects. Meaning may vary.  
  system_state_var sysState; //System state
};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
