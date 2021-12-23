#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "../../common/common_types.hpp"

Adafruit_SSD1306 oled(128, 64, &Wire);

typedef uint8_t mac_t[6];

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
  // initialize the display
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();
  oled.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  oled.setRotation(2);
  oled.setTextWrap(false);
  oled.setCursor(0, 0);
  oled.println(WiFi.macAddress());
  oled.display();

  // initialize ESP NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() == 0)
  {
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_recv_cb(&rxCallback);
    oled.println("Init ok");
  }
  else
  {
    oled.println("Init failed!");
  }

  float random = -12.45;
  fixed16_t rand16 = toFixed16(random);
  float rand32 = fromFixed16(rand16);
  oled.setCursor(0, 6 * 8);

  oled.println(random);
  oled.println(rand32);

  oled.display();
  delay(1000);
}

void loop() {
  static uint32_t rxCount = 0;

  if (rxInfo.newData)
  {
    rxInfo.newData = false;
    rxCount++;

    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("This device:");
    oled.println(WiFi.macAddress());
    oled.println("Sender:");
    char strBuf[100];
    const uint8_t * const senderMacBytes = (const uint8_t * const) rxInfo.sender;
    snprintf_P(strBuf, sizeof(strBuf), PSTR("%02X:%02X:%02X:%02X:%02X:%02X"), senderMacBytes[0], senderMacBytes[1], senderMacBytes[2], senderMacBytes[3], senderMacBytes[4], senderMacBytes[5]);
    oled.println(strBuf);

    switch (rxInfo.msg.type)
    {
      case e_msgType::temperatureHumidity:
        snprintf_P(strBuf, sizeof(strBuf), PSTR("Temp: %.01f C"), fromFixed16(rxInfo.msg.data.tempHumid.temperature));
        oled.println(strBuf);
        snprintf_P(strBuf, sizeof(strBuf), PSTR("Humid: %.1f %%"), fromFixed16(rxInfo.msg.data.tempHumid.humidity));
        oled.println(strBuf);
        break;

      default:
        oled.println("<unknown type>");
        break;
    }

    oled.setCursor(0, 6 * 8);
    oled.print("RX Length: ");
    oled.println(rxInfo.len);
    oled.print("RX Count: ");
    oled.println(rxCount);

    oled.display();
  }

  yield();
}