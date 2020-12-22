#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <base64.h>
#include "Debug.h"
#include "../Network/JsonRequestClient.h"
#include "SensorDataStruct.h"
#include "../../include/MemoryHelper.h"

/**
 * @brief Basic function definitions for an sensor client like an interface
 */
class BaseSensorClient {
public:
    virtual String getType() = 0;

    /**
     * @brief Reset all dynamic variables for sensor
     * @param sensorData        Handle to sensor struct
     */
    static void resetData(SensorDataStruct *sensorData) {
    }
};