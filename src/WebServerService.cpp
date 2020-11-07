#include <ESPAsyncWebServer.h>
#include "WebServerService.h"
#include "WebServerFiles/Headers.h"

#define FILE_ROUTING(FILE) \
server.on(FILE##_URI, HTTP_GET, [&](AsyncWebServerRequest *request) \
{ \
  AsyncWebServerResponse *response = request->beginResponse_P(200, FILE##_TYPE, FILE##_DATA, FILE##_SIZE); \
  response->addHeader("Content-Encoding", "gzip"); \
  request->send(response); \
});

AsyncWebServer server(80);

void WebServerService::begin()
{
  #include "WebServerFiles/FileRoutings.inl"

  server.onNotFound([](AsyncWebServerRequest *request)
  {
    request->send(404, "text/plain", "Not found");
  });

  server.begin();
}
