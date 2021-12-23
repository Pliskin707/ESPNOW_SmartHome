#include "common_types.hpp"

float16 toFloat16 (const float value)
{
    float16 retVal = 0;
    int exponent;
    float fractional;

    fractional = frexpf(value, &exponent);
    if (exponent)
    {
        // calculate the biased fractional part
        retVal = (int) (fractional * ((float) 0x03FFu));   // 11 bit precision: MSB is always set, since frexpf() returns a value between [0.5 .. 1.0)

        // clear the hidden 11th bit
        retVal &= 0x02FF;

        // add the exponent

        // limit
        if (exponent < -14) exponent = -14;
        else if (exponent > 15) exponent = 15;

        // exponent bias (offset)
        exponent += 15;
        float16 shiftedExponent = exponent & 0x001F; // limit to 5 bits
        shiftedExponent <<= 10; // located between the fractional part and the sign
        retVal |= shiftedExponent;
    }

    // add the sign
    if (value < 0)  // compare with the original value, since float allows -0.0f values
        retVal |= (1 << 15);

    return retVal;
}

float fromFloat16 (const float16 value)
{
    float retVal = 0;
    int biasedFractional = value & 0x02FF;
    int exponent = (value & 0x7C00) >> 10;

    // restore the hidden 11th bit
    biasedFractional |= (1 << 10);

    // get the real exponent
    exponent -= 15;

    float fractional = ((float) biasedFractional) / ((float) 0x03FFu);
    retVal = (float) (exp2(exponent) * fractional);

    // negative?
    if (value & (1 << 15))
        retVal = -retVal;

    return retVal;
}