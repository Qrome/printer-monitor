#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include "../Global/GlobalDataController.h"
#include "WebserverMemoryVariables.h"
#include "../../include/MemoryHelper.h"

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
    void redirectTarget(String targetUri);
    void displayPrinterStatus();
    void handleSystemReset();
    void handleWifiReset();
    void handleUpdateConfig();
    
    
    void handleConfigurePrinter();
    void handleUpdatePrinter();
    void handleConfigureStation();
    void handleUpdateStation();
    void handleConfigureWeather();
    void handleUpdateWeather();
    void handleConfigureSensor();
    void handleUpdatePage();
};
