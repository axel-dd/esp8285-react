#include <Arduino.h>
#include "WifiClientService.h"
#include "WifiApService.h"
#include "OtaService.h"
#include "WebServerService.h"

WifiClientService WifiClient;
WifiApService WifiAp;
OtaService Ota;
WebServerService WebServer;

void setup()
{
  Serial.begin(115200);

  Ota.begin();
  WifiClient.begin();
  WebServer.begin();
}

void loop()
{
  WifiAp.loop();
  Ota.loop();
}