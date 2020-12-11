#pragma once
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "Debug.h"
#include "PrinterClientInterface.h"
#include "TimeClient.h"
#include "Macros.h"

class WebServer {
private:
  ESP8266WebServer *serverHandle;
  String themeColor;
  Debug *debugHandle;
  String lastReportStatus;
  String swVersion;
  String hostName;
  TimeClient *timeClient;
  PrinterClientInterface *printerClient;
  boolean hasPsu;

  boolean isBasicAuth;
  char *wwwUsername;
  char *wwwPassword;
   
public:
  WebServer(ESP8266WebServer *serverHandle, Debug *debugHandle, PrinterClientInterface *printerClient, TimeClient *timeClient, String themeColor, String swVersion, boolean hasPsu);

  void setHostname(String hostName);

  void redirectHome();
  String getHeader();
  String getHeader(boolean refresh);
  String getFooter();
  
  void displayPrinterStatus();
  void displayMessage(String message);

  void setAuthentication(boolean enableBasicAuth, char *username, char *password);
  boolean authentication();
  void handleSystemReset();
  void handleWifiReset();
  int8_t getWifiQuality();
  String zeroPad(int value);
};
