#include "MPU9250_SensorValues.h" 

MPU9250_SensorValues::MPU9250_SensorValues()
{
 pos_x = 0;
 pos_y = 0;
 pos_z = 0;
 acc_x = 0;
 acc_y = 0;
 acc_z = 0;
}

MPU9250_SensorValues::~MPU9250_SensorValues()
{}

void MPU9250_SensorValues::ReadMpuSensorValue(MPU_Struct& mpuStruct)
{

  update();
  
  mpuStruct.mPos_x = pos_x;
  mpuStruct.mPos_y = pos_y;
  mpuStruct.mPos_z = pos_z;

  mpuStruct.mAcc_x = acc_x;
  mpuStruct.mAcc_y = acc_y;
  mpuStruct.mAcc_z = acc_z;
}

void MPU9250_SensorValues::begin()
{
  Serial.println("Starting MPU communication handler");
  Serial.begin(115200);

  Wire.begin();

  delay(200);
  mpu.setup();
}

void MPU9250_SensorValues::update()
{

  mpu.update();
  pos_x = abs(mpu.getRoll());
  pos_y = abs(mpu.getPitch());
  pos_z = abs(mpu.getYaw());

  acc_x = abs(mpu.getAcc(0));
  acc_y = abs(mpu.getAcc(1));
  acc_z = abs(mpu.getAcc(2));
   
}



void MPU9250_SensorValues::stop()
{

}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
