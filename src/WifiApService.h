#ifndef __WIFIAPSERVICE_H__
#define __WIFIAPSERVICE_H__

#define MANAGE_NETWORK_DELAY 10000
#define DNS_PORT 53

class DNSServer;
class WifiApService
{
public:
    WifiApService();
    void loop();

private:
    /// for the mangement delay loop
    unsigned long _lastManaged;

    /// for the captive portal
    DNSServer* _dnsServer;

    void manageAP();
    void startAP();
    void stopAP();
    void handleDNS();
};

#endif // __WIFIAPSERVICE_H__