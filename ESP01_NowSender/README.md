# ESP-01 (512K) Sender #

This is a low cost sender device with very limited IO which connects to one or more sensors using (preferably) any serial communication protocol. There are no ADC available to use.

![ESP-01 Pinout](https://www.mischianti.org/wp-content/uploads/2019/01/F61TRALJQMOC8BD.LARGE1_.jpg)

## Adjusting the receiver address ##

Within the [`main.cpp`](/ESP01_NowSender/src/main.cpp), adjust the 
```cpp
static const uint8_t receiverMac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};    // \brief Receviers MAC address
```
to the one of your [NodeMCU_NowReceiver](/../../tree/main/NodeMCU_NowReceiver)

## Build Targets ##

There are two build targets:

### 1. DeepSleep ###

This build requires the board manipulation described [here](https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/). It is *recommended* to also remove the power LED to further optimize power consumption.

**Note**

Since the `deepSleep()` mechanism resets the µC every time the given duration elapsed, all variables (including `static` ones) also get reset. The `loop()` part of the [`main.cpp`](/ESP01_NowSender/src/main.cpp) does not get called. Everything must be executed within the `setup()` function.

If persistent data is required, the [EEPROM emulation](https://diyprojects.io/esp8266-how-read-write-erase-the-eeprom-calculate-space-needed/) can be used to store (and flush) the data before the `deepSleep()` command is called.

### 2. DefaultESP01 ###

This build requires no further modification of the ESP-01 board and simply puts the WiFi module to sleep during the idle time. This means all variables work as expected and the `loop()` gets called.

# Power consumption #

## ToDo ##
- [ ] measure power consumption for [DeepSleep](/./ESP01_NowSender#1-deepsleep) build configuration
- [ ] measure power consumption for [DefaultESP01](/./ESP01_NowSender#2-defaultesp01) build configuration
