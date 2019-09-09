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
 prevAcc_x = 0;
 prevAcc_y = 0;
 prevAcc_z = 0;
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

  acc_x = mpu.getAcc(0) * 1000.0;
  acc_y = mpu.getAcc(1) * 1000.0;
  acc_z = mpu.getAcc(2) * 1000.0;

  Serial.print("curr_ms : ");
  Serial.println(curr_ms);
  Serial.print("prev_ms : ");
  Serial.println(prev_ms);

  float accXDiff = abs(prevAcc_x - acc_x);
  float accYDiff = abs(prevAcc_y - acc_y);
  float accZDiff = abs(prevAcc_z - acc_z);
  
  Serial.print("accXDiff : ");
  Serial.println(accXDiff);
  Serial.print("accYDiff : ");
  Serial.println(accYDiff);
  Serial.print("accZDiff : ");
  Serial.println(accZDiff);

  if(accXDiff < 30 && accYDiff < 30 && accZDiff < 30 ) 
  {
    vel_x = 0;
    vel_y = 0;
    vel_z = 0;
  }
  else
  {    
    vel_x += (mpu.getAcc(0) * (curr_ms - prev_ms));
    vel_y += (mpu.getAcc(1) * (curr_ms - prev_ms));
    vel_z += (mpu.getAcc(2) * (curr_ms - prev_ms));
  }
  
 
  prev_ms = curr_ms;
  prevAcc_x = acc_x;
  prevAcc_y = acc_y;
  prevAcc_z = acc_z;
  
  Serial.print("vel_x : ");
  Serial.println(vel_x);
  Serial.print("vel_y : ");
  Serial.println(vel_y);
  Serial.print("vel_z : ");
  Serial.println(vel_z);
  
}



void MPU9250_SensorValues::stop()
{

}


/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
