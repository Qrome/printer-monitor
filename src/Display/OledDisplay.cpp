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
    this->checkDisplay();
    this->ui->update();
}

void OledDisplay::flipDisplayUpdate() {
    this->ui->init();
    if (this->globalDataController->isDisplayInverted()) {
        this->oledDisplay->flipScreenVertically(); // connections at top of OLED display
    }
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








void OledDisplay::checkDisplay() {
    BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();

    if (!this->displayOn && this->globalDataController->getDisplayClock()) {
        this->enableDisplay(true);
    }
    if (this->displayOn && !printerClient->isPrinting() && !this->globalDataController->getDisplayClock()) {
        // Put Display to sleep
        this->oledDisplay->clear();
        this->oledDisplay->display();
        this->oledDisplay->setFont(ArialMT_Plain_16);
        this->oledDisplay->setTextAlignment(TEXT_ALIGN_CENTER);
        this->oledDisplay->setContrast(255); // default is 255
        this->oledDisplay->drawString(64, 5, "Printer Offline\nSleep Mode...");
        this->oledDisplay->display();
        delay(5000);
        this->enableDisplay(false);
        this->debugController->printLn("Printer is offline going down to sleep...");
        return;
    } else if (!this->displayOn && !this->globalDataController->getDisplayClock()) {
        if (printerClient->isOperational()) {
            // Wake the Screen up
            this->enableDisplay(true);
            this->oledDisplay->clear();
            this->oledDisplay->display();
            this->oledDisplay->setFont(ArialMT_Plain_16);
            this->oledDisplay->setTextAlignment(TEXT_ALIGN_CENTER);
            this->oledDisplay->setContrast(255); // default is 255
            this->oledDisplay->drawString(64, 5, "Printer Online\nWake up...");
            this->oledDisplay->display();
            this->debugController->printLn("Printer is online waking up...");
            delay(5000);
            return;
        }
    } else if (this->globalDataController->getDisplayClock()) {
        if ((!printerClient->isPrinting() || printerClient->isPSUoff()) && !this->isClockOn) {
            this->debugController->printLn("Clock Mode is turned on.");
            if (!DISPLAYWEATHER) {
                this->ui->disableAutoTransition();
                this->ui->setFrames(clockFrame, 1);
            } else {
                this->ui->enableAutoTransition();
                this->ui->setFrames(clockFrame, 2);
            }
            this->ui->setOverlays(clockOverlay, 1);
            this->isClockOn = true;
        } else if (printerClient->isPrinting() && !printerClient->isPSUoff() && this->isClockOn) {
            this->debugController->printLn("Printer Monitor is active.");
            this->ui->setFrames(frames, 3);
            this->ui->setOverlays(overlays, 1);
            this->ui->enableAutoTransition();
            isClockOn = false;
        }
    }
}

void OledDisplay::enableDisplay(boolean enable) {
  this->displayOn = enable;
  TimeClient * timeClient = this->globalDataController->getTimeClient();
  if (enable) {
    if (timeClient->getMinutesFromLast(this->displayOffEpoch) >= this->globalDataController->getClockResyncMinutes()) {
      // The display has been off longer than the minutes between refresh -- need to get fresh data
      timeClient->resetLastEpoch();
      this->displayOffEpoch = 0;  // reset
    }
    this->oledDisplay->displayOn();
    this->debugController->printLn(
        "Display was turned ON: " + timeClient->getFormattedTime()
    );
  } else {
    this->oledDisplay->displayOff();
    this->debugController->printLn(
        "Display was turned OFF: " + timeClient->getFormattedTime()
    );
    this->displayOffEpoch = timeClient->getLastEpoch();
  }
}

void OledDisplay::drawScreen1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();

    String bed = printerClient->getValueRounded(printerClient->getTempBedActual());
    String tool = printerClient->getValueRounded(printerClient->getTempToolActual());
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_16);
    if (bed != "0") {
        display->drawString(29 + x, 0 + y, "Tool");
        display->drawString(89 + x, 0 + y, "Bed");
    } else {
        display->drawString(64 + x, 0 + y, "Tool Temp");
    }
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_24);
    if (bed != "0") {
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawString(12 + x, 14 + y, tool + "°");
        display->drawString(74 + x, 14 + y, bed + "°");
    } else {
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->drawString(64 + x, 14 + y, tool + "°");
    }
}

void OledDisplay::drawScreen2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();

    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_16);

    display->drawString(64 + x, 0 + y, "Time Remaining");
    //display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_24);
    int val = printerClient->getProgressPrintTimeLeft().toInt();
    int hours = this->globalDataController->numberOfHours(val);
    int minutes = this->globalDataController->numberOfMinutes(val);
    int seconds = this->globalDataController->numberOfSeconds(val);

    String time = this->globalDataController->zeroPad(hours) + ":" + 
        this->globalDataController->zeroPad(minutes) + ":" + 
        this->globalDataController->zeroPad(seconds);
    display->drawString(64 + x, 14 + y, time);
}

