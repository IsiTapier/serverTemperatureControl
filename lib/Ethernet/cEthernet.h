#ifndef cEthernet_h
#define cEthernet_h

#include <ETH.h>
#include "../Util.h"

class cEthernet
{
private:
    static void setupAsyncElegantOTA();
    static void WiFiEvent(WiFiEvent_t event);
    static void testClient(const char * host, uint16_t port);
public:
    static void setup();
    static void loop();
    static bool getConnected();
};

#endif