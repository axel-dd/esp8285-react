#ifndef __WIFICLIENTSERVICE_H__
#define __WIFICLIENTSERVICE_H__
#include <ESP8266WiFi.h>

#define WIFI_RECONNECTION_DELAY 1000 * 10
#define WIFI_RECONNECTION_ATTEMPT 5

class WifiClientService
{
public:
    WifiClientService();

    void begin();

private:
    WiFiEventHandler _onStationModeGotIPHandler;
    void onStationModeGotIP(const WiFiEventStationModeGotIP &event);
};

#endif // __WIFICLIENTSERVICE_H__