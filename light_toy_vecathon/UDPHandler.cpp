#include "UDPHandler.h" 

#include <stdio.h>
#include <stdlib.h>



UDPHandler::UDPHandler()
{
  /*
  m_mpuStruct.mPos_x = 0;
  m_mpuStruct.mPos_y = 0;
  m_mpuStruct.mPos_z = 0;
  */
}

UDPHandler::~UDPHandler()
{

}

void ComGet_ModeSelect(uint8_t* data)
{
  ptrMyReceivedData->ptrMode->msgID = data[0];
  ptrMyReceivedData->ptrMode->Mode = data[1];
}

void ComGet_Color(uint8_t* data)
{
  ptrMyReceivedData->ptrColor->msgID = data[0];
  ptrMyReceivedData->ptrColor->color.R = data[1];
  ptrMyReceivedData->ptrColor->color.G = data[2];
  ptrMyReceivedData->ptrColor->color.B = data[3];
}

void ComGet_Intensity(uint8_t* data)
{
  ptrMyReceivedData->ptrIntensity->msgID = data[0];
  ptrMyReceivedData->ptrIntensity->intensity = data[1];
}

void ComGet_Config(uint8_t* data)
{
  ptrMyReceivedData->ptrConfig->msgID = data[0];
  ptrMyReceivedData->ptrConfig->param1 = data[1];
  ptrMyReceivedData->ptrConfig->param2 = data[2];
}

void ComGet_Image(uint8_t* data)
{
  ptrMyReceivedData->ptrImage->msgID = data[0];
  ptrMyReceivedData->ptrImage->msg_size = data[1];
  ptrMyReceivedData->ptrImage->image_bytes = data+2;
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
            
            switch (msg_id)
            {
              case MODE_SELECT:
                ComGet_ModeSelect(m_message);
              break;
              case COLOR:
                ComGet_Color(m_message);
              break;
              case INTENSITY:
                ComGet_Intensity(m_message);
              break;
              case CONFIG:
                ComGet_Config(m_message);
              break;
              case IMAGE:
                ComGet_Image(m_message);
              break;
              default:
              /* Do Nothing */
              break;
            }
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
             // sprintf(buffer, formato, m_mpuStruct.mPos_x,m_mpuStruct.mPos_y,m_mpuStruct.mPos_z);
              
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

/*
void UDPHandler::network_loop(MPU_Struct mpuStruct)
{
  m_mpuStruct = mpuStruct;
}
*/
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
