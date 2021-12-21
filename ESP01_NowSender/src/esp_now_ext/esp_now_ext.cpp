#include "esp_now_ext.hpp"

EspNowExtClass EspNowExt;
static uint8_t _txRetries = 0;
static struct
{
    uint8_t txRetriesRemaining;
    uint8_t len;
    mac dest;
    const uint8_t * data;
} _txPackage = {0, 0, {0}, nullptr};

bool EspNowExtClass::begin (const uint8_t txRetries)
{
    if (esp_now_init() == 0)
    {
        _txRetries = txRetries;
        esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);  // priority is given to the station interface (not the SoftAP)
        esp_now_register_send_cb(&this->_txCallback);
        return true;
    }

    return false;
}

int EspNowExtClass::addPeer (const uint8_t mac[6])
{
    return esp_now_add_peer((uint8_t *) mac, ESP_NOW_ROLE_IDLE, 11, nullptr, 0);   // from the documentation: "The peer's Role does not affect any function, but only stores the Role information for the application layer."
}

int EspNowExtClass::send (const mac destination, const void * data, const uint8_t len)
{
    _txPackage.txRetriesRemaining = _txRetries;
    _txPackage.len = len;
    memcpy(_txPackage.dest, destination, sizeof(mac));
    _txPackage.data = (const uint8_t *) data;

    return esp_now_send((uint8_t *) _txPackage.dest, (uint8_t *) _txPackage.data, _txPackage.len);
}

int EspNowExtClass::send (const void * data, const uint8_t len)
{
    return this->send(nullptr, data, len);
}

void EspNowExtClass::_txCallback (uint8_t * mac_addr, uint8_t status)
{
    if ((status != 0) && _txPackage.txRetriesRemaining)
        EspNowExt._retrySend();
}

void EspNowExtClass::_retrySend (void)
{
    if (_txPackage.txRetriesRemaining)
        _txPackage.txRetriesRemaining--;

    esp_now_send((uint8_t *) _txPackage.dest, (uint8_t *) _txPackage.data, _txPackage.len);
}