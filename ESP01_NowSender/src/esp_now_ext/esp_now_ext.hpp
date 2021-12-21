#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <espnow.h>

typedef uint8_t mac[6];

class EspNowExtClass
{
    private:
        static void _txCallback (uint8_t * mac_addr, uint8_t status);
        void _retrySend (void);

    public:
        bool begin (const uint8_t txRetries = 3);
        int addPeer (const mac address);
        int send (const mac destination, const void * data, const uint8_t len);
        int send (const void * data, const uint8_t len);
};

extern EspNowExtClass EspNowExt;    // \brief global instance for extended ESP-NOW functionality