#ifndef __MAX6675_H__
#define __MAX6675_H__

#include <SPI.h>

class Thermocouple
{
    private:
        const uint8_t _cs;

    public:
        Thermocouple (const uint8_t cs) : _cs(cs) {};
        void begin (void) const;
        float getTemp (void) const;
};

#endif