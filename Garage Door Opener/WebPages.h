/* ************************************************************************************************
** Write web page content to the webserver. 
** ************************************************************************************************ */

#pragma once
#include <Arduino.h>

// Implementation for webserver class is in header. Need WEBDUINO_NO_IMPLEMENTATION defined
// so we don't get multiple instances of it. 
#define WEBDUINO_NO_IMPLEMENTATION 
#include <Webduino\WebServer.h>

void SendLoginPage(WebServer &rServer);
void SendAccessGrantedPage(WebServer &rServer);
void SendAccessDeniedPage(WebServer &rServer);
void SendErrorPage(WebServer &rServer);
void SendPageNotFound(WebServer &rServer);