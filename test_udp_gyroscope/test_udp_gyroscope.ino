

#include <rom/rtc.h>
# include <WiFi.h>
#include "UDPHandler.h"
#include "MPU9250_SensorValues.h"
#include "network_credentials.h"


/* ------------------------ */
/* --- Global variables --- */
/* ------------------------ */

/* UDP communication handler */
UDPHandler udp_handler;
MPU9250_SensorValues mpu9250_sensorVal;

MPU9250_Struct mpuStruct;

void setup()
{
  
  Serial.begin(115200);

  /* Setup the WiFi connection */
  setup_wifi();
  delay(10);

  /* Start with MQTT communication. Create object */  
  udp_handler.configure();
  mpu9250_sensorVal.begin();
  udp_handler.begin();
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



/* Main loop */
void loop()
{
  mpu9250_sensorVal.ReadMpuSensorValue(mpuStruct);
  udp_handler.network_loop(mpuStruct);
}
