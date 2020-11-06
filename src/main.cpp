#include <Arduino.h>

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#include "WifiClientService.h"
#include "WifiApService.h"
#include "OtaService.h"

//#include <ESP8266mDNS.h>

#include "jquery.min.gz.h"
//extern const uint8_t *JQueryMinGzFile;

#include "config.h"

ESP8266WebServer server(80);

WifiClientService WifiClientSrv;
WifiApService WifiApSrv;
OtaService OtaSrv;

void getHelloWord()
{
  DynamicJsonDocument doc(512);
  doc["name"] = "Hello world";

  Serial.print(F("Stream..."));
  String buf;
  serializeJson(doc, buf);
  server.send(200, "application/json", buf);
  Serial.print(F("done."));
}

void getSettings()
{
  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use arduinojson.org/v6/assistant to compute the capacity.
  //  StaticJsonDocument<512> doc;
  // You can use DynamicJsonDocument as well
  DynamicJsonDocument doc(512);

  doc["ip"] = WiFi.localIP().toString();
  doc["gw"] = WiFi.gatewayIP().toString();
  doc["nm"] = WiFi.subnetMask().toString();

  if (server.arg("signalStrength") == "true")
  {
    doc["signalStrengh"] = WiFi.RSSI();
  }

  if (server.arg("chipInfo") == "true")
  {
    doc["chipId"] = ESP.getChipId();
    doc["flashChipId"] = ESP.getFlashChipId();
    doc["flashChipSize"] = ESP.getFlashChipSize();
    doc["flashChipRealSize"] = ESP.getFlashChipRealSize();
  }
  if (server.arg("freeHeap") == "true")
  {
    doc["freeHeap"] = ESP.getFreeHeap();
  }

  Serial.print(F("Stream..."));
  String buf;
  serializeJson(doc, buf);
  server.send(200, F("application/json"), buf);
  Serial.print(F("done."));
}

void setRoom()
{
  String postBody = server.arg("plain");
  Serial.println(postBody);

  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, postBody);
  if (error)
  {
    // if the file didn't open, print an error:
    Serial.print(F("Error parsing JSON "));
    Serial.println(error.c_str());

    String msg = error.c_str();

    server.send(400, F("text/html"),
                "Error in parsin json body! <br>" + msg);
  }
  else
  {
    JsonObject postObj = doc.as<JsonObject>();

    Serial.print(F("HTTP Method: "));
    Serial.println(server.method());

    if (server.method() == HTTP_POST)
    {
      if (postObj.containsKey("name") && postObj.containsKey("type"))
      {

        Serial.println(F("done."));

        // Here store data or doing operation

        // Create the response
        // To get the status of the result you can get the http status so
        // this part can be unusefully
        DynamicJsonDocument doc(512);
        doc["status"] = "OK";

        Serial.print(F("Stream..."));
        String buf;
        serializeJson(doc, buf);

        server.send(201, F("application/json"), buf);
        Serial.print(F("done."));
      }
      else
      {
        DynamicJsonDocument doc(512);
        doc["status"] = "KO";
        doc["message"] = F("No data found, or incorrect!");

        Serial.print(F("Stream..."));
        String buf;
        serializeJson(doc, buf);

        server.send(400, F("application/json"), buf);
        Serial.print(F("done."));
      }
    }
  }
}

// Define routing
void restServerRouting()
{
  // handle GET request
  server.on("/", HTTP_GET, []() {
    server.send(200, F("text/html"),
                F("<!DOCTYPE html><html><head> <script src=\"jquery.min.js.gz\"></script> <script>$(document).ready(function(){$(\"button\").click(function(){$(\"p\").hide();});});</script> </head><body><h2>This is a heading</h2><p>This is a paragraph.</p><p>This is another paragraph.</p><button>Click me to hide paragraphs</button></body></html>"));
  });
  server.on(F("/jquery.min.js.gz"), HTTP_GET, []() {
    server.send(200, "text/html", JQueryMinGzFile);
    // addHeader("Content-Encoding", "gzip");
    Serial.print(F("send jquery.min.js.gz done."));
  });
  server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
  server.on(F("/settings"), HTTP_GET, getSettings);

  // handle POST request
  server.on(F("/setRoom"), HTTP_POST, setRoom);
}

// Manage not found URL
void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup()
{
  Serial.begin(115200);

  OtaSrv.begin();
  WifiClientSrv.begin();

  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostname blitzbier.local
  //MDNS.setHostname(HOSTNAME);
  //MDNS.begin(HOSTNAME);
  // Add HTTP service to MDNS
  //MDNS.addService("http", "tcp", 80);

  // Set server routing
  restServerRouting();
  //Set not found response
  server.onNotFound(handleNotFound);
  //Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  WifiApSrv.loop();
  OtaSrv.loop();

  server.handleClient();
  //MDNS.update();
}