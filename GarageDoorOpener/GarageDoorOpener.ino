/* *************************************************************************
 *  Garage door opener. Provides a web interface to trigger a garage door to 
 *  open/close. Passwords can be updated using a MegunoLink interface panel.
 */
#include "Ethernet.h"
#include "Webduino.h"

#include "CommandHandler.h" // see: http://www.megunolink.com/documentation/arduino-libraries/serial-command-handler/
#include "EEPROMStore.h" // see: http://www.megunolink.com/documentation/arduino-libraries/eepromstore/
#include "ArduinoTimer.h" // see: http://www.megunolink.com/documentation/arduino-libraries/arduino-timer/

#include "WebPages.h"

/* -----------------------------------------------------
 *  Hardware configuration
 */

// Digital input for door states. Inputs have pull-ups and are active low. 
const uint8_t DoorOpenPin = 6;
const uint8_t DoorClosedPin = 5;

// Digital output connected to a relay that activates the door
const uint8_t DoorControlPin = 7;

// Digital output for LED that signals door activation. 
const uint8_t IndicatorPin = 13;

// Duration to close the switch on the door opener. This should be long
// enough for the mechanism to start; typically it doesn't to remain 
// activated for the door to complete its motion. It is the same as the
// time you'd hold down the button to start the door moving. 
const uint8_t DoorActivationPeriod = 600; // [ms]

// MAC address for the ethernet controller. Preferably globally unique, but
// at least unique for the local network. 
byte MyMACAddress[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xDE };

// IP address for the arduino. Either gobally unique, or at least unique
// on the local network. Should be a statically allocated address (that is
// not using DHCP so we know how to connect to the arduino when we want 
// to open the door. 
IPAddress MyIPAddress( 192, 168, 15, 22);


/* -----------------------------------------------------
 *  Password and setting storage
 */
const uint8_t MaxPasswordLength = 15;
const uint8_t PasswordSlots = 5;

bool HadValidPasswordRecently = false; 

// Expire the users password after a little while 
const uint32_t MaxPasswordAge = 5*60; // seconds
ArduinoTimer AgeOfValidPassword; 

struct PasswordData
{
  char Passwords[PasswordSlots][MaxPasswordLength + 1];

  void Reset()
  {
    memset(Passwords, 0, sizeof(Passwords));
  }
}; 

EEPROMStore<PasswordData> PasswordStore;

// Check the supplied password against all the ones we know of. Return true
// if the password is valid and false otherwise. 
bool IsPasswordValid(const char *TestPassword)
{
  for(int i = 0; i < PasswordSlots; ++i)
  {
    if (strcmp(PasswordStore.Data.Passwords[i], TestPassword) == 0)
    {
      Serial.println(F("Matched valid password"));
      HadValidPasswordRecently = true; 
      AgeOfValidPassword.Reset();
      return true; 
    }
  }

  Serial.println(F("Invalid password"));
  return false; 
}

// Returns true if a valid password has been entered recently. 
bool HasValidPassword()
{
  if (HadValidPasswordRecently && AgeOfValidPassword.TimePassed_Seconds(MaxPasswordAge))
  {
    HadValidPasswordRecently = false; 
  }

  return HadValidPasswordRecently;
}

/* -----------------------------------------------------
 *  Hardware Support
 */


enum DoorState
{
  // Garage door open sensor is triggered
  Open,

  // Door closed sensor is activated
  Closed,

  // Neither sensor is active. 
  Unknown,
};

void InitializeHardware();
DoorState GetDoorState();
void ActivateGarageDoor();

void InitializeHardware()
{
  pinMode(DoorControlPin, OUTPUT);
  digitalWrite(DoorControlPin, LOW);

  pinMode(IndicatorPin, OUTPUT);
  digitalWrite(IndicatorPin, LOW);

  pinMode(DoorOpenPin, INPUT_PULLUP);
  pinMode(DoorClosedPin, INPUT_PULLUP);
}

