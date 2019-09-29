
#include <rom/rtc.h>
#include <WiFi.h>
#include "LED_controller.h"
#include "UDPHandler.h"
#include "network_credentials.h"

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
/* Gesture recognition object */

/* Communication controller */
UDPHandler udpHandlerObj(&stick_state.val);

unsigned long m_last_time_test = 0;
int directionPosition = 0;  // for debugging

bool m_wifi_connected = false;
bool m_communication_setup = false;

void setup()
{

  Serial.begin(115200);

  /* Setup the WiFi connection */
  setup_wifi();
  delay(10);

  /* Setup the hardware */
  setup_hardware();

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
  stick_state.val.imuData.xVelocity = 0;
  stick_state.val.imuData.xAcc = 0;
  stick_state.val.imuData.yPos = 0;
  stick_state.val.imuData.yVelocity = 0;
  stick_state.val.imuData.yAcc = 0;
  stick_state.val.imuData.zPos = 0;
  stick_state.val.imuData.zVelocity = 0;
  stick_state.val.imuData.zAcc = 0;

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
  stick_state.old.imuData.xVelocity = 0;
  stick_state.old.imuData.yPos = 0;
  stick_state.old.imuData.yVelocity = 0;
  stick_state.old.imuData.zPos = 0;
  stick_state.old.imuData.zVelocity = 0;
  stick_state.old.imuData.xAcc = 0;
  stick_state.old.imuData.yAcc = 0;
  stick_state.old.imuData.zAcc = 0;

  /* Init state */

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
  delay(100);
}

/* Setup WiFi connection */
void check_wifi()
{
  /* There is connection */
  if(WiFi.status() == WL_CONNECTED)
  {
    /* Print IP info only with first connection */
    if(!m_wifi_connected)
    {
      Serial.println('\n');
      Serial.println("Connection established!");
      Serial.print("IP address:\t");
      Serial.println(WiFi.localIP()); // Send the IP address of the ESP8266 to the computer
      Serial.print("MAC address: ");
      Serial.println(WiFi.macAddress());
    }
    m_wifi_connected = true;
  }
  else
  {
    m_wifi_connected = false;
  }  
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
  if(!m_communication_setup)
  {
    udpHandlerObj.begin();
    m_communication_setup = true;
  }
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

/* Handle the communication */
void network_loop()
{
  /* Check for WiFi connection */
  check_wifi();

  /* Setup MQTT */
  if(m_wifi_connected) setup_communication();
}

/* Main loop */
void loop()
{
  /* Network loop */
  network_loop();
  
  /* Check for Gyroscope updates */
  mpu9250_sensorVal.ReadMpuSensorValue(&stick_state.val);

  /* Check for status updates */
  status_update();

  /* Feed the LED controller */
  LED_controller.feed();
}
