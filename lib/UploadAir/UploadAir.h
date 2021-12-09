#ifndef UploadAir_h
#define UploadAir_h

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

class UploadAir
{
private:
    static void setupAsyncElegantOTA();
public:
    static void setup();
    static void loop();
};

#endif