#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include "Configuration.h"
#include "../Network/TimeClient.h"
#include "../Network/OpenWeatherMapClient.h"
#include "../Clients/BasePrinterClient.h"
#include "DebugController.h"

class GlobalDataController {
private:
    /**
     * Internal
     */
    String lastReportStatus = "";
    TimeClient *timeClient;
    BasePrinterClient *basePrinterClient;
    OpenWeatherMapClient *weatherClient; 
    DebugController *debugController;

    /**
     * Configuration variables
     */
    String PrinterApiKey = PRINTERCLIENT_APIKEY;
    String PrinterHostName = PRINTERCLIENT_HOSTNAME;
    String PrinterServer = PRINTERCLIENT_SERVER;
    int PrinterPort = PRINTERCLIENT_PORT;
    String PrinterAuthUser = PRINTERCLIENT_AUTHUSER;
    String PrinterAuthPass = PRINTERCLIENT_AUTHPASS;
    bool PrinterHasPsu = PRINTERCLIENT_HASPSU;

    int WebserverPort = WEBSERVER_PORT;
    bool WebserverIsBasicAuth = WEBSERVER_IS_BASIC_AUTH;
    String WebserverUsername = WEBSERVER_USERNAME;
    String WebserverPassword = WEBSERVER_PASSWORD;
    String WebserverTheme = WEBSERVER_THEMECOLOR;

    int ClockUtcOffset = TIME_UTCOFFSET;
    bool DisplayClock = DISPLAYCLOCK;
    bool ClockIs24h = TIME_IS_24HOUR;
    int ClockResyncMinutes = TIME_RESYNC_MINUTES_DELAY;

    bool useLedFlash = USE_FLASH;

    bool WeatherShow = DISPLAYWEATHER;
    String WeatherApiKey = WEATHER_APIKEY;
    int WeatherCityId = WEATHER_CITYID;
    bool WeatherIsMetric = WEATHER_METRIC;
    String WeatherLang = WEATHER_LANGUAGE;

#ifndef USE_NEXTION_DISPLAY
    bool DisplayInvertDisplay = DISPLAY_INVERT_DISPLAY;
#endif

public:
    GlobalDataController(TimeClient *timeClient, OpenWeatherMapClient *weatherClient, DebugController *debugController);
    void setup();
    void listSettingFiles();
    void readSettings();
    void writeSettings();

    void setPrinterClient(BasePrinterClient *basePrinterClient);
    TimeClient *getTimeClient();
    BasePrinterClient *getPrinterClient();
    String getLastReportStatus();
    String getVersion();
    
    String getPrinterApiKey();
    void setPrinterApiKey(String printerApiKey);
    String getPrinterHostName();
    void setPrinterHostName(String printerHostName);
    String getPrinterServer();
    void setPrinterServer(String printerServer);
    int getPrinterPort();
    void setPrinterPort(int printerPort);
    String getPrinterAuthUser();
    void setPrinterAuthUser(String printerAuthUser);
    String getPrinterAuthPass();
    void setPrinterAuthPass(String printerAuthPass);
    bool hasPrinterPsu();
    void setHasPrinterPsu(bool hasPsu);
    
    int getWebserverPort();
    bool getWebserverIsBasicAuth();
    String getWebserverUsername();
    String getWebserverPassword();
    String getWebserverTheme();

    bool isDisplayInverted();

    int getClockUtcOffset();
    bool getDisplayClock();
    bool getClockIs24h();
    int getClockResyncMinutes();

    void ledOnOff(boolean value);
    void flashLED(int number, int delayTime);
    bool resetConfig();

    int8_t getWifiQuality();
    int numberOfSeconds(int time) { return time % 60UL; }
    int numberOfMinutes(int time) { return (time / 60UL) % 60UL; }
    int numberOfHours(int time) { return time / 3600UL; }
    String zeroPad(int value) { String rtnValue = String(value); if (value < 10) { rtnValue = "0" + rtnValue; } return rtnValue; }
};
