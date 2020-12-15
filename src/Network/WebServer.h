#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include "../Global/GlobalDataController.h"
#include "WebserverMemoryVariables.h"

class WebServer {
private:
    GlobalDataController *globalDataController;
    ESP8266WebServer *server;
    ESP8266HTTPUpdateServer *serverUpdater;
    DebugController *debugController;

public:
    WebServer(GlobalDataController *globalDataController, DebugController *debugController);
    void setup();
    void findMDNS();

    void handleClient();
    boolean authentication();
    void redirectHome();
    void displayPrinterStatus();
    void handleSystemReset();
    void handleWifiReset();
    void handleUpdateConfig();
    void handleUpdateStation();
    void handleUpdateWeather();
    void handleConfigurePrinter();
    void handleConfigureStation();
    void handleWeatherConfigure();
    void handleUpdatePage();
};
