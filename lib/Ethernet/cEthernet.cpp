#define ETH_CLK_MODE ETH_CLOCK_GPIO17_OUT
#define ETH_PHY_POWER 12

#include "cEthernet.h"

static bool eth_connected = false;

//PC ETH MAC: 08:3A:F2:3A:C9:93, IPv4: 10.1.11.40, FULL_DUPLEX, 100Mbps DNS: 10.1.1.1
//UFO ETH MAC: 08:3A:F2:3A:C9:93, IPv4: 10.1.10.164, FULL_DUPLEX, 100Mbps DNS: 10.1.10.26

void cEthernet::WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case SYSTEM_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-server-emperature-control");
      break;
    case SYSTEM_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case SYSTEM_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      Serial.print(", DNS: ");
      Serial.print(ETH.dnsIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case SYSTEM_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void cEthernet::testClient(const char * host, uint16_t port) {
  Serial.print("\nconnecting to ");
  Serial.println(host);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("closing connection\n");
  client.stop();
}

void cEthernet::setup() {
  WiFi.onEvent(WiFiEvent);
  ETH.begin();
  while(!eth_connected&&FORCE_ETHERNET) {
    delay(1000);
  }

  Serial.println();
  // Serial.println("WiFi connected.");
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());
  // Serial.println();
  // if (eth_connected) {
  //   testClient("google.com", 80);
  // }
}


void cEthernet::loop() {
  while(!ETH.begin()) {
    delay(2000);
  }
  if (eth_connected) {
    testClient("google.com", 80);
  }
  delay(10000);
}

bool cEthernet::getConnected() {
  return eth_connected;
}