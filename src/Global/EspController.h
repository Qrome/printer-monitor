#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Configuration.h"

/**
 * @brief Static only functions for control the ESP
 */
class EspController {
public:
    static int8_t getWifiQuality();
    static void getHeap(uint32_t* hfree, uint16_t* hmax, uint8_t* hfrag);
};
