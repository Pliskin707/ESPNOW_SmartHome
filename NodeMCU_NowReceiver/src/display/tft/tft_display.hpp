#ifndef __TFT_DISPLAY_HPP__
#define __TFT_DISPLAY_HPP__

#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "../common_display.hpp"

class tft_display : public Adafruit_ST7735, public common_display
{
    private:
        const int8_t _brightnessPin = D0;

    public:
        tft_display() : Adafruit_ST7735(D8, D2, D3) {};
        void begin (const uint8_t brightness = 0);
        void display (void);
        void clearDisplay (void);
        void setBrightness (uint8_t brightness);
};

#endif