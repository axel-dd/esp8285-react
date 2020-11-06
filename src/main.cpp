#include <Arduino.h>
#include <ArduinoJson.h>

#include <ESPAsyncWebServer.h>

#include "WifiClientService.h"
#include "WifiApService.h"
#include "OtaService.h"

//#include <ESP8266mDNS.h>

#include "index.html.gz.h"
#include "jquery.min.js.gz.h"

#include "config.h"

WifiClientService WifiClientSrv;
WifiApService WifiApSrv;
OtaService OtaSrv;
AsyncWebServer server(80);

const char* PARAM_MESSAGE = "message";

const char* CONTENT_TYPE_JS = "application/javascript";
const char* CONTENT_TYPE_HTML = "text/html";

void setup()
{
  Serial.begin(115200);

  OtaSrv.begin();
  WifiClientSrv.begin();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    AsyncWebServerResponse *response = request->beginResponse_P(200, CONTENT_TYPE_HTML, INDEX_HTML_GZ_DATA, INDEX_HTML_GZ_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.on("/" JQUERY_MIN_JS_GZ_FILE, HTTP_GET, [](AsyncWebServerRequest *request)
  {
    AsyncWebServerResponse *response = request->beginResponse_P(200, CONTENT_TYPE_JS, JQUERY_MIN_JS_GZ_DATA, JQUERY_MIN_JS_GZ_SIZE);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });

  server.onNotFound([](AsyncWebServerRequest *request)
  {
    request->send(404, "text/plain", "Not found");
  });

  server.begin();
}

void loop()
{
  WifiApSrv.loop();
  OtaSrv.loop();
}