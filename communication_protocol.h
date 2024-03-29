#if !defined COMMUNICATION_PROTOCOL_H
#define COMMUNICATION_PROTOCOL_H

#include "config.h"
#include "common_datatypes.h"

/* Bluetooth message identifiers */
enum BT_Message_Id
{
  MODE_SELECT = 0,
  COLOR = 1,
  INTENSITY = 2,
  CONFIG = 3,
  IMAGE = 4,
  GYROSCOPE = 5
};

/* -------- Bluetooth interface description --------- */

/* BT message mode select structure */
struct bt_mode_msg
{
  uint8_t msgID;
  uint8_t Mode;
};

/* BT color message */
struct bt_color_msg
{
  uint8_t msgID;
  RGBcolor color;
};

/* BT intensity message */
struct bt_intensity_msg
{
  uint8_t msgID;
  uint8_t intensity;
};

/* BT config message */
struct bt_config_msg
{
  uint8_t msgID;
  uint8_t param1;
  uint8_t param2;
  //etc
};

/* BT image message */
struct bt_image_msg
{
  uint8_t msgID;
  uint8_t msg_size; //Todo: maybe larger datasize
  uint8_t* image_bytes;
};

/* BT gyroscope message */
struct bt_gyroscope_msg
{
  uint8_t msgID;
  uint8_t x;
  uint8_t y;
  uint8_t z;
};

/* */
struct MyReceivedData
{
  bt_mode_msg* ptrMode = NULL;
  bt_color_msg* ptrColor = NULL;
  bt_intensity_msg* ptrIntensity = NULL;
  bt_config_msg* ptrConfig = NULL;
  bt_image_msg* ptrImage = NULL;
};

/* Global variable */
extern MyReceivedData* ptrMyReceivedData; 

#endif
