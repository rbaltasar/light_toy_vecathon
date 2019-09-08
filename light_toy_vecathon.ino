
#include <rom/rtc.h>
#include <WiFi.h>
#include "LED_controller.h"
#include "UDPHandler.h"
#include "network_credentials.h"

//Include library for MPU
//Include library for Gesture Recognition Sensor
//Include library for Bluetooth communication

/* ------------------------ */
/* --- Global variables --- */
/* ------------------------ */

/* Shared memory containing the global configuration and state of the stick */
/* This object is shared within the LED Controller and the Communication Handler */
state_tracker<stick_status> stick_state;
/* LED controller object */
LEDController LED_controller(&stick_state.val);

/* MPU object */
MPU9250_SensorValues mpu9250_sensorVal;
MPU9250_Struct mpuStruct;
/* Gesture recognition object */

/* Communication controller */
UDPHandler udpHandlerObj(&stick_state.val);

unsigned long m_last_time_test = 0;
int directionPosition = 0;  // for debugging
  
void setup()
{
  
  Serial.begin(115200);

  /* Setup the WiFi connection */
  setup_wifi();
  delay(10);

  /* Setup the hardware */
  setup_hardware(); 

  /* Setup bluetooth */
  setup_communication();

  /* Initial configuration of the stick when the system is booted */
  stick_state.val.stick_mode =  200; //10 + STROBE;
  stick_state.val.color.R = 0;
  stick_state.val.color.G = G_DEFAULT;
  stick_state.val.color.B = 0;
  stick_state.val.brightness = 1;
  stick_state.val.effect_delay = 50;
  stick_state.val.effect_speed = 50; 
  stick_state.val.effect_amount = 1;
  stick_state.val.sysState = STARTUP;
  stick_state.val.imuData.xPos = 0;
  stick_state.val.imuData.xSpeed = 0;
  stick_state.val.imuData.yPos = 0;
  stick_state.val.imuData.ySpeed = 0;
  stick_state.val.imuData.zPos = 0;
  stick_state.val.imuData.zSpeed = 0;

  /* Same configuration for the "old" variable. TODO: create copy operator */
  stick_state.old.stick_mode = 99;
  stick_state.old.color.R = 0;
  stick_state.old.color.G = G_DEFAULT;
  stick_state.old.color.B = 0;
  stick_state.old.brightness = 1;
  stick_state.old.effect_delay = 50;
  stick_state.old.effect_speed = 50; 
  stick_state.old.effect_amount = 1;
  stick_state.old.imuData.xPos = 0;
  stick_state.old.imuData.xSpeed = 0;
  stick_state.old.imuData.yPos = 0;
  stick_state.old.imuData.ySpeed = 0;
  stick_state.old.imuData.zPos = 0;
  stick_state.old.imuData.zSpeed = 0;  

  /* Init state */

  /* Setup finished. Show leds */
  //LED_controller.setLeds(stick_state.val.color,0,NUM_LEDS);

  Serial.println("Finished setup");
}

/* Get the IP address in String format */
String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}

