#include <DNSServer.h>
#include "WifiApService.h"
#include <ESP8266WiFi.h>
#include "config.h"

WifiApService::WifiApService()
{
    _dnsServer = nullptr;
    _lastManaged = millis();
}

void WifiApService::loop()
{
    unsigned long currentMillis = millis();
    unsigned long manageElapsed = (unsigned long)(currentMillis - _lastManaged);
    if (manageElapsed >= MANAGE_NETWORK_DELAY)
    {
        _lastManaged = currentMillis;
        manageAP();
    }
    handleDNS();
}

void WifiApService::manageAP()
{
    WiFiMode_t currentWiFiMode = WiFi.getMode();
    if (WiFi.status() != WL_CONNECTED)
    {
        if (currentWiFiMode == WIFI_OFF || currentWiFiMode == WIFI_STA)
        {
            startAP();
        }
    }
    else
    {
        if (currentWiFiMode == WIFI_AP || currentWiFiMode == WIFI_AP_STA)
        {
            stopAP();
        }
    }
}

void WifiApService::startAP()
{
    Serial.println("Starting software access point");
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    if (!_dnsServer)
    {
        IPAddress apIp = WiFi.softAPIP();
        Serial.print("Starting captive portal on ");
        Serial.println(apIp);
        _dnsServer = new DNSServer;
        _dnsServer->start(DNS_PORT, "*", apIp);
    }
}

void WifiApService::stopAP()
{
    if (_dnsServer)
    {
        Serial.println("Stopping captive portal");
        _dnsServer->stop();
        delete _dnsServer;
        _dnsServer = nullptr;
    }

    Serial.println("Stopping software access point");
    WiFi.softAPdisconnect(true);
}

void WifiApService::handleDNS()
{
    if (_dnsServer)
    {
        _dnsServer->processNextRequest();
    }
}
