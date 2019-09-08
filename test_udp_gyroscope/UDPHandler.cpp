#include "UDPHandler.h" 

UDPHandler::UDPHandler()
{
  m_mpuStruct.mPos_x = 0;
  m_mpuStruct.mPos_y = 0;
  m_mpuStruct.mPos_z = 0;
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
              sprintf(buffer, formato, m_mpuStruct.mPos_x,m_mpuStruct.mPos_y,m_mpuStruct.mPos_z);
              
              Serial.println("Received request. Sending gyro info");
              packet.printf(buffer);
            }
           
            m_received_msg = false;

        });       
    }
}

void UDPHandler::stop()
{
  Serial.println("Stopping UDP communication handler");
  m_UDP.close();
}


void UDPHandler::network_loop(MPU_Struct mpuStruct)
{
  m_mpuStruct = mpuStruct;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
