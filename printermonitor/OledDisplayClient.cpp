#include "OledDisplayClient.h"


OledDisplayClient::OledDisplayClient(byte sdaPin, byte sclPin, byte i2cAddress, bool isSh1106, bool invertDisplay, PrinterClientInterface *printerClient) {
  this->printerClient = printerClient;
  if(isSh1106) {
    this->display = new SH1106Wire(i2cAddress, sdaPin, sclPin);
  } else {
    this->display = new SSD1306Wire(i2cAddress, sdaPin, sclPin);
  }
  this->ui = new OLEDDisplayUi(this->display);
  this->invertDisplay = invertDisplay;

  this->display->init();
  if (invertDisplay) {
    this->display->flipScreenVertically(); // connections at top of OLED display
  }
  this->display->clear();
  this->display->display();

  //this->overlays[0] = drawHeaderOverlay;
  //this->clockOverlay[0] = drawClockHeaderOverlay;
}

void OledDisplayClient::postSetup() {
  static OledDisplayClient* obj = this;
  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_TOP, SLIDE_DOWN
  this->ui->setFrameAnimation(SLIDE_LEFT);
  this->ui->setTargetFPS(30);
  this->ui->disableAllIndicators();
  this->ui->setFrames(this->frames, (3));
  this->frames[0] = [](OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { obj->drawScreen1(display, state, x, y); };
  this->frames[1] = [](OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { obj->drawScreen2(display, state, x, y); };
  this->frames[2] = [](OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { obj->drawScreen3(display, state, x, y); };
  this->clockFrame[0] = [](OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { obj->drawClock(display, state, x, y); };
  this->clockFrame[1] = [](OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { obj->drawWeather(display, state, x, y); };
  this->ui->setOverlays(this->overlays, 1);

  // Inital UI takes care of initalising the display too.
  this->ui->init();
  if (this->invertDisplay) {
    this->display->flipScreenVertically();  //connections at top of OLED display
  }
}

void OledDisplayClient::showPageInit(String printerTypeName, String softwareVersion) {
  this->display->setTextAlignment(TEXT_ALIGN_CENTER);
  this->display->setContrast(255); // default is 255
  this->display->setFont(ArialMT_Plain_16);
  this->display->drawString(64, 1, "Printer Monitor");
  this->display->setFont(ArialMT_Plain_10);
  this->display->drawString(64, 18, "for " + printerTypeName);
  this->display->setFont(ArialMT_Plain_16);
  this->display->drawString(64, 30, "By Qrome");
  this->display->drawString(64, 46, "V" + softwareVersion);
  this->display->display();
}

void OledDisplayClient::showPageApMode(String protalSsid) {
  this->display->clear();
  this->display->setTextAlignment(TEXT_ALIGN_CENTER);
  this->display->setFont(ArialMT_Plain_10);
  this->display->drawString(64, 0, "Wifi Manager");
  this->display->drawString(64, 10, "Please connect to AP");
  this->display->setFont(ArialMT_Plain_16);
  this->display->drawString(64, 26, protalSsid);
  this->display->setFont(ArialMT_Plain_10);
  this->display->drawString(64, 46, "To setup Wifi connection");
  this->display->display();
}

void OledDisplayClient::showPageWeather() {
}

void OledDisplayClient::showPagePrinter() {
}

void OledDisplayClient::drawScreen1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  String bed = this->printerClient->getValueRounded(this->printerClient->getTempBedActual());
  String tool = this->printerClient->getValueRounded(printerClient->getTempToolActual());
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

void OledDisplayClient::drawScreen2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);

  display->drawString(64 + x, 0 + y, "Time Remaining");
  //display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_24);
  int val = printerClient->getProgressPrintTimeLeft().toInt();
  int hours = numberOfHours(val);
  int minutes = numberOfMinutes(val);
  int seconds = numberOfSeconds(val);

  String time = zeroPad(hours) + ":" + zeroPad(minutes) + ":" + zeroPad(seconds);
  display->drawString(64 + x, 14 + y, time);
}

void OledDisplayClient::drawScreen3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);

  display->drawString(64 + x, 0 + y, "Printing Time");
  //display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_24);
  int val = printerClient->getProgressPrintTime().toInt();
  int hours = numberOfHours(val);
  int minutes = numberOfMinutes(val);
  int seconds = numberOfSeconds(val);

  String time = zeroPad(hours) + ":" + zeroPad(minutes) + ":" + zeroPad(seconds);
  display->drawString(64 + x, 14 + y, time);
}

void OledDisplayClient::drawClock(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  /*display->setTextAlignment(TEXT_ALIGN_CENTER);
  
  String displayTime = timeClient.getAmPmHours() + ":" + timeClient.getMinutes() + ":" + timeClient.getSeconds();
  if (IS_24HOUR) {
    displayTime = timeClient.getHours() + ":" + timeClient.getMinutes() + ":" + timeClient.getSeconds(); 
  }
  String displayName = PrinterHostName;
  if (printerClient.getPrinterType() == "Repetier") {
    displayName = printerClient.getPrinterName();
  }
  display->setFont(ArialMT_Plain_16);
  display->drawString(64 + x, 0 + y, displayName);
  display->setFont(ArialMT_Plain_24);
  display->drawString(64 + x, 17 + y, displayTime); */
}

void OledDisplayClient::drawWeather(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  /*display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_24);
  display->drawString(0 + x, 0 + y, weatherClient.getTempRounded(0) + getTempSymbol());
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_24);

  display->setFont(ArialMT_Plain_16);
  display->drawString(0 + x, 24 + y, weatherClient.getCondition(0));
  display->setFont((const uint8_t*)Meteocons_Plain_42);
  display->drawString(86 + x, 0 + y, weatherClient.getWeatherIcon(0));*/
}

String OledDisplayClient::zeroPad(int value) {
  String rtnValue = String(value);
  if (value < 10) {
    rtnValue = "0" + rtnValue;
  }
  return rtnValue;
}
