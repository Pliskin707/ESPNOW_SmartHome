#include "tft_display.hpp"

void tft_display::begin (const uint8_t brightness)
{
    this->Adafruit_ST7735::initR(INITR_18BLACKTAB); // TODO which option is required?
    this->clearDisplay();

    if (this->_brightnessPin >= 0)
    {
        pinMode(this->_brightnessPin, OUTPUT);
        analogWriteFreq(1000);
        analogWriteRange(100);
        this->setBrightness(brightness);
    }

    this->setSPISpeed(20000000);
}

void tft_display::clearDisplay (void)
{
    this->fillScreen(0);
}

void tft_display::display (void)
{
}

void tft_display::setBrightness (uint8_t brightness)
{
    if (this->_brightnessPin >= 0)
    {
        brightness = ((brightness > 100) ? 100 : brightness);
        analogWrite(this->_brightnessPin, brightness);
    }
}