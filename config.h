
#if !defined CONFIG_H
#define CONFIG_H

//Hardware configuration
#define NUM_LEDS 72
#define IR_PIN 8

#define NUM_MAX_LAMPS 6

#define LED_BUILTIN 2
#define LED_PIN 17
#define COLOR_MODE GRB//GRB

#define R_DEFAULT 255
#define G_DEFAULT 218
#define B_DEFAULT 107

#define INIT_COMM_TIMEOUT 60000
#define ALIVE_PERIOD_MQTT 30000
#define ALIVE_PERIOD_UDP 5000

#define HANDSHAKE_ATTEMPT_INTERVAL 3000

#define AMBIENT_LIGHT_THRESHOLD 30.0
#define AMBIENT_LIGHT_DELAY 150

#define UDP_ALIVE_PORT 7002

//----------- Configuration -------------
//LED configuration
#define VAL_MAX_DB 20
#define RGB_MAX 255
#define RGB_MIN 0
#define Rout 20
#define Gin 12
#define Gout 24
#define Bin 20
#define Bout 32
#define SMOOTHNESS 5
#define MAX_LED_LENGTH 5
#define MAX_BASS_LENGTH 20
#define MAX_VOICE_LENGTH 20
#define BACKGROUND_MODE_3 0
#define BACKGROUND_MODE_1 10

//FFT configuration
#define NSAMPLES 64
#define THRESHOLD 1000
#define SAMPLING_FREQUENCY 200000

//---------- Debugging ------------------
#define DEBUG_UDP 1
//#define DEBUG_TRACES_TIME
//#define DEBUG_TRACES_FREQ
//#define DEBUG_TRACES_FREQ_SPECTRUM
//#define DEBUG_TRACES_RGB
//#define DEBUG_TRACES_LED
//#define DEBUG_TRACES_IR

#endif
