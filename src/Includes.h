#pragma once
#include <Arduino.h>
#include <WiFiManager.h>
#include "Global/GlobalDataController.h"
#include "Global/DebugController.h"
#include "Configuration.h"
#if WEBSERVER_ENABLED
    #include "Network/WebServer.h"
#endif
#include "Network/TimeClient.h"
#include "Network/OpenWeatherMapClient.h"
#include "Network/JsonRequestClient.h"
#include "Clients/RepetierClient.h"
#include "Clients/KlipperClient.h"
#include "Clients/DuetClient.h"
#include "Clients/OctoPrintClient.h"
#ifdef USE_NEXTION_DISPLAY
    #include "Display/NextionDisplay.h"
#else
    #include <SSD1306Wire.h>
    #include <SH1106Wire.h>
    #include "Display/OledDisplay.h"
#endif

// Initilize all needed data
DebugController debugController(DEBUG_MODE_ENABLE);
JsonRequestClient jsonRequestClient(&debugController);
TimeClient timeClient(TIME_UTCOFFSET, &debugController);
OpenWeatherMapClient weatherClient(WEATHER_APIKEY, WEATHER_CITYID, 1, WEATHER_METRIC, WEATHER_LANGUAGE, &debugController, &jsonRequestClient);
GlobalDataController globalDataController(&timeClient, &weatherClient, &debugController);
WebServer webServer(&globalDataController, &debugController);

// Register all printer clients
DuetClient printerClient0(&globalDataController, &debugController, &jsonRequestClient);
KlipperClient printerClient1(&globalDataController, &debugController, &jsonRequestClient);
OctoPrintClient printerClient2(&globalDataController, &debugController, &jsonRequestClient);
RepetierClient printerClient3(&globalDataController, &debugController, &jsonRequestClient);

// Construct correct display client
#ifdef USE_NEXTION_DISPLAY
    SoftwareSerial displaySerialPort(DISPLAY_RX_PIN, DISPLAY_TX_PIN);
    NextionDisplay displayClient(&displaySerialPort, &globalDataController, &debugController);
#else
    #if DISPLAY_SH1106
        SH1106Wire  display(DISPLAY_I2C_DISPLAY_ADDRESS, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
    #else
        SSD1306Wire display(DISPLAY_I2C_DISPLAY_ADDRESS, DISPLAY_SDA_PIN, DISPLAY_SCL_PIN);
    #endif
    OledDisplay displayClient(&display, &globalDataController, &debugController);
#endif