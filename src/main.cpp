#include "Arduino.h"
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include "cEthernet.h"
#include "EmailClient.h"
#include "DataCollection.h"
#include "Display.h"



void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  cEthernet::setup();
  EmailClient::setup();
  Display::setup();
}

void loop() {
  delay(2000);
  DataCollection::loop();
  EmailClient::loop();
  Display::loop();
}

