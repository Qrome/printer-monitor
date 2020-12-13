#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "../Global/GlobalDataController.h"
#include <OLEDDisplayUi.h>
#include <OLEDDisplay.h>

class OledDisplay {
private:
    GlobalDataController *globalDataController;
    OLEDDisplay *oledDisplay;
    OLEDDisplayUi *ui;

    FrameCallback frames[3];
    FrameCallback clockFrame[2];
    boolean isClockOn = false;
    OverlayCallback overlays[1];
    OverlayCallback clockOverlay[1];

public:
    OledDisplay(OLEDDisplay *oledDisplay, GlobalDataController *globalDataController);
    void preSetup();
    void postSetup();
    void showBootScreen();
    void showApAccessScreen(String apSsid, String apIp);
    void showWebserverSplashScreen(bool isEnabled);




    void drawScreen1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    void drawScreen2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    void drawScreen3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    void drawClock(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    void drawWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
    void drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
    void drawClockHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
};