#ifndef EMAIL_CLIENT.h
#define EMAIL_CLIENT.h

#include "../Util.h"
#include <ESP_Mail_Client.h>
#include "DataCollection.h"
class EmailClient
{
private:
    static void smtpCallback(SMTP_Status status);
public:
    static void setup();
    static void loop();
    static bool sendMessage(float temperature, bool type = 0, float otherTemperature = 0);
};

#endif