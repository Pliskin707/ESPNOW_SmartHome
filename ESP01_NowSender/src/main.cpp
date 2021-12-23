#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "esp_now_ext/esp_now_ext.hpp"
#include "sensors/dht_temp_humid/dht_temp_humid.hpp"
#include "../../common/common_types.hpp"

#ifndef DEEPSLEEP
#warning Using normal (unmodified) ESP01 configuration: no deep sleep
#define SLEEPDURATION_MINUTES(x)   ((x) * 60ul * 1000ul)
#else
#warning Using deep sleep mode: all variables get reset every time the controller wakes up! (use EEPROM if persistent data is required)
#define SLEEPDURATION_MINUTES(x)   ((x) * 60ul * 1000000ul)
#endif

#define LEDPIN  LED_BUILTIN

static const uint8_t receiverMac[6] = {0x50, 0x02, 0x91, 0xFC, 0xCB, 0x1C};    // \brief Receviers MAC address
static uint32_t loopCounter = 0;

static void transmit (void)
{
  digitalWrite(LEDPIN, false);
  t_msgData msg = {.type = e_msgType::temperatureHumidity};
  t_tempHumidData payload = {0};  // use a temporary struct so unaligned memory is written

  // read values from sensor
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (!isnan(event.temperature))
    payload.temperature = event.temperature;

  dht.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity))
    payload.humidity = event.relative_humidity;

  // copy with memcpy() so memory alignment is not required
  memcpy(&msg.data.tempHumid, &payload, sizeof(payload));

  // send data
  EspNowExt.send(receiverMac, &msg, sizeof(msg));
  loopCounter++;
  digitalWrite(LEDPIN, true);
}

void setup() {
  pinMode(LEDPIN, OUTPUT);

  // WiFi
  WiFi.mode(WIFI_STA);
  // a connection to a "real" wifi network is not required

  // ESP-NOW
  EspNowExt.begin();
  EspNowExt.addPeer(receiverMac);

  // temperature and humidity sensor
  dht.begin();
  
  #ifdef DEEPSLEEP
  transmit();
  yield();
  ESP.deepSleep(SLEEPDURATION_MINUTES(1));
  #else
  WiFi.forceSleepBegin();
  #endif
}

void loop() {
  #ifndef DEEPSLEEP
  static uint32_t nextTransmit = 0;
  const uint32_t time = millis();

  if (time >= nextTransmit)
  {
    // prepare next transmit time
    if (!nextTransmit)
      nextTransmit = time;

    nextTransmit += SLEEPDURATION_MINUTES(1);
    WiFi.forceSleepWake();
    transmit();
    WiFi.forceSleepBegin();
  }
  
  yield();
  #else
  // this will never be called since the deepSleep() function resets the µC
  #endif
}