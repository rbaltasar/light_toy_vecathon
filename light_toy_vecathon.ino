
#include <rom/rtc.h>
#include "LED_controller.h"

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

unsigned long m_last_time_test = 0;

void setup()
{
  
  Serial.begin(115200);

  /* Setup the hardware */
  setup_hardware(); 

  /* Setup bluetooth */
  setup_communication();

  /* Initial configuration of the stick when the system is booted */
  stick_state.val.stick_mode = 10 + STROBE;
  stick_state.val.color.R = 0;
  stick_state.val.color.G = G_DEFAULT;
  stick_state.val.color.B = 0;
  stick_state.val.brightness = 1;
  stick_state.val.effect_delay = 50;
  stick_state.val.effect_speed = 50; 
  stick_state.val.effect_amount = 1;
  stick_state.val.sysState = STARTUP;

  /* Same configuration for the "old" variable. TODO: create copy operator */
  stick_state.old.stick_mode = 99;
  stick_state.old.color.R = 0;
  stick_state.old.color.G = G_DEFAULT;
  stick_state.old.color.B = 0;
  stick_state.old.brightness = 1;
  stick_state.old.effect_delay = 50;
  stick_state.old.effect_speed = 50; 
  stick_state.old.effect_amount = 1;

  /* Init state */

  /* Setup finished. Show leds */
  LED_controller.setLeds(stick_state.val.color,0,NUM_LEDS);

  Serial.println("Finished setup");
}

/* Setup the LED controller */
void setup_hardware()
{  
  /* Setup LED controller */
  LED_controller.setup();

  //Setup MPU

  //Setup Gesture Recognition Sensor

}

/* Setup the communication */
void setup_communication()
{
  //Setup the bluetooth controller

  //Be discoverable. Accept connections?
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

  /* Check for status updates */
  status_update();
  /* Check for Gyroscope updates */

  /* Check for Gesture Recognition updates */

  /* Feed the LED controller */
  LED_controller.feed();

  //Initial debug: change effect every 30 seconds
  if( (millis() - m_last_time_test) > 30000 )
  {
    if(stick_state.val.stick_mode == (10 + BOUNCING_COLORED_BALLS)) stick_state.val.stick_mode = 10;
    else stick_state.val.stick_mode++;
    
    m_last_time_test = millis();
  }

}
