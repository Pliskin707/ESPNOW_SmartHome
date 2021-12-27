#include "sd_storage.hpp"

void add_delimiter (char ** pbuf)
{
    **pbuf = DELIMITER;
    (*pbuf)++;
}

void add_string (char ** pbuf, const char * const str)
{
    add_delimiter(pbuf);
    strcpy(*pbuf, str);
    *pbuf += strlen(str);
}

void add_float (char ** pbuf, const float flt)
{
    div_t qr = div(flt * 1000, 1000);

    add_delimiter(pbuf);
    *pbuf += sprintf_P(*pbuf, PSTR("%d%c%03d"), qr.quot, DECSEPARATOR, qr.rem);
}

bool storage::begin (void)
{
    this->_sdOk = this->_sd.begin(D1);
    return this->_sdOk;
}

bool storage::test (void)
{
    if (this->_sdOk)
    {
        auto testFile = SD.open("test.txt", FILE_WRITE);
        if (testFile)
        {
            testFile.println("Hello World");
            testFile.flush();
            testFile.close();
            SD.end(false);

            return true;
        }
    }

    return false;
}

bool storage::openOrCreate (const char * const prefix)
{
    bool success = false;
    String filePath;

    if (prefix)
    {
        filePath.concat(prefix);
        filePath.concat('_');
    }

    filePath.concat(F("sensors.csv"));

    do
    {
        if (!this->_sdOk)
            break;

        this->_file = this->_sd.open(filePath, FILE_WRITE);
        if (!this->_file)
            break;

        this->_fileOpen = true;

        // create header?
        if (!this->_file.size())
        {
            this->_file.print(F("Timestamp"));
            this->_file.print(DELIMITER);
            this->_file.print(F("ID"));
            this->_file.print(DELIMITER);
            this->_file.print(F("Sensor Type"));
            this->_file.print(DELIMITER);
            this->_file.print(F("Value1"));
            this->_file.print(DELIMITER);
            this->_file.print(F("Value2"));
            this->_file.println();
        }

        success = true;
    } while (false);
    
    return success;
}

bool storage::writeSensorData (const struct tm * const timestamp, const char * const ID, const char * const type, const float value1, const float value2)
{
    bool success = false;

    do
    {
        if (!this->_fileOpen)
            break;

        char line[200];
        char * pos = line;
        pos += common_display::getDateTimeStr(line, sizeof(line), timestamp, true);

        add_string(&pos, ID);
        add_string(&pos, type);
        add_float(&pos, value1);

        if (!isnan(value2))
            add_float(&pos, value2);

        if (this->_file.println(line) > 0)
            success = true;
    } while (false);

    return success;
}

void storage::flush (void)
{
    if (this->_fileOpen)
        this->_file.flush();
}