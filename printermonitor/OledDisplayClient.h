#pragma once
#include <ESP8266WiFi.h>
#include <base64.h>
#include "Debug.h"
#include "SH1106Wire.h"
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"
#include "PrinterClientInterface.h"
#include "Macros.h"

class OledDisplayClient {
private:
  OLEDDisplay *display;
  OLEDDisplayUi *ui;
  bool invertDisplay = false;

  FrameCallback frames[3];
  FrameCallback clockFrame[2];
  boolean isClockOn = false;

  OverlayCallback overlays[1];
  OverlayCallback clockOverlay[1];

  PrinterClientInterface *printerClient;

public:
  OledDisplayClient(byte sdaPin, byte sclPin, byte i2cAddress, bool isSh1106, bool invertDisplay, PrinterClientInterface *printerClient);
  void postSetup();
  
  void showPageInit(String printerTypeName, String softwareVersion);
  void showPageApMode(String protalSsid);
  void showPageWeather();
  void showPagePrinter();

private:
  void drawScreen1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  void drawScreen2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  void drawScreen3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  void drawClock(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
  void drawWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);

  String zeroPad(int value);
};
