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
#if (PRINTERCLIENT == REPETIER_CLIENT)
    #include "Clients/RepetierClient.h"
#elif (PRINTERCLIENT == KLIPPER_CLIENT)
    #include "Clients/KlipperClient.h"
#elif (PRINTERCLIENT == DUET_CLIENT)
    #include "Clients/DuetClient.h"
#else
    #include "Clients/OctoPrintClient.h"
#endif
#ifdef USE_NEXTION_DISPLAY
    #include "Display/NextionDisplay.h"
#else
    #include <SSD1306Wire.h>
    #include <SH1106Wire.h>
    #include "Display/OledDisplay.h"
#endif

// Initilize all needed data
DebugController debugController(DEBUG_MODE_ENABLE);
TimeClient timeClient(TIME_UTCOFFSET, &debugController);
OpenWeatherMapClient weatherClient(WEATHER_APIKEY, WEATHER_CITYID, 1, WEATHER_METRIC, WEATHER_LANGUAGE, &debugController);
GlobalDataController globalDataController(&timeClient, &weatherClient, &debugController);
WebServer webServer(&globalDataController, &debugController);

// Construct the correct printer client
#if (PRINTERCLIENT == REPETIER_CLIENT)
    //RepetierClient printerClient(&globalDataController);
#elif (PRINTERCLIENT == KLIPPER_CLIENT)
    KlipperClient printerClient(&globalDataController, &debugController);
#elif (PRINTERCLIENT == DUET_CLIENT)
    //DuetClient printerClient(PrinterApiKey, PrinterServer, PrinterPort, PrinterAuthUser, PrinterAuthPass, HAS_PSU, debugHandle);
#else
    //OctoPrintClient printerClient(PrinterApiKey, PrinterServer, PrinterPort, PrinterAuthUser, PrinterAuthPass, HAS_PSU, debugHandle);
#endif

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