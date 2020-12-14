#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <base64.h>
#include "Debug.h"
#include "../Global/DebugController.h"

#define PRINTER_REQUEST_GET     0
#define PRINTER_REQUEST_POST    1

class JsonRequestClient {
private:
    DebugController *debugController;
    String lastError = "";

    static DynamicJsonDocument* lastJsonDocument;
public:
    JsonRequestClient(DebugController *debugController);
    
    DynamicJsonDocument *requestJson(int requestType, String server, int port, String encodedAuth, String httpPath, String apiPostBody, size_t bufferSize, bool withResponse);
    String getLastError();
    void resetLastError();

private:
    WiFiClient requestWifiClient(int requestType, String server, int port, String encodedAuth, String httpPath, String apiPostBody);
    void freeLastJsonDocument();
    DynamicJsonDocument *createNewJsonDocument(size_t bufferSize);
};
