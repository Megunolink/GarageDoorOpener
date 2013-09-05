#include "WebPages.h"

// Implementation for webserver class is in header. Need WEBDUINO_NO_IMPLEMENTATION defined
// so we don't get multiple instances of it. 
#define WEBDUINO_NO_IMPLEMENTATION 
#include "Libraries\Webduino\WebServer.h"

void SendHeader(WebServer &rServer)
{
  rServer.print(F("<html><head><title>Gateway</title>"));
  rServer.print(F("<meta name='viewport' content='width=320; initial-scale=1.0; maximum-scale=1.0; user-scalable=0;'/>"));
  rServer.print(F("</head>"));
}

void SendLoginPage(WebServer &rServer)
{
  SendHeader(rServer);
  rServer.println(F("<body>"));
  rServer.print(F("<form action='LetMeIn.html' method='POST'>"));
  rServer.print(F("<p> Speak friend, and enter: </p>"));
  rServer.print(F("<p> <input type='text' name='message' autocomplete='off' /> "));
  rServer.print(F("<input type='submit' name='send' value='Go'/> </p>"));
  rServer.print(F("</form>"));
  rServer.print(F("</body></html>"));
}

void SendAccessGrantedPage(WebServer &rServer)
{
  SendHeader(rServer);
  rServer.println(F("<body>"));
  rServer.print(F("<p>Welcome</p>"));
  rServer.print(F("<form action='LetMeIn.html' method='POST'>"));
  rServer.print(F("<input type='submit' name='send' value='Activate Door'/> </p>"));
  rServer.print(F("</form>"));
  rServer.print(F("<form action='index.html' method='GET'>"));
  rServer.print(F("<input type='submit' name='Lock' value='Lock'/> </p>"));
  rServer.print(F("</form>"));
  rServer.print(F("</body></html>"));
}

void SendAccessDeniedPage(WebServer &rServer)
{
  SendHeader(rServer);
  rServer.println(F("<body>"));
  rServer.print(F("<p>None shall pass!</p>"));
  rServer.print(F("</body></html>"));
}

void SendErrorPage(WebServer &rServer)
{
  SendHeader(rServer);
  rServer.println(F("<body><p>Something has gone wrong. Please try later</p></body></html>"));
}

void SendPageNotFound(WebServer &rServer)
{
  SendHeader(rServer);
  rServer.println(F("<body><p>Can't find that here</p></body></html>"));
}