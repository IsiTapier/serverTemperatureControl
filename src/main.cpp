#include "Arduino.h"
#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include "cEthernet.h"
#include "EmailClient.h"
#include "DataCollection.h"
#include "Display.h"



void setup() {
  Serial.begin(115200);
  Display::setup();
  DataCollection::setup();
  cEthernet::setup();
  EmailClient::setup();
  Serial.println("Setup finished");
}

void loop() {
  DataCollection::loop();
  // cEthernet::loop();
  EmailClient::loop();
  Display::loop();
  delay(2000);
}

