#include "WebPages.h"

// Implementation for webserver class is in header. Need WEBDUINO_NO_IMPLEMENTATION defined
// so we don't get multiple instances of it. 
#define WEBDUINO_NO_IMPLEMENTATION 
#include "Webduino.h"

void SendHeader(WebServer &rServer)
{
  rServer.print(F("<!DOCTYPE html><html lang='en'>"));
  rServer.print(F("<head>"));
  rServer.print(F("<meta charset='utf-8' />"));
  rServer.print(F("<meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1' />"));
  rServer.print(F("<meta http-equiv='X-UA-Compatible' content='IE=edge' />"));
  rServer.print(F("<title>Gateway</title>"));
  
  // Bootstrap core.
  rServer.print(F("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css' integrity='sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u' crossorigin='anonymous' />"));
  
  // Bootstrap theme
  rServer.print(F("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css' integrity='sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp' crossorigin='anonymous' />"));

  // Site Styles
  rServer.print(F("<link rel='stylesheet' href='style.css' />"));

  // jQuery
  rServer.print(F("<script src='https://code.jquery.com/jquery-3.1.0.min.js' integrity='sha256-cCueBR6CsyA4/9szpPfrX3s49M9vUU5BgtiJj06wt/s=' crossorigin='anonymous'></script>"));
  
  // Latest compiled and minified JavaScript
  rServer.print(F("<script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js' integrity='sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa' crossorigin='anonymous'></script>"));
  rServer.print(F("</head>"));
}

void SendLoginPage(WebServer &rServer)
{
  Serial.println(F("Login page"));
  SendHeader(rServer);
  rServer.println(F("<body><div class='container'>"));
  rServer.print(F("<form action='LetMeIn.html' method='POST'>"));
  rServer.print(F("<h2>Speak Friend &amp; Enter</h2>"));
  rServer.print(F("<input type='text' id='txtPassword' class='form-control' placeholder='Password' required name='message' autocomplete='off' /> "));
  rServer.print(F("<button class='btn btn-lg btn-primary btn-block' type='submit' name='send' value='Go'/>Login</button>"));
  rServer.print(F("</form>"));
  rServer.print(F("</div></body></html>"));
}

void SendAccessGrantedPage(WebServer &rServer)
{
  SendHeader(rServer);
  rServer.println(F("<body><div class='container'>"));
  rServer.print(F("<form action='LetMeIn.html' method='POST'>"));
  rServer.print(F("<h2>Welcome Friend</h2>"));
  rServer.print(F("<button class='btn btn-lg btn-primary btn-block' type='submit' name='send' value='Activate Door'>Activate Door</button>"));
  rServer.print(F("<a class='btn btn-lg btn-default btn-block' href='index.html' role='button'>Lock</a>"));
  rServer.print(F("<div class='alert alert-success'>The way is <span id='CurrentState' style='font-weight: bold;'>unresolved</span></div>"));
  rServer.print(F("</form>"));
  rServer.print(F("<script>$(function() { "));
  rServer.print(F("function UpdateDoorState() { $.get('doorstate.json', function(data) { $('#CurrentState').text(data.DoorState); }); }"));
  rServer.print(F("UpdateDoorState();"));
  rServer.print(F("setInterval(function() { UpdateDoorState(); }, 1500);"));
  rServer.print(F("})</script>"));
  rServer.print(F("</div></body></html>"));
}

void SendAccessDeniedPage(WebServer &rServer)
{
  SendHeader(rServer);
  rServer.println(F("<body><div class='container'>"));
  rServer.print(F("<h2>None shall pass!</h2>"));
  rServer.print(F("</div></body></html>"));
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

void SendStyles(WebServer &rServer)
{
  Serial.println(F("Styles"));
  rServer.println(F("body {padding-top: 40px;padding-bottom: 40px;background-color: #eee;}"));
  rServer.println(F("form {max-width: 350px;padding: 5px;margin: 0 auto;}"));
  rServer.println(F("h2 {margin-bottom: 10px;}"));
  rServer.println(F(".form-control {position: relative;height: auto;-webkit-box-sizing: border-box;-moz-box-sizing: border-box;box-sizing: border-box;padding: 10px;font-size: 16px;}"));
  rServer.println(F(".form-control:focus {z-index: 2;}"));
  rServer.println(F("input {margin-bottom: 10px;border-top-left-radius: 0;border-top-right-radius: 0;}"));
  rServer.println(F(".alert {margin-top: 20px; text-align: center;}"));
}

void SendDoorState(WebServer &rServer, const char *DoorStateDescription)
{
  Serial.println(F("Get door state"));
  rServer.print(F("{ \"DoorState\" : \""));
  rServer.print(DoorStateDescription);
  rServer.println(F("\"}"));
}

