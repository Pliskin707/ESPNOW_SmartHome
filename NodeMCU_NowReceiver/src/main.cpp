#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <MAX6675.h>

#include "../../common/common_types.hpp"

#define TFT_DISPLAY
#define ENABLE_WLAN
#define ENABLE_STORAGE

#ifndef TFT_DISPLAY
#include "display/oled/oled_display.hpp"
oled_display display;
#else
#include "display/tft/tft_display.hpp"
tft_display display;
#endif

#ifdef ENABLE_WLAN
#include <WiFiUdp.h>
#include "../../../../../wifiauth2.h"               // this contains "ssid" and "password" strings
#define TIMEZONESTR   "CET-1CEST,M3.5.0,M10.5.0/3"  // Berlin (see https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv)
#define NTPSERVERSTR  "time.google.com"
#endif

#ifdef ENABLE_STORAGE
#include "storage/sd_storage.hpp"
storage store;
#endif

MAX6675 thermocouple(D4);

/** 
 * Pin assignment:
 *
 * | Node | Display | SD Slot | MAX6675 |
 * | :--: | :-----: | :-----: | :-----: |
 * | D0   | LED     |         |         |
 * | D1   |         | CS      |         |
 * | D2   | AO      |         |         |
 * | D3   | Reset   |         |         |
 * | D4   |         |         | CS      |
 * | D5   | SCK     | SCK     | SCK     |
 * | D6   |         | MISO    | SO      |
 * | D7   | SDA     | MOSI    |         |
 * | D8   | CS      |         |         |
 */

const unsigned int NUMBER_OF_MODES = 1;

__attribute__((section(".noinit"))) static struct
{
  uint32_t notInitialized;
  uint8_t displayBrightness;
} variables;

volatile static struct
{
  bool newData;
  mac_t sender;
  t_msgData msg;
  uint8_t len;
} rxInfo = {false};

void rxCallback (uint8_t * mac, uint8_t * data, uint8_t len)
{
  memset ((void *) &rxInfo, 0, sizeof(rxInfo));
  memcpy((void *) rxInfo.sender, mac, sizeof(mac_t));
  const int limitedLen = (len > sizeof(rxInfo.msg)) ? sizeof(rxInfo.msg) : len;
  memcpy((void *) &rxInfo.msg, data, limitedLen);
  rxInfo.len = len;
  rxInfo.newData = true;
}

void setup() {
  // initialize the variables
  if (variables.notInitialized)
  {
    variables.notInitialized = false;
    variables.displayBrightness = 100;
  }
  else
  {
    switch (variables.displayBrightness)
    {
    case 100: variables.displayBrightness = 1;    break;
    case 1:   variables.displayBrightness = 0;    break;
    default:  variables.displayBrightness = 100;  break;
    }
  }

  // initialize the display
  display.begin();
  display.setTextColor(-1, 0);  // white text, black background (should work for both oled and tft)
  display.println(WiFi.macAddress());
  display.display();

  #ifdef TFT_DISPLAY
  for (uint8_t bri = 0; bri <= variables.displayBrightness; bri++)
  {
    display.setBrightness(bri);
    delay(5);
  }
  #endif

  // initialize ESP NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() == 0)
  {
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_recv_cb(&rxCallback);
    display.println("ESP-NOW init ok");
  }
  else
  {
    display.println("ESP-NOW init failed!");
  }

  #ifdef ENABLE_WLAN
  // configure time
  configTime(TIMEZONESTR, NTPSERVERSTR);

  // connect to the "real" Wifi
  WiFi.begin(ssid, password);
  const uint32_t timeout = millis() + 10000;
  wl_status_t status;
  do
  {
    display.print('.');
    display.display();
    status = WiFi.status();
    if (status == WL_CONNECTED)
      break;

    delay(250);
  } while (millis() < timeout);
  display.printf_P("\nWLAN %s\n", (status == WL_CONNECTED) ? "connected":"failed");
  #endif

  #ifdef ENABLE_STORAGE
  display.printf_P(PSTR("SD card init %s\n"), store.begin() ? "OK":"Failed");
  display.printf_P(PSTR("SD file init %s\n"), store.openOrCreate() ? "OK":"Failed");
  #endif

  display.display();
  delay(1000);
}

