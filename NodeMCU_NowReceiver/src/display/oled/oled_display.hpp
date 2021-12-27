#ifndef __OLED_DISPLAY_HPP__
#define __OLED_DISPLAY_HPP__

#include <Adafruit_SSD1306.h>
#include "../common_display.hpp"

class oled_display : public Adafruit_SSD1306, public common_display
{
    public:

        oled_display() : Adafruit_SSD1306(128, 64, &Wire) {};
        void begin (void);
};

#endif