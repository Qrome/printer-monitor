#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "../Global/GlobalDataController.h"

class WebServer {
private:
    GlobalDataController *globalDataController;
    ESP8266WebServer *server;
    ESP8266HTTPUpdateServer *serverUpdater;

public:
    WebServer(GlobalDataController *globalDataController);
    void setup();

    
    boolean authentication();
    void redirectHome();
    void displayPrinterStatus();
    void handleSystemReset();
    void handleWifiReset();
    void handleUpdateConfig();
    void handleUpdateWeather();
    void handleConfigure();
    void handleWeatherConfigure();
};
