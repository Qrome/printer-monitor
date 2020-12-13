#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include "Configuration.h"

class GlobalDataController {
private:
    /**
     * Internal
     */
    String printerClientTypeName = "";

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

#ifndef USE_NEXTION_DISPLAY
    bool DisplayInvertDisplay = DISPLAY_INVERT_DISPLAY;
#endif

public:
    GlobalDataController();
    void setup();
    void listSettingFiles();
    void readSettings();
    void writeSettings();


    String getVersion();
    String getPrinterClientType();
    void setPrinterClientType(String clientTypeName);

    String getPrinterApiKey();
    String getPrinterHostName();
    String getPrinterServer();
    int getPrinterPort();
    String getPrinterAuthUser();
    String getPrinterAuthPass();
    bool hasPrinterPsu();

    int getWebserverPort();
    bool getWebserverIsBasicAuth();
    String getWebserverUsername();
    String getWebserverPassword();
    String getWebserverTheme();

    bool isDisplayInverted();

    void debugPrint(const char *data);
    void debugPrint(String data);
    void debugPrint(int8_t data);
    void debugPrintF(const char *data, unsigned int uInt);
    void debugPrintLn(const char *data);
    void debugPrintLn(String data);
    void debugPrintLn(long int data);
    void debugPrintLn(int8_t data);

    void ledOnOff(boolean value);
    void flashLED(int number, int delayTime);

    int8_t getWifiQuality();
};
