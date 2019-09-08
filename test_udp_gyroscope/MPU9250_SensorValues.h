#if !defined MPU9250_SensorValues_H
#define MPU9250_SensorValues_H

#include "MPU9250.h"

class MPU9250_SensorValues
{

private:
  MPU9250 mpu;
  uint16_t pos_x, pos_y, pos_z;
  uint16_t acc_x, acc_y, acc_z;

  void setup();
  void loop();
  
public: 

  MPU9250_SensorValues();
  ~MPU9250_SensorValues();

  void begin();
  void stop();
  void ReadMpuSensorValue(MPU_Struct mpuStruct);

};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
