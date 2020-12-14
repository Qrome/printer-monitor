#include "OledDisplay.h"

OledDisplay::OledDisplay(OLEDDisplay *oledDisplay, GlobalDataController *globalDataController, DebugController *debugController) {
    this->globalDataController = globalDataController;
    this->debugController = debugController;
    this->oledDisplay = oledDisplay;
    this->ui = new OLEDDisplayUi(oledDisplay);
}

void OledDisplay::preSetup() {
    this->oledDisplay->init();
    if (this->globalDataController->isDisplayInverted()) {
        this->oledDisplay->flipScreenVertically(); // connections at top of OLED display
    }
    this->oledDisplay->clear();
    this->oledDisplay->display();
}


void OledDisplay::postSetup() {
    static OledDisplay* obj = this;
    this->frames[0] = [](OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { obj->drawScreen1(display, state, x, y); };
    this->frames[1] = [](OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { obj->drawScreen2(display, state, x, y); };
    this->frames[2] = [](OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { obj->drawScreen3(display, state, x, y); };
    this->clockFrame[0] = [](OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { obj->drawClock(display, state, x, y); };
    this->clockFrame[1] = [](OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { obj->drawWeather(display, state, x, y); };
    this->overlays[0] = [](OLEDDisplay *display, OLEDDisplayUiState* state) { obj->drawHeaderOverlay(display, state); };
    this->clockOverlay[0] = [](OLEDDisplay *display, OLEDDisplayUiState* state) { obj->drawClockHeaderOverlay(display, state); };

    // You can change the transition that is used
    // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_TOP, SLIDE_DOWN
    this->ui->setFrameAnimation(SLIDE_LEFT);
    this->ui->setTargetFPS(30);
    this->ui->disableAllIndicators();
    this->ui->setFrames(frames, (3));
    this->ui->setOverlays(overlays, 1);

    // Inital UI takes care of initalising the display too.
    this->ui->init();
    if (this->globalDataController->isDisplayInverted()) {
        this->oledDisplay->flipScreenVertically(); // connections at top of OLED display
    }
}

void OledDisplay::handleUpdate() {
    this->ui->update();
}

void OledDisplay::showBootScreen() {
    this->oledDisplay->setTextAlignment(TEXT_ALIGN_CENTER);
    this->oledDisplay->setContrast(255); // default is 255
    this->oledDisplay->setFont(ArialMT_Plain_16);
    this->oledDisplay->drawString(64, 1, "Printer Monitor");
    this->oledDisplay->setFont(ArialMT_Plain_10);
    this->oledDisplay->drawString(64, 18, "for " + this->globalDataController->getPrinterClient()->getPrinterType());
    this->oledDisplay->setFont(ArialMT_Plain_16);
    this->oledDisplay->drawString(64, 30, "By Qrome");
    this->oledDisplay->drawString(64, 46, "V" + this->globalDataController->getVersion());
    this->oledDisplay->display();
}

void OledDisplay::showApAccessScreen(String apSsid, String apIp) {
    this->oledDisplay->clear();
    this->oledDisplay->setTextAlignment(TEXT_ALIGN_CENTER);
    this->oledDisplay->setFont(ArialMT_Plain_10);
    this->oledDisplay->drawString(64, 0, "Wifi Manager");
    this->oledDisplay->drawString(64, 10, "Please connect to AP");
    this->oledDisplay->setFont(ArialMT_Plain_16);
    this->oledDisplay->drawString(64, 26, apSsid);
    this->oledDisplay->setFont(ArialMT_Plain_10);
    this->oledDisplay->drawString(64, 46, "To setup Wifi connection");
    this->oledDisplay->display();
}

void OledDisplay::showWebserverSplashScreen(bool isEnabled) {
    if (isEnabled) {
        String webAddress = "http://" + WiFi.localIP().toString() + ":" + String(this->globalDataController->getWebserverPort()) + "/";
        this->debugController->printLn("Use this URL : " + webAddress);
        this->oledDisplay->clear();
        this->oledDisplay->setTextAlignment(TEXT_ALIGN_CENTER);
        this->oledDisplay->setFont(ArialMT_Plain_10);
        this->oledDisplay->drawString(64, 10, "Web Interface On");
        this->oledDisplay->drawString(64, 20, "You May Connect to IP");
        this->oledDisplay->setFont(ArialMT_Plain_16);
        this->oledDisplay->drawString(64, 30, WiFi.localIP().toString());
        this->oledDisplay->drawString(64, 46, "Port: " + String(this->globalDataController->getWebserverPort()));
        this->oledDisplay->display();
    } else {
        this->debugController->printLn("Web Interface is Disabled");
        this->oledDisplay->clear();
        this->oledDisplay->setTextAlignment(TEXT_ALIGN_CENTER);
        this->oledDisplay->setFont(ArialMT_Plain_10);
        this->oledDisplay->drawString(64, 10, "Web Interface is Off");
        this->oledDisplay->drawString(64, 20, "Enable in Settings.h");
        this->oledDisplay->display(); 
    }
}










void OledDisplay::drawScreen1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

}

void OledDisplay::drawScreen2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

}

void OledDisplay::drawScreen3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

}

void OledDisplay::drawClock(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

}

void OledDisplay::drawWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {

}

void OledDisplay::drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {

}

void OledDisplay::drawClockHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {

}