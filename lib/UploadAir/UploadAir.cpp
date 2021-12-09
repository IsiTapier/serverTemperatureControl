#include "UploadAir.h"

AsyncWebServer server(80);

void UploadAir::setup() {
    setupAsyncElegantOTA();
}

void UploadAir::loop() {
  if(WiFi.isConnected())
      AsyncElegantOTA.loop();
   
}

void setupAsyncElegantOTA() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Temperature Control Device");
    });

    AsyncElegantOTA.begin(&server, "admin", "60Fdk3F1L4b!z03iLqQbXYa");    // Start ElegantOTA
    server.begin();
    Serial.println("HTTP server started");
    Serial.println(WiFi.localIP());
}

