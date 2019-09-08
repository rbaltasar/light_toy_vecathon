#include "UDPHandler.h" 

#include <stdio.h>
#include <stdlib.h>



UDPHandler::UDPHandler(stick_status* stick_status_request):
m_stick_status_request(stick_status_request)
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

                Serial.print("Handled mode request message: ");
                Serial.println(m_message[1]);
                m_stick_status_request->stick_mode = m_message[1];
                
              break;
              case COLOR:

                Serial.print("Handled color request message: ");
                Serial.println(m_message[1]);
                Serial.println(m_message[2]);
                Serial.println(m_message[3]);
                
                m_stick_status_request->color.R = m_message[1];
                m_stick_status_request->color.G = m_message[2];
                m_stick_status_request->color.B = m_message[3];
              break;
              case INTENSITY:

                Serial.print("Handled brightness request message: ");
                Serial.println(m_message[1]);
                m_stick_status_request->brightness = m_message[1];
              break;
              case CONFIG:
                Serial.println("Config message. Nothing to do");
              break;
              case IMAGE:
                Serial.println("Image message. Nothing to do");
         
              break;
              case GYROSCOPE:
              {
                Serial.println("Gyroscope request");
                char buffer[50]=" ";
                char* formato="{\"x\": %i, \"y\": %i, \"z\": %i}";
                sprintf(buffer, formato, m_mpuStruct.mPos_x,m_mpuStruct.mPos_y,m_mpuStruct.mPos_z);
              
                //Answer
                packet.printf(buffer);
              break;
              }
              default:
              /* Do Nothing */
              break;
            }
        });

      
    }

    Serial.println("UDP setup complete");
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