void ActivateGarageDoor()
{
  Serial.println(F("Door activated"));
  
  digitalWrite(DoorControlPin, HIGH);
  digitalWrite(IndicatorPin, HIGH);
  delay(DoorActivationPeriod);
  digitalWrite(DoorControlPin, LOW);
  digitalWrite(IndicatorPin, LOW);
}


DoorState GetDoorState()
{
  if (digitalRead(DoorOpenPin) == 0)
  {
    return Open;
  }
  else if (digitalRead(DoorClosedPin) == 0)
  {
    return Closed;
  }

  return Unknown;
}

/* -----------------------------------------------------
 *  Handling serial commands
 */
CommandHandler<> SerialCommands;

void InitializeSerialCommands()
{
  SerialCommands.AddCommand(F("SetPass"), Cmd_SetPassword);
  SerialCommands.AddCommand(F("ListPass"), Cmd_ListPasswords);
  SerialCommands.AddCommand(F("ActivateDoor"), Cmd_ActivateDoor);
}

void Cmd_ListPasswords(CommandParameter &p)
{
  Serial.println(F("Passwords"));
  Serial.println(F("---------"));
  for(int i = 0; i < PasswordSlots; ++i)
  {
    Serial.print(i);
    Serial.print(F(": "));
    if (PasswordStore.Data.Passwords[i][0] == '\0')
    {
      Serial.println(F("(empty)"));
    }
    else
    {
      Serial.println(PasswordStore.Data.Passwords[i]);
    }
  }
}

void Cmd_SetPassword(CommandParameter &p)
{
  unsigned Slot = p.NextParameterAsInteger(-1);

  if (Slot >= PasswordSlots)
  {
    Serial.print(F("Invalid password slot. Slots available: 0.."));
    Serial.println(PasswordSlots - 1);
    return;
  }

  char *Password = p.NextParameter();
  unsigned Length = strlen(Password);
  if (Length > MaxPasswordLength)
  {
    Serial.print(F("Password is too long. Maximum length is: "));
    Serial.println(MaxPasswordLength);
    return;
  }

  // Clear old password & make sure null terminator is present then
  // save new password. 
  memset(PasswordStore.Data.Passwords[Slot], 0, MaxPasswordLength + 1);
  memcpy(PasswordStore.Data.Passwords[Slot], Password, Length);
  PasswordStore.Save();

  Serial.print(F("Password slot "));
  Serial.print(Slot);
  Serial.println(F(" updated"));
}

void Cmd_ActivateDoor(CommandParameter &p)
{
  Serial.println(F("Activating door"));
  ActivateGarageDoor();
}

/* -----------------------------------------------------
 *  Handle commands for webserver
 */
WebServer AccessControlServer("", 80);

void InitializeWebserver()
{
  AccessControlServer.setDefaultCommand(ShowWebRoot);
  AccessControlServer.addCommand("LetMeIn.html", CheckLogin);
  AccessControlServer.addCommand("index.html", ShowWebRoot);
  AccessControlServer.addCommand("style.css", SendStyles);
  AccessControlServer.addCommand("doorstate.json", GetDoorState);
  AccessControlServer.setFailureCommand(ShowPageNotFound);

  Ethernet.begin(MyMACAddress, MyIPAddress);
  AccessControlServer.begin();
}

void ProcessWebserver()
{
  char achBuffer[64];
  int nBufferLength = sizeof(achBuffer);

  AccessControlServer.processConnection(achBuffer, &nBufferLength);
}

// --------------- Page request handlers
void ShowWebRoot(WebServer &rServer, WebServer::ConnectionType Type, char *pchUrlTail, bool bTailComplete)
{
  Serial.println(F("Show web root"));
  
  // Expire grace login period when the home page is shown. 
  HadValidPasswordRecently = false;

  // We show the password entry page as the web root. Presents a form that asks for a password.
  rServer.httpSuccess();

  switch (Type)
  {
  case WebServer::GET:
    SendLoginPage(rServer);
    break;

  case WebServer::POST:
    SendErrorPage(rServer);
    break;

    // None of these are expected, so we don't respond. 
  case WebServer::INVALID:
  case WebServer::HEAD:
  case WebServer::PUT:
  case WebServer::DELETE:
  case WebServer::PATCH:
  default:
    break;
  }
}

