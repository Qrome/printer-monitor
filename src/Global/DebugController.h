#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Configuration.h"

class DebugController {
private:
    bool enabled;

public:
    DebugController(bool enabled);
    bool isEnabled();
    void setup();

    void print(const char *data);
    void print(String data);
    void print(int8_t data);
    void printF(const char *data, unsigned int uInt);
    void printLn(const char *data);
    void printLn(String data);
    void printLn(long int data);
    void printLn(int8_t data);
};
