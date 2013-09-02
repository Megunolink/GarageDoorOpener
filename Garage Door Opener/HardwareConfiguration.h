/* ************************************************************************************************
** Pin assignments
** ************************************************************************************************ */

#pragma once
#include <Arduino.h>

/*
Digital output connected to a relay that activates the door
*/
#define PIN_DOOR 7

/*
Digital output for LED that signals door activation. 
*/
#define PIN_INDICATOR 13

/*
Duration to close the switch on the door opener. This should be long
enough for the mechanism to start; typically it doesn't to remain 
activated for the door to complete its motion. It is the same as the
time you'd hold down the button to start the door moving. 
*/
#define DOOR_ACTIVATION_PERIOD 600 // [ms]

/*
MAC address for the ethernet controller. Preferably globally unique, but
at least unique for the local network. 
*/
#define LOCAL_MAC_ADDRESS { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }

/*
IP address for the arduino. Either gobally unique, or at least unique
on the local network. Should be a statically allocated address (that is
not using DHCP so we know how to connect to the arduino when we want 
to open the door. 
*/
#define LOCAL_IP_ADDRESS { 192, 168, 15, 21};

/*
Passwords are stored in the eeprom in fixed size chunks. 
*/
// Address of first password stored in the eeprom. 
#define PWD_BASE_ADDRESS 100

// Length of each password record
#define PWD_RECORD_LENGTH 10

// Number of passwords stored. 
#define PWD_MAX 10