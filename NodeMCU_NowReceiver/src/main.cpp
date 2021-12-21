#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

Adafruit_SSD1306 oled(128, 64, &Wire);

typedef uint8_t mac_t[6];

volatile static struct
{
  bool newData;
  mac_t sender;
  uint32_t loopCounter;
  uint8_t len;
} rxInfo = {false};

void rxCallback (uint8_t * mac, uint8_t * data, uint8_t len)
{
  memcpy((void *) rxInfo.sender, mac, sizeof(mac_t));
  const int limitedLen = (len > 4) ? 4 : len;
  rxInfo.loopCounter = 0;
  memcpy((void *) &rxInfo.loopCounter, data, limitedLen);
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
    oled.println(WiFi.macAddress());
    oled.println();
    char strMac[30];
    const uint8_t * const senderMacBytes = (const uint8_t * const) rxInfo.sender;
    snprintf_P(strMac, sizeof(strMac), PSTR("%02X:%02X:%02X:%02X:%02X:%02X"), senderMacBytes[0], senderMacBytes[1], senderMacBytes[2], senderMacBytes[3], senderMacBytes[4], senderMacBytes[5]);
    oled.println(strMac);
    oled.println(rxInfo.loopCounter);
    oled.print("RX Length: ");
    oled.println(rxInfo.len);
    oled.print("RX Count: ");
    oled.println(rxCount);

    oled.display();
  }

  yield();
}