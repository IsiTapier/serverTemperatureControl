#ifndef EMAIL_CLIENT.h
#define EMAIL_CLIENT.h

#include "../Util.h"
#include <ESP_Mail_Client.h>
#include "cEthernet.h"
#include "DataCollection.h"
class EmailClient
{
private:
    static void smtpCallback(SMTP_Status status);
public:
    static void setup();
    static void loop();
    static bool sendMessage(float temperature, float humidity, bool entwarnung = 0, bool type = SEND_HAUSMEISTER);
};

#endif