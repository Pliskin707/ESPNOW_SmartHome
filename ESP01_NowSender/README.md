# ESP-01 (512K) Sender #

This is a low cost sender device with very limited IO which connects to one or more sensors using (preferably) any serial communication protocol. There are no ADC available to use.

## Build Targets ##

There are two build targets:

### 1. DeepSleep ###

This build requires the board manipulation described [here](https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/). It is *recommended* to also remove the power LED to further optimize power consumption.

**Note**

Since the `deepSleep()` mechanism resets the µC every time the given duration elapsed, all variables (including `static` ones) also get reset. The `loop()` part of the [`main.cpp`](/main/ESP01_NowSender/src/main.cpp) does not get called. Everything must be executed within the `setup()` function.

### 2. DefaultESP01 ###

This build requires no further modification of the ESP-01 board and simply puts the WiFi module to sleep during the idle time. This means all variables work as expected and the `loop()` gets called.
