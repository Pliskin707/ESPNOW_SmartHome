#ifndef __COMMON_DISPLAY_HPP__
#define __COMMON_DISPLAY_HPP__

#include <time.h>
#include <Arduino.h>

class common_display
{
    public:
        static int getTimeStr (char * buf, const size_t len, const struct tm * const timestamp, const bool includeSeconds = true);
        static int getDateStr (char * buf, const size_t len, const struct tm * const timestamp, const bool includeCentury = true);
        static int getDateTimeStr (char * buf, const size_t len, const struct tm * const timestamp, const bool longVersion = true);
        static String getDateTimeStr (const struct tm * const timestamp, const bool longVersion = true);
};

#endif