void OledDisplay::drawScreen3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();

    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->setFont(ArialMT_Plain_16);

    display->drawString(64 + x, 0 + y, "Printing Time");
    //display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_24);
    int val = printerClient->getProgressPrintTime().toInt();
    int hours = this->globalDataController->numberOfHours(val);
    int minutes = this->globalDataController->numberOfMinutes(val);
    int seconds = this->globalDataController->numberOfSeconds(val);

    String time = this->globalDataController->zeroPad(hours) + ":" + 
        this->globalDataController->zeroPad(minutes) + ":" + 
        this->globalDataController->zeroPad(seconds);
    display->drawString(64 + x, 14 + y, time);
}

void OledDisplay::drawClock(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    TimeClient * timeClient = this->globalDataController->getTimeClient();
    BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();

    display->setTextAlignment(TEXT_ALIGN_CENTER);
  
    String displayTime = timeClient->getAmPmHours() + ":" + timeClient->getMinutes() + ":" + timeClient->getSeconds();
    if (this->globalDataController->getClockIs24h()) {
        displayTime = timeClient->getHours() + ":" + timeClient->getMinutes() + ":" + timeClient->getSeconds(); 
    }
    String displayName = this->globalDataController->getPrinterHostName();
    if (printerClient->getPrinterType() == "Repetier") {
        displayName = printerClient->getPrinterName();
    }
    display->setFont(ArialMT_Plain_16);
    display->drawString(64 + x, 0 + y, displayName);
    display->setFont(ArialMT_Plain_24);
    display->drawString(64 + x, 17 + y, displayTime);
}

void OledDisplay::drawWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
    OpenWeatherMapClient *weatherClient = this->globalDataController->getWeatherClient();

    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_24);
    display->drawString(0 + x, 0 + y, weatherClient->getTempRounded(0) + weatherClient->getTempSymbol());
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->setFont(ArialMT_Plain_24);

    display->setFont(ArialMT_Plain_16);
    display->drawString(0 + x, 24 + y, weatherClient->getCondition(0));
    display->setFont((const uint8_t*)Meteocons_Plain_42);
    display->drawString(86 + x, 0 + y, weatherClient->getWeatherIcon(0));
}

void OledDisplay::drawHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
    TimeClient * timeClient = this->globalDataController->getTimeClient();
    BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();

    display->setColor(WHITE);
    display->setFont(ArialMT_Plain_16);
    String displayTime = timeClient->getAmPmHours() + ":" + timeClient->getMinutes();
    if (this->globalDataController->getClockIs24h()) {
        displayTime = timeClient->getHours() + ":" + timeClient->getMinutes();
    }
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(0, 48, displayTime);
  
    if (!this->globalDataController->getClockIs24h()) {
        String ampm = timeClient->getAmPm();
        display->setFont(ArialMT_Plain_10);
        display->drawString(39, 54, ampm);
    }

    display->setFont(ArialMT_Plain_16);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    String percent = String(printerClient->getProgressCompletion()) + "%";
    display->drawString(64, 48, percent);
    
    // Draw indicator to show next update
    int updatePos = (printerClient->getProgressCompletion().toFloat() / float(100)) * 128;
    display->drawRect(0, 41, 128, 6);
    display->drawHorizontalLine(0, 42, updatePos);
    display->drawHorizontalLine(0, 43, updatePos);
    display->drawHorizontalLine(0, 44, updatePos);
    display->drawHorizontalLine(0, 45, updatePos);
    
    this->drawRssi(display);
}

void OledDisplay::drawClockHeaderOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
    TimeClient * timeClient = this->globalDataController->getTimeClient();
    BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();

    display->setColor(WHITE);
    display->setFont(ArialMT_Plain_16);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    int printerStateDrawXPos = 0;
    if (!this->globalDataController->getClockIs24h()) {
        display->drawString(0, 48, timeClient->getAmPm());
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        printerStateDrawXPos = 64;
    }
    if (printerClient->isPSUoff()) {
        display->drawString(printerStateDrawXPos, 47, "psu off");
    } else if (printerClient->getState() != PRINTER_STATE_OFFLINE) {
        display->drawString(printerStateDrawXPos, 47, "online");
    } else {
        display->drawString(printerStateDrawXPos, 47, "offline");
    }
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawRect(0, 43, 128, 2);
    
    this->drawRssi(display);
}

void OledDisplay::drawRssi(OLEDDisplay *display) { 
    int8_t quality = this->globalDataController->getWifiQuality();
    for (int8_t i = 0; i < 4; i++) {
        for (int8_t j = 0; j < 3 * (i + 2); j++) {
            if (quality > i * 25 || j == 0) {
                display->setPixel(114 + 4 * i, 63 - j);
            }
        }
    }
}