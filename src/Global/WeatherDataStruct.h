#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <base64.h>

typedef struct {
    bool    show;
    String  apiKey;
    int     cityId;
    bool    isMetric;
    String  lang;
} WeatherDataStruct;
