#pragma once
#include <ESP8266WiFi.h>
#include "libs/ArduinoJson/ArduinoJson.h"
#include <base64.h>
#include "Debug.h"
#include <SoftwareSerial.h>
#include "PrinterClientInterface.h"
#include "Macros.h"

class NextionDisplayClient {
private:
  SoftwareSerial *softwareSerialPort;
  PrinterClientInterface *printerClient;
  
public:
  NextionDisplayClient(byte rxPin, byte txPin, PrinterClientInterface *printerClient);
  void postSetup();
  
  void showPageInit(String printerTypeName, String softwareVersion);
  void showPageApMode(String protalSsid);
  void showPageWeather();
  void showPagePrinter();

private:
  void sendCommand(const char* cmd);
};
