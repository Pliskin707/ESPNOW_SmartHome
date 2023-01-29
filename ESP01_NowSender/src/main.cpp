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

static const uint8_t receiverMac[6] = {0xAC, 0x0B, 0xFB, 0xCF, 0x22, 0x89};    // \brief Receviers MAC address
static uint32_t loopCounter = 0;

static bool transmit (void)
{
  // digitalWrite(LEDPIN, false);
  t_msgData msg = {.type = e_msgType::temperatureHumidity};
  t_tempHumidData payload = {0};  // use a temporary struct so unaligned memory is written
  uint16_t readTimeout = 3000;
  bool success = true;

  // read values from sensor
  sensors_event_t event;
  while (readTimeout--)
  {
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
      delay(1);
      yield();
    }
    else
      break;
  }

  if (!isnan(event.temperature))
    payload.temperature = toFixed16(event.temperature);
  else
    success = false;

  dht.humidity().getEvent(&event);
  if (!isnan(event.relative_humidity))
    payload.humidity = toFixed16(event.relative_humidity);
  else
    success = false;

  // copy with memcpy() so memory alignment is not required
  memcpy(&msg.data.tempHumid, &payload, sizeof(payload));

  // send data
  EspNowExt.send(receiverMac, &msg, sizeof(msg));
  loopCounter++;
  // digitalWrite(LEDPIN, true);

  return success;
}

void setup() {
  // pinMode(LEDPIN, OUTPUT);

  // temperature and humidity sensor
  dht.begin();

  // WiFi
  WiFi.mode(WIFI_STA);
  // a connection to a "real" wifi network is not required

  // ESP-NOW
  EspNowExt.begin();
  EspNowExt.addPeer(receiverMac);
  
  #ifdef DEEPSLEEP
  delay(2000);  // maybe the sensor requires some time?
  const bool success = transmit();
  yield();
  ESP.deepSleep(SLEEPDURATION_MINUTES((success ? 30 : 120)));
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