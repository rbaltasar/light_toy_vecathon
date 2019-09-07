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

/* UDP message identifiers */
enum UDP_Message_Id
{
  MODE_SELECT = 0,
  SYNC_REQ = 1,  
  ACK = 2,
  ERR = 3,
  PAYLOAD_SINGLE = 4,
  PAYLOAD_WINDOW = 5,
  PAYLOAD_FULL = 6,
  CONFIGURATION = 7
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

/* -------- UDP interface description --------- */

/* Generic UDP payload */
struct udp_payload 
{
  uint8_t mask; //To address a payload to specific slaves
  RGBcolor color;  //Color information
  uint8_t amplitude; //Amplitude information
};

/* UDP message structure with single payload */
struct udp_payload_msg
{
  uint8_t msgID;
  udp_payload payload;
};

/* UDP message structure with 6 payload elements */
/* Used to receive color information from up to 6 different spectrum windows */
struct udp_payload_window_spectrum_msg
{
  uint8_t msgID;
  uint8_t numMsg;
  udp_payload payload[NUM_MAX_LAMPS];
};

/* UDP message structure with a payload element for each number of LEDs */
struct udp_payload_full_spectrum_msg
{
  uint8_t msgID;
  RGBcolor* color; //One RGB value for each LED
};

/* UDP message to set a mode */
struct udp_mode_select 
{
  uint8_t msgID;
  uint8_t mode_select;
};

/* UDP message to set a mode */
struct udp_music_mode_configuration 
{
  uint8_t msgID;
  uint8_t effect_delay;
  uint8_t effect_direction;
  RGBcolor base_color;
  uint8_t color_increment;
};

/* UDP synchronization request */
struct udp_sync_req
{
  uint8_t msgID;
  uint8_t delay_ms; //Option to add a delay to the synchronization (currently not supported)
};

/* UDP acknowledgement. Currently not supported */
struct udp_ack 
{
  uint8_t msgID;
  uint8_t nodeID;
  uint8_t ackID;
};


#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
