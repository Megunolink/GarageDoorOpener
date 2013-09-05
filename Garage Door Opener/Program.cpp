/* ************************************************************************************************
** 
**  Webserver that opens/closes garage door through a relay connected
**  to the door toggle. The relay is connected to a digital output
**  (PIN_DOOR). When the relay closes it causes the door to activate. 
**  
**  Very simple security is implmented: a password is required to 
**  activate the switch. The password is posted as clear-text, however.
**  If the Ardunio is not accesible from a public network, this should
**  be sufficient. Typically security is via physical access and security
**  settings on the wireless network. Use at your own risk.
**  
**  Passwords are saved in the device eeprom (in clear text). They can be
**  changed by a simple serial port protocol:
**    init-pwd\r => initialize storage area in eeprom for passwords
**    list-pwd\r => list known passwords. 
**    set-pwd <Password Slot> <New Password>\r => save a new password. There
**            are 10 password slots numbered 1..10. Passwords may not contain
**            spaces (that is the parameter separation character).
**    clear-pwd <Password Slot>\r => clear the password in the given slot. 
**    
**  
**  IP address of the webserver is hard coded. See HardwareConfiguration.h

** ************************************************************************************************ */

#pragma once
#include <Arduino.h>
#include "HardwareConfiguration.h"
#include "PasswordManager.h"
#include "CommandProcessing.h"
#include "Webserver.h"
#include "AccessControl.h"

// Global password manager object. 
CPasswordManager PasswordManager(PWD_BASE_ADDRESS, PWD_RECORD_LENGTH, PWD_MAX);

void setup()
{
  Serial.begin(9600);

  InitializeCommandProcessing();
  InitializeGarageDoorWebserver();
  SetupAccessControl();  
  
  Serial.println(F("Garage door opener ready."));
}

void loop()
{
  ProcessSerialInput();
  ProcessGarageDoorWebserver();
  PasswordManager.CheckGracePeriod();
}