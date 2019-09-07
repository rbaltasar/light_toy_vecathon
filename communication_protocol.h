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
  uint8_t mode;
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
  
}


#endif
