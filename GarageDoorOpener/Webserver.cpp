#include "Webserver.h"
#include "HardwareConfiguration.h"
#include "Ethernet.h"
#include "Webduino.h"
#include "WebPages.h"
#include "PasswordManager.h"
#include "AccessControl.h"

// Setup a webserver, on port 80. 
WebServer AccessControlServer("", 80);

// --------------- Prototypes
void ShowWebRoot(WebServer &rServer, WebServer::ConnectionType Type, char *pchUrlTail, bool bTailComplete);
void CheckLogin(WebServer &rServer, WebServer::ConnectionType Type, char *pchUrlTail, bool bTailComplete);
bool IsValidLogin(WebServer &rServer);
void ShowPageNotFound(WebServer &rServer, WebServer::ConnectionType Type, char *pchUrlTail, bool bTailComplete);

// --------------- Initialization and processing. 

void InitializeGarageDoorWebserver()
{
  static uint8_t auLocalMAC[] = LOCAL_MAC_ADDRESS;
  static uint8_t auLocalIPAddress[] = LOCAL_IP_ADDRESS;

  AccessControlServer.setDefaultCommand(ShowWebRoot);
  AccessControlServer.addCommand("LetMeIn.html", CheckLogin);
  AccessControlServer.addCommand("index.html", ShowWebRoot);
  AccessControlServer.setFailureCommand(ShowPageNotFound);

  Ethernet.begin(auLocalMAC, auLocalIPAddress);
  AccessControlServer.begin();
}

void ProcessGarageDoorWebserver()
{
  char achBuffer[64];
  int nBufferLength = sizeof(achBuffer);

  AccessControlServer.processConnection(achBuffer, &nBufferLength);
}

// --------------- Page request handlers
void ShowWebRoot(WebServer &rServer, WebServer::ConnectionType Type, char *pchUrlTail, bool bTailComplete)
{
  // Expire grace login period when the home page is shown. 
  PasswordManager.ClearGracePassword();

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
      SendAccessDeniedPage(rServer);
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
  if (PasswordManager.HasValidPassword())
    return true;

  // Work through all the parameters supplied until we find the one with the
  // password in it. Then check to see if the password is valid. 
  while (rServer.readPOSTparam(achName, sizeof(achName), achValue, sizeof(achValue)))
    if (strcmp(achName, "message") == 0) // we have the password parameter. 
      return PasswordManager.IsPasswordValid(achValue, false);

  return false; // didn't find the password parameter. 
}

void ShowPageNotFound(WebServer &rServer, WebServer::ConnectionType Type, char *pchUrlTail, bool bTailComplete)
{
  // Expire grace login period when invalid page requested
  PasswordManager.ClearGracePassword();

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
