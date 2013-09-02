#include "AccessControl.h"
#include "HardwareConfiguration.h"

/*
Configures pin mode for the digital output that controls the garage door
opener & sets the to a default (deactivated state). 
*/
void SetupAccessControl()
{
  pinMode(PIN_INDICATOR, OUTPUT);     
  pinMode(PIN_DOOR, OUTPUT);
  digitalWrite(PIN_INDICATOR, LOW);
  digitalWrite(PIN_DOOR, LOW);
}

/*
Briefly triggers the garage door opener & flashes the indicator. 
*/
void ActivateGarageDoor()
{
  Serial.println(F("Door activated."));
  
  digitalWrite(PIN_INDICATOR, HIGH);   // set the LED on
  digitalWrite(PIN_DOOR, HIGH);  // Open door.
  delay(DOOR_ACTIVATION_PERIOD);              
  digitalWrite(PIN_INDICATOR, LOW);    // set the LED off
  digitalWrite(PIN_DOOR, LOW);  // Door will continue to open by itself.
}