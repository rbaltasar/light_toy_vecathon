#include "UDPHandler.h" 

UDPHandler::UDPHandler()
{
 m_x = 0;
 m_y = 0;
 m_z = 0;
}

UDPHandler::~UDPHandler()
{

}


void UDPHandler::begin() 
{
  Serial.println("Starting UDP communication handler");

  if(m_UDP.listen(7001)) {

        m_UDP.onPacket([this](AsyncUDPPacket packet) {

            /* Copy message into local buffer */
            noInterrupts();
            memcpy(m_message, packet.data(), packet.length());
            m_received_msg = true;          
            interrupts();
            
            uint8_t msg_id = m_message[0];
            
            if(msg_id != 0x05)
            {
              Serial.println("Received a normal message");

              //Todo: here we must trigger in our system the change requested by the message
            }

            /* This is a IMU information request. Respond with the real data! */
            else
            {

              char buffer[50]=" ";
              char* formato="{\"x\": %i, \"y\": %i, \"z\": %i}";
              sprintf(buffer, formato, m_x, m_y, m_z);
              
              Serial.println("Received request. Sending gyro info");
              packet.printf(buffer);
            }
           
            m_received_msg = false;

            /* Change the simulated IMU data */
            process_message();
        });       
    }
}

/* Just simulate IMU input */
void UDPHandler::process_message()
{
   if(m_x < 359)
  {
    ++m_x;
  }
  else if(m_y < 359)
  {
    ++m_y;
  }
  else if(m_z < 359)
  {
    ++m_z;
  }
  else
  {
    m_x = 0;
    m_y = 0;
    m_z = 0;
  }

}

void UDPHandler::stop()
{
  Serial.println("Stopping UDP communication handler");
  m_UDP.close();
}


void UDPHandler::network_loop()
{
 
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