void loop() {
  const uint32_t millisCount = millis();
  static uint32_t rxCount = 0;
  static uint32_t nextTimeSync = 0;
  static uint32_t nextThermoRead = 0;
  static struct tm timestamp;
  static bool clearDisplay = true;

  if (rxInfo.newData)
  {
    rxInfo.newData = false;
    rxCount++;

    #ifndef TFT_DISPLAY
    display.clearDisplay();
    #endif

    if (clearDisplay)
    {
      display.clearDisplay();
      clearDisplay = false;
    }

    char strBuf[100];
    char sensorType[50];
    float value1 = NAN, value2 = NAN;
    String macStr = toString((const uint8_t *) rxInfo.sender);
    bool storeValues = false;

    display.setCursor(0, 0);
    display.println("This device:");
    display.println(WiFi.macAddress());
    display.println("Sender:");
    display.println(macStr.c_str());

    switch (rxInfo.msg.type)
    {
      case e_msgType::temperatureHumidity:
        strncpy_P(sensorType, PSTR("TempHumid"), sizeof(sensorType));
        value1 = fromFixed16(rxInfo.msg.data.tempHumid.temperature);
        value2 = fromFixed16(rxInfo.msg.data.tempHumid.humidity);
        storeValues = true;

        snprintf_P(strBuf, sizeof(strBuf), PSTR("Temp: %.01f C"), value1);
        display.println(strBuf);
        snprintf_P(strBuf, sizeof(strBuf), PSTR("Humid: %.1f %%"), value2);
        display.println(strBuf);
        break;

      default:
        display.println("<unknown type>");
        break;
    }

    display.setCursor(0, 6 * 8);
    display.print("RX Length: ");
    display.println(rxInfo.len);
    display.print("RX Count: ");
    display.println(rxCount);

    #ifdef ENABLE_STORAGE
    if ((timestamp.tm_year > 70) && storeValues)
    {
      store.writeSensorData(&timestamp, macStr.c_str(), sensorType, value1, value2);
      store.flush();
    }
    #else
    (void) storeValues; // suppress "unused" warning
    #endif

    // display.display();
  }

  if (millisCount >= nextThermoRead)
  {
    nextThermoRead += 300;

    // it seems the data received is shifted by 1 compared to the data sheet, so correct for it here by doing a "float right shift" by one (= divide by 2)
    const float celcius = thermocouple.readTempC() / 2.0f;  

    display.setCursor(0, 11 * 8);
    if (isnan(celcius))
    {
      display.setTextColor(0xF800, 0);  // red
      display.println(F("Thermocouple error"));
    }
    else if (celcius == MAX6675_THERMOCOUPLE_OPEN)
    {
      display.setTextColor(0xF800, 0);  // red
      display.println(F("No Thermocouple found"));
    }
    else
    {
      char buf[50];
      snprintf_P(buf, sizeof(buf), PSTR("Local Temp: %.2fC"), celcius);
      display.print(buf);
    }

    display.setTextColor(-1, 0);
  }

  if (millisCount >= nextTimeSync)
  {
    nextTimeSync += 1000;

    time_t now;
    char buf[20];
    
    time(&now);
    localtime_r(&now, &timestamp);

    if (timestamp.tm_year > 70)
    {
      display.setCursor(0, 10 * 8);
      display.setTextColor(0x07FF, 0); // cyan
      display.getDateTimeStr(buf, sizeof(buf), &timestamp);
      display.print(buf);
      display.setTextColor(-1, 0);
    }

    display.display();
  }

  yield();
}