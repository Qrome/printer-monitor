#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include "Configuration.h"
#include "ClockDataStruct.h"
#include "SystemDataStruct.h"
#include "WeatherDataStruct.h"
#include "../Network/TimeClient.h"
#include "../Network/OpenWeatherMapClient.h"
#include "../Display/BaseDisplayClient.h"
#include "../Clients/BasePrinterClient.h"
#include "DebugController.h"
#include "../../include/MemoryHelper.h"
#include "EspController.h"

static const char ERROR_MESSAGES_ERR1[] PROGMEM = "[ERR1] Printer fo update not found!";

/**
 * @brief Handles all needed data for all instances
 */
class GlobalDataController {
private:
    /**
     * Internal
     */
    String lastReportStatus = "";
    TimeClient *timeClient;
    OpenWeatherMapClient *weatherClient; 
    DebugController *debugController;
    BaseDisplayClient *baseDisplayClient;
    BasePrinterClient **basePrinterClients;
    int basePrinterCount = 0;

    /**
     * Configuration variables
     */
    PrinterDataStruct *printers;
    int printersCnt = 0;
    SystemDataStruct systemData;
    ClockDataStruct clockData;
    WeatherDataStruct weatherData;

public:
    GlobalDataController(TimeClient *timeClient, OpenWeatherMapClient *weatherClient, DebugController *debugController);
    void setup();
    void listSettingFiles();
    void readSettings();
    void writeSettings();
    SystemDataStruct *getSystemSettings();
    ClockDataStruct *getClockSettings();
    WeatherDataStruct *getWeatherSettings();  
    TimeClient *getTimeClient();
    OpenWeatherMapClient *getWeatherClient();
    BaseDisplayClient *getDisplayClient();
    void setDisplayClient(BaseDisplayClient *baseDisplayClient);
    void ledOnOff(boolean value);
    void flashLED(int number, int delayTime);
    bool resetConfig();
    
    void registerPrinterClient(int id, BasePrinterClient *basePrinterClient);
    PrinterDataStruct *getPrinterSettings();
    PrinterDataStruct *addPrinterSetting();
    int getNumPrinters();
    String getPrinterStateAsText(PrinterDataStruct *printerHandle);
    String getPrinterClientType(PrinterDataStruct *printerHandle);
    void syncPrinter(PrinterDataStruct *printerHandle);

private:
    void initDefaultConfig();
    bool readSettingsForChar(String line, String expSearch, char *targetChar, size_t maxLen);
    bool readSettingsForBool(String line, String expSearch, bool *targetBool);
    bool readSettingsForInt(String line, String expSearch, int *targetInt);
    bool readSettingsForString(String line, String expSearch, String *targetString);
    






public:

    int numberOfSeconds(int time) { return time % 60UL; }
    int numberOfMinutes(int time) { return (time / 60UL) % 60UL; }
    int numberOfHours(int time) { return time / 3600UL; }
    String zeroPad(int value) { String rtnValue = String(value); if (value < 10) { rtnValue = "0" + rtnValue; } return rtnValue; }



};
