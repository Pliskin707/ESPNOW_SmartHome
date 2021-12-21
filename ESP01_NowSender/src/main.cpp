#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "esp_now_ext/esp_now_ext.hpp"

static const uint8_t receiverMac[6] = {0x50, 0x02, 0x91, 0xFC, 0xCB, 0x1C};    // \brief Receviers MAC address
static uint32_t loopCounter = 0;

void setup() {
  // WiFi
  WiFi.mode(WIFI_STA);
  // a connection to a "real" wifi network is not required

  // ESP-NOW
  EspNowExt.begin();
  EspNowExt.addPeer(receiverMac);
}

void loop() {
  static uint32_t nextTransmit = 0;
  const uint32_t time = millis();

  if (time >= nextTransmit)
  {
    // prepare next transmit time
    if (!nextTransmit)
      nextTransmit = time;

    nextTransmit += 1000;

    // send data
    EspNowExt.send(receiverMac, &loopCounter, sizeof(loopCounter));
    loopCounter++;
  }
  
  yield();
}