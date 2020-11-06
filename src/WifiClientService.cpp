#include "WifiClientService.h"
#include <config.h>

WifiClientService::WifiClientService()
{
    _onStationModeGotIPHandler = WiFi.onStationModeGotIP(
        std::bind(&WifiClientService::onStationModeGotIP, this, std::placeholders::_1));
}

void WifiClientService::begin()
{
    Serial.println("Connecting to WiFi...");

    // configure for DHCP
    WiFi.config(INADDR_ANY, INADDR_ANY, INADDR_ANY);
    WiFi.hostname(HOSTNAME);

    // attempt to connect to the network
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void WifiClientService::onStationModeGotIP(const WiFiEventStationModeGotIP &event)
{
    Serial.println("Connected to WiFi: " WIFI_SSID);
    Serial.print("IP: ");
    Serial.println(event.ip);
    Serial.print("Gateway: ");
    Serial.println(event.gw);
}