void SendStyles(WebServer &rServer, WebServer::ConnectionType Type, char *pchUrlTail, bool bTailComplete)
{
  rServer.httpSuccess("text/css; charset=utf-8");

  switch (Type)
  {
  case WebServer::GET:
  case WebServer::POST:
    SendStyles(rServer);
    break;

    // None of these are expected, so we don't respond. 
  case WebServer::INVALID:
  case WebServer::HEAD:
  case WebServer::PUT:
  case WebServer::DELETE:
  case WebServer::PATCH:
  default:
    break;
  }
}

void GetDoorState(WebServer &rServer, WebServer::ConnectionType Type, char *pchUrlTail, bool bTailComplete)
{
  rServer.httpSuccess("application/json; charset=utf-8", "Pragma: no-cache\r\nCache-Control: no-cache\r\n");

  switch (Type)
  {
  case WebServer::GET:
  case WebServer::POST:
    {
      const char *DoorStateDescription = "unknown";
      DoorState CurrentState = GetDoorState();
      if (CurrentState == Open)
      {
        DoorStateDescription = "open";
      }
      else if (CurrentState == Closed)
      {
        DoorStateDescription = "closed";
      }
      SendDoorState(rServer, DoorStateDescription);
    }
    break;

    // None of these are expected, so we don't respond. 
  case WebServer::INVALID:
  case WebServer::HEAD:
  case WebServer::PUT:
  case WebServer::DELETE:
  case WebServer::PATCH:
  default:
    break;
  } 
}

void CheckLogin(WebServer &rServer, WebServer::ConnectionType Type, char *pchUrlTail, bool bTailComplete)
{

  rServer.httpSuccess();

  switch (Type)
  {
  case WebServer::GET:
    SendErrorPage(rServer);
    break;

  case WebServer::POST:
    if (IsValidLogin(rServer))
    {
      ActivateGarageDoor();
      SendAccessGrantedPage(rServer);
    }
    else
    {
      SendAccessDeniedPage(rServer);
    }
    break;

    // None of these are expected, so we don't respond. 
  case WebServer::INVALID:
  case WebServer::HEAD:
  case WebServer::PUT:
  case WebServer::DELETE:
  case WebServer::PATCH:
  default:
    break;
  }
}

bool IsValidLogin(WebServer &rServer)
{
  const int nMaxParameterBuffer = 20;
  char achName[nMaxParameterBuffer];
  char achValue[nMaxParameterBuffer];

  // Check if there is a recent password & we are still within grace period. 
  if (HasValidPassword())
    return true;

  // Work through all the parameters supplied until we find the one with the
  // password in it. Then check to see if the password is valid. 
  while (rServer.readPOSTparam(achName, sizeof(achName), achValue, sizeof(achValue)))
    if (strcmp(achName, "message") == 0) // we have the password parameter. 
      return IsPasswordValid(achValue);

  return false; // didn't find the password parameter. 
}

void ShowPageNotFound(WebServer &rServer, WebServer::ConnectionType Type, char *pchUrlTail, bool bTailComplete)
{
  // Expire grace login period when invalid page requested
  HadValidPasswordRecently = false;

  // We show the password entry page as the web root. Presents a form that asks for a password.
  rServer.httpSuccess();

  switch (Type)
  {
  case WebServer::GET:
  case WebServer::POST:
    SendPageNotFound(rServer);
    break;

    // None of these are expected, so we don't respond. 
  case WebServer::INVALID:
  case WebServer::HEAD:
  case WebServer::PUT:
  case WebServer::DELETE:
  case WebServer::PATCH:
  default:
    break;
  }
}

/* -----------------------------------------------------
 *  Program entry points.
 */
 void setup() 
{
  Serial.begin(9600);
  Serial.println(F("Garage Door Controller"));
  Serial.print(F("Built: ")); Serial.println(F(__TIMESTAMP__));

  InitializeHardware();
  InitializeSerialCommands();
  PasswordStore.Load();
  InitializeWebserver();
}

void loop() 
{
  SerialCommands.Process();
  ProcessWebserver();
}