/* Setup WiFi connection */
void setup_wifi()
{
  WiFi.begin(ssid, password); // Connect to the network. Use network_credentials.h
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  //WiFi.mode(WIFI_STA);
  
  int i = 0;
  while (WiFi.status() != WL_CONNECTED)
  { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

}

/* Setup the LED controller */
void setup_hardware()
{  
  /* Setup LED controller */
  LED_controller.setup();

  //Setup MPU
  mpu9250_sensorVal.begin();

  //Setup Gesture Recognition Sensor

}

/* Setup the communication */
void setup_communication()
{
  udpHandlerObj.begin();
}

/* Update the current stick mode */
/* Switch between communication handlers when necessary */
void mode_update()
{
  Serial.print("Received change request to mode ");
  Serial.println(stick_state.val.stick_mode);

  /* Finish previous effect */
  LED_controller.end_effect();
  
  //Do whatever we need to update the current state
    
  /* Shadow current mode for evaluation of new requests */
  stick_state.old.stick_mode = stick_state.val.stick_mode; 
  
  /* Inform the LED controller about the update */
  LED_controller.update_mode();   
}

/* Check for update requests */
void status_update()
{   
  /* Check difference in mode request */
  if(stick_state.val.stick_mode != stick_state.old.stick_mode)
  {
    mode_update();
  }
  
  /* Check difference in brightness request */
  else if(stick_state.val.brightness != stick_state.old.brightness)
  {
    Serial.print("Received change request to brightness level ");
    Serial.println(stick_state.val.brightness);    

    /* Shadow current change for evaluation of new requests */
    stick_state.old.brightness = stick_state.val.brightness;  
    
    /* Inform the LED controller about the update */
    LED_controller.update_mode();
  }

  /* Check difference in color request */
  else if(stick_state.val.color.R != stick_state.old.color.R || stick_state.val.color.G != stick_state.old.color.G || stick_state.val.color.B != stick_state.old.color.B)
  {
    Serial.print("Received change request to color: ");
    Serial.println(stick_state.val.color.R);
    Serial.println(stick_state.val.color.G);   
    Serial.println(stick_state.val.color.B);  

    /* Shadow current change for evaluation of new requests */
    stick_state.old.color.R = stick_state.val.color.R;
    stick_state.old.color.G = stick_state.val.color.G;
    stick_state.old.color.B = stick_state.val.color.B;
    
    /* Inform the LED controller about the update */
    LED_controller.update_mode();
  }
}

/* Main loop */
void loop()
{
  /* Execute the network loop of the currently used communication handler */
  mpu9250_sensorVal.ReadMpuSensorValue(mpuStruct);
  udpHandlerObj.network_loop(mpuStruct);
  
  /* Check for status updates */
  status_update();
  /* Check for Gyroscope updates */

  /* Check for Gesture Recognition updates */

  /* Feed the LED controller */
  LED_controller.feed();

#if 0
  //Initial debug: change effect every second
  if( (millis() - m_last_time_test) > 10 )
  {

    //////////////////////////////////////
    /* SIMULATION OF MODE; SPPED;POSTION */
    stick_state.val.stick_mode = ptrMyReceivedData->ptrMode->Mode;

    /*
    stick_state.val.imuData.xPos = 
    stick_state.val.imuData.yPos = 
    stick_state.val.imuData.zPos = 

    stick_state.val.imuData.xSpeed =
    stick_state.val.imuData.ySpeeds =
    stick_state.val.imuData.zSpeedPos =
    */
    stick_state.val.color.R = ptrMyReceivedData->ptrColor->color.R;
    stick_state.val.color.G = ptrMyReceivedData->ptrColor->color.G;
    stick_state.val.color.B = ptrMyReceivedData->ptrColor->color.B;
    stick_state.val.brightness = ptrMyReceivedData->ptrIntensity->intensity;
  
   // stick_state.val.imuData.xSpeed = 400;
     
    if((directionPosition == 0) && (stick_state.val.imuData.xPos < 360))
    {
      stick_state.val.imuData.xPos++;
      stick_state.val.imuData.yPos++;
      stick_state.val.imuData.zPos++;
      stick_state.val.imuData.xSpeed++;
      stick_state.val.imuData.ySpeed=+3;
      stick_state.val.imuData.zSpeed=+2;
      if(stick_state.val.imuData.xPos == 360)
      {
        directionPosition = 1;
      }
    }
    else if( directionPosition == 1)
    {
      Serial.print("MIN Speed of Stick: ");
      Serial.println(stick_state.val.imuData.xSpeed);
      stick_state.val.imuData.xPos--;
      stick_state.val.imuData.yPos--;
      stick_state.val.imuData.zPos--;
      stick_state.val.imuData.xSpeed--;
      stick_state.val.imuData.ySpeed=-2;
      stick_state.val.imuData.ySpeed=-3;
      if(stick_state.val.imuData.xPos == 0)
      {
        directionPosition = 0;
      }
    }
    
    //////////////////////////////////////
    /* END OF SIMULATION OF MODE; SPPED;POSTION */
    m_last_time_test = millis();
  }
#endif
}
