#if !defined UDPHANDLER_H
#define UDPHANDLER_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include "MPU9250_SensorValues.h"
#include "AsyncUDP.h"
#include "communication_protocol.h"


class UDPHandler
{

private:

  /* UDP client */
  AsyncUDP m_UDP;
  /* Intermediate buffer for fast callback handling */
  uint8_t m_message[100];
  IPAddress m_master_ip;

  stick_status* m_stick_status_request;

  bool m_received_msg;
  
public: 

  UDPHandler(stick_status* stick_status_request);
  ~UDPHandler();

  void begin();
  void stop();
  void configure(){}; //Nothing to do but overload needed
  //void network_loop();

 
};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
