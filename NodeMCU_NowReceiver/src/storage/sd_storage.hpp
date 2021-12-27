#ifndef __SD_STORAGE_HPP__
#define __SD_STORAGE_HPP__

#include <SD.h>
#include "display/common_display.hpp"

#define DELIMITER       ';'
#define DECSEPARATOR    ','

class storage
{
    private:
        SDClass _sd;
        fs::File _file;
        bool _sdOk = false;
        bool _fileOpen = false;

    public:
        bool begin (void);
        bool test (void);
        bool openOrCreate (const char * const prefix = nullptr);
        bool writeSensorData (const struct tm * const timestamp, const char * const ID, const char * const type, const float value1, const float value2 = NAN); //! \brief has no overflow protection
        void flush (void);
};

#endif