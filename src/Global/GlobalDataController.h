#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include "Configuration.h"
#include "../Network/TimeClient.h"
#include "../Network/OpenWeatherMapClient.h"
#include "../Clients/BasePrinterClient.h"
#include "DebugController.h"
#include "../Display/BaseDisplayClient.h"

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
    BaseDisplayClient *baseDisplayClient;

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
    bool UseLedFlash = USE_FLASH;
    bool WeatherShow = DISPLAYWEATHER;
    String WeatherApiKey = WEATHER_APIKEY;
    int WeatherCityId = WEATHER_CITYID;
    bool WeatherIsMetric = WEATHER_METRIC;
    String WeatherLang = WEATHER_LANGUAGE;
#ifdef DISPLAY_INVERT_DISPLAY
    bool DisplayInvertDisplay = DISPLAY_INVERT_DISPLAY;
#else
    bool DisplayInvertDisplay = false;
#endif

public:
    GlobalDataController(TimeClient *timeClient, OpenWeatherMapClient *weatherClient, DebugController *debugController);
    void setup();
    void listSettingFiles();
    void readSettings();
    void writeSettings();

    void setPrinterClient(BasePrinterClient *basePrinterClient);
    void setDisplayClient(BaseDisplayClient *baseDisplayClient);
    TimeClient *getTimeClient();
    OpenWeatherMapClient *getWeatherClient();
    BasePrinterClient *getPrinterClient();
    BaseDisplayClient *getDisplayClient();
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
    void setWebserverIsBasicAuth(bool webserverIsBasicAuth);
    String getWebserverUsername();
    void setWebserverUsername(String webserverUsername);
    String getWebserverPassword();
    void setWebserverPassword(String webserverPassword);
    String getWebserverTheme();
    void setWebserverTheme(String webserverTheme);

    bool isDisplayInverted();
    void setIsDisplayInverted(bool displayInvertDisplay);

    int getClockUtcOffset();
    void setClockUtcOffset(int clockUtcOffset);
    bool getDisplayClock();
    void setDisplayClock(bool displayClock);
    bool getClockIs24h();
    void setClockIs24h(bool clockIs24h);
    int getClockResyncMinutes();
    void setClockResyncMinutes(int clockResyncMinutes);

    bool useLedFlash();
    void setUseLedFlash(bool useLedFlash);

    bool getWeatherShow();
    void setWeatherShow(bool weatherShow);
    String getWeatherApiKey();
    void setWeatherApiKey(String weatherApiKey);
    int getWeatherCityId();
    void setWeatherCityId(int weatherCityId);
    bool getWeatherIsMetric();
    void setWeatherIsMetric(bool weatherIsMetric);
    String getWeatherLang();
    void setWeatherLang(String weatherLang);

    void ledOnOff(boolean value);
    void flashLED(int number, int delayTime);
    bool resetConfig();

    int8_t getWifiQuality();
    int numberOfSeconds(int time) { return time % 60UL; }
    int numberOfMinutes(int time) { return (time / 60UL) % 60UL; }
    int numberOfHours(int time) { return time / 3600UL; }
    String zeroPad(int value) { String rtnValue = String(value); if (value < 10) { rtnValue = "0" + rtnValue; } return rtnValue; }
};
