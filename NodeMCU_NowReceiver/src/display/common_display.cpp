#include "common_display.hpp"

int common_display::getTimeStr (char * buf, const size_t len, const struct tm * const timestamp, const bool includeSeconds)
{
    if (includeSeconds)
        return snprintf_P(buf, len, PSTR("%02d:%02d:%02d"), timestamp->tm_hour, timestamp->tm_min, timestamp->tm_sec);
    
    return snprintf_P(buf, len, PSTR("%02d:%02d"), timestamp->tm_hour, timestamp->tm_min);
}

int common_display::getDateStr (char * buf, const size_t len, const struct tm * const timestamp, const bool includeCentury)
{
    if (includeCentury)
        return snprintf_P(buf, len, PSTR("%04d-%02d-%02d"), timestamp->tm_year + 1900, timestamp->tm_mon + 1, timestamp->tm_mday);
    
    return snprintf_P(buf, len, PSTR("%02d-%02d-%02d"), timestamp->tm_year % 100, timestamp->tm_mon + 1, timestamp->tm_mday);
}

int common_display::getDateTimeStr (char * buf, const size_t len, const struct tm * const timestamp, const bool longVersion)
{
    int pos = getDateStr(buf, len, timestamp, longVersion);
    if (((size_t) (pos + 1)) < len)
    {
        buf[pos++] = ' ';
        pos += getTimeStr(buf + pos, len - pos, timestamp, longVersion);
    }

    return pos;
}

String common_display::getDateTimeStr (const struct tm * const timestamp, const bool longVersion)
{
    char buf[20];   // "YYYY-mm-dd HH:MM:SS" + EOS = 20
    getDateTimeStr(buf, sizeof(buf), timestamp, longVersion);

    return String(buf);
}