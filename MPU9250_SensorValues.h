#if !defined MPU9250_SensorValues_H
#define MPU9250_SensorValues_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "MPU9250.h"
#include "common_datatypes.h"

class MPU9250_SensorValues
{

private:
  MPU9250 mpu;
  uint16_t pos_x, pos_y, pos_z;
  float acc_x, acc_y, acc_z;
  float vel_x, vel_y, vel_z;
  uint32_t prev_ms;
  void setup();
  void update();
  
public: 

  MPU9250_SensorValues();
  ~MPU9250_SensorValues();

  void begin();
  void stop();
  void ReadMpuSensorValue(stick_status* stick_status_request);

};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
