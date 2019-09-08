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

void MPU9250_SensorValues::ReadMpuSensorValue(stick_status* stick_status_request)
{

  update();

  
  
  stick_status_request->imuData.xPos = pos_x;
  stick_status_request->imuData.yPos = pos_y;
  stick_status_request->imuData.zPos = pos_z;

  stick_status_request->imuData.xAcc = acc_x;
  stick_status_request->imuData.yAcc = acc_y;
  stick_status_request->imuData.zAcc = acc_z;
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

  acc_x = mpu.getAcc(0) * 1000;
  acc_y = mpu.getAcc(1) * 1000;
  acc_z = mpu.getAcc(2) * 1000;

  mpu.printRawData();
}



void MPU9250_SensorValues::stop()
{

}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
