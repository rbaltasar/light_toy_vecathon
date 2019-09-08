#include "MPU9250_SensorValues.h" 

MPU9250_SensorValues::MPU9250_SensorValues()
{
 pos_x = 0;
 pos_y = 0;
 pos_z = 0;
 acc_x = 0;
 acc_y = 0;
 acc_z = 0;
 vel_x = 0;
 vel_y = 0;
 vel_z = 0;
 prev_ms = millis();
}

MPU9250_SensorValues::~MPU9250_SensorValues()
{}

void MPU9250_SensorValues::ReadMpuSensorValue(stick_status* stick_status_request)
{

  update();

  stick_status_request->imuData.xPos = pos_x;
  stick_status_request->imuData.yPos = pos_y;
  stick_status_request->imuData.zPos = pos_z;

  stick_status_request->imuData.xVelocity = vel_x;;
  stick_status_request->imuData.yVelocity = vel_y;
  stick_status_request->imuData.zVelocity = vel_z;

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
  uint32_t curr_ms = millis();
  mpu.update();
  pos_x = abs(mpu.getRoll());
  pos_y = abs(mpu.getPitch());
  pos_z = abs(mpu.getYaw());

  acc_x = mpu.getAcc(0) * 1000;
  acc_y = mpu.getAcc(1) * 1000;
  acc_z = mpu.getAcc(2) * 1000;

  /*
  vel_x += (acc_x * (curr_ms - prev_ms));
  vel_y += (acc_y * (curr_ms - prev_ms));
  vel_z += (acc_z * (curr_ms - prev_ms));
  prev_ms = curr_ms;
  */
}



void MPU9250_SensorValues::stop()
{

}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
