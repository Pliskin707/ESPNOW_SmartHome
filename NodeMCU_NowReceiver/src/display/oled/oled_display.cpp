#include "oled_display.hpp"

void oled_display::begin (void)
{
    this->Adafruit_SSD1306::begin(SSD1306_SWITCHCAPVCC, 0x3C);
    this->clearDisplay();
    this->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    this->setRotation(2);
    this->setTextWrap(false);
    this->setCursor(0, 0);
}