#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <base64.h>

#define SENSOR_CLIENT_BME280       (int)0
#define SENSOR_CLIENT_BME680       (int)1
#define SENSOR_CLIENT_HTU21D       (int)2

typedef struct {
    int     sensType;
    char    error[120];    
} SensorDataStruct;
