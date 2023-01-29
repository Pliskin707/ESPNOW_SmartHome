#include "max6675.hpp"

void Thermocouple::begin (void) const
{
    pinMode(this->_cs, OUTPUT);
    SPI.begin();
}

float Thermocouple::getTemp (void) const
{
    uint16_t rawValue = 0;

    digitalWrite(this->_cs, LOW);
    delayMicroseconds(10);

    // TOOD...
}