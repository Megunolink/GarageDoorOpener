/* ************************************************************************************************
** Write web page content to the webserver. 
** ************************************************************************************************ */

#pragma once
#include <Arduino.h>

class WebServer;

void SendLoginPage(WebServer &rServer);
void SendAccessGrantedPage(WebServer &rServer);
void SendAccessDeniedPage(WebServer &rServer);
void SendErrorPage(WebServer &rServer);
void SendPageNotFound(WebServer &rServer);