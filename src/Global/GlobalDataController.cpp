#include "GlobalDataController.h"

GlobalDataController::GlobalDataController(TimeClient *timeClient, OpenWeatherMapClient *weatherClient, DebugController *debugController) {
     this->timeClient = timeClient;
     this->weatherClient = weatherClient;
     this->debugController = debugController;
}

void GlobalDataController::setup() {
    this->listSettingFiles();
    this->readSettings();
}

void GlobalDataController::listSettingFiles() {
    this->debugController->printLn("========= FileSystem Files =================");
    Dir dir = LittleFS.openDir("/");
    while (dir.next())
    {
        this->debugController->printLn(dir.fileName());
    }
}

void GlobalDataController::readSettings() {
    if (LittleFS.exists(CONFIG) == false) {
        this->debugController->printLn("Settings File does not yet exists.");
        writeSettings();
        return;
    }
    File fr = LittleFS.open(CONFIG, "r");
    String line;
    while(fr.available()) {
        line = fr.readStringUntil('\n');

        if (line.indexOf("printerApiKey=") >= 0) {
            this->PrinterApiKey = line.substring(line.lastIndexOf("printerApiKey=") + 14);
            this->PrinterApiKey.trim();
            this->debugController->printLn("PrinterApiKey=" + this->PrinterApiKey);
        }
        if (line.indexOf("printerHostName=") >= 0) {
            this->PrinterHostName = line.substring(line.lastIndexOf("printerHostName=") + 16);
            this->PrinterHostName.trim();
            this->debugController->printLn("PrinterHostName=" + this->PrinterHostName);
        }
        if (line.indexOf("printerServer=") >= 0) {
            this->PrinterServer = line.substring(line.lastIndexOf("printerServer=") + 14);
            this->PrinterServer.trim();
            this->debugController->printLn("PrinterServer=" + this->PrinterServer);
        }
        if (line.indexOf("printerPort=") >= 0) {
            this->PrinterPort = line.substring(line.lastIndexOf("printerPort=") + 12).toInt();
            this->debugController->printLn("PrinterPort=" + String(this->PrinterPort));
        }
        if (line.indexOf("printerName=") >= 0) {
            String printer = line.substring(line.lastIndexOf("printerName=") + 12);
            printer.trim();
            this->getPrinterClient()->setPrinterName(printer);
            this->debugController->printLn("PrinterName=" + this->getPrinterClient()->getPrinterName());
        }
        if (line.indexOf("printerAuthUser=") >= 0) {
            this->PrinterAuthUser = line.substring(line.lastIndexOf("printerAuthUser=") + 16);
            this->PrinterAuthUser.trim();
            this->debugController->printLn("PrinterAuthUser=" + this->PrinterAuthUser);
        }
        if (line.indexOf("printerAuthPass=") >= 0) {
            this->PrinterAuthPass = line.substring(line.lastIndexOf("printerAuthPass=") + 16);
            this->PrinterAuthPass.trim();
            this->debugController->printLn("PrinterAuthPass=" + this->PrinterAuthPass);
        }
        if (line.indexOf("printerHasPsu=") >= 0) {
            this->PrinterHasPsu = line.substring(line.lastIndexOf("printerHasPsu=") + 14).toInt();
            this->debugController->printLn("PrinterHasPsu=" + String(this->PrinterHasPsu));
        }
        if(line.indexOf("displayInvertDisplay=") >= 0) {
            this->DisplayInvertDisplay = line.substring(line.lastIndexOf("displayInvertDisplay=") + 21).toInt();
            this->debugController->printLn("DisplayInvertDisplay=" + String(this->DisplayInvertDisplay));
        }
        if (line.indexOf("webserverTheme=") >= 0) {
            this->WebserverTheme = line.substring(line.lastIndexOf("webserverTheme=") + 15);
            this->WebserverTheme.trim();
            this->debugController->printLn("webserverTheme=" + this->WebserverTheme);
        }
        if (line.indexOf("webserverIsBasicAuth=") >= 0) {
            this->WebserverIsBasicAuth = line.substring(line.lastIndexOf("webserverIsBasicAuth=") + 21).toInt();
            this->debugController->printLn("webserverIsBasicAuth=" + String(this->WebserverIsBasicAuth));
        }
        if (line.indexOf("webserverUsername=") >= 0) {
            this->WebserverUsername = line.substring(line.lastIndexOf("webserverUsername=") + 18);
            this->WebserverUsername.trim();
            this->debugController->printLn("webserverUsername=" + this->WebserverUsername);
        }
        if (line.indexOf("webserverPassword=") >= 0) {
            this->WebserverPassword = line.substring(line.lastIndexOf("webserverPassword=") + 18);
            this->WebserverPassword.trim();
            this->debugController->printLn("webserverPassword=" + this->WebserverPassword);
        }
        if (line.indexOf("clockUtcOffset=") >= 0) {
            this->ClockUtcOffset = line.substring(line.lastIndexOf("clockUtcOffset=") + 15).toFloat();
            this->debugController->printLn("clockUtcOffset=" + String(this->ClockUtcOffset));
        }
        if (line.indexOf("clockResyncMinutes=") >= 0) {
            this->ClockResyncMinutes = line.substring(line.lastIndexOf("clockResyncMinutes=") + 19).toInt();
            this->debugController->printLn("clockResyncMinutes=" + String(this->ClockResyncMinutes));
        }
        if (line.indexOf("displayClock=") >= 0) {
            this->DisplayClock = line.substring(line.lastIndexOf("displayClock=") + 13).toInt();
            this->debugController->printLn("displayClock=" + String(this->DisplayClock));
        }
        if (line.indexOf("clockIs24h=") >= 0) {
            this->ClockIs24h = line.substring(line.lastIndexOf("clockIs24h=") + 11).toInt();
            this->debugController->printLn("clockIs24h=" + String(this->ClockIs24h));
        }
        if (line.indexOf("weatherShow=") >= 0) {
            this->WeatherShow = line.substring(line.lastIndexOf("weatherShow=") + 12).toInt();
            this->debugController->printLn("weatherShow=" + String(this->WeatherShow));
        }
        if (line.indexOf("weatherApiKey=") >= 0) {
            this->WeatherApiKey = line.substring(line.lastIndexOf("weatherApiKey=") + 14);
            this->WeatherApiKey.trim();
            this->debugController->printLn("weatherApiKey=" + this->WeatherApiKey);
        }
        if (line.indexOf("weatherCityId=") >= 0) {
            this->WeatherCityId = line.substring(line.lastIndexOf("weatherCityId=") + 14).toInt();
            this->debugController->printLn("weatherCityId=" + String(this->WeatherCityId));
        }
        if (line.indexOf("weatherIsMetric=") >= 0) {
            this->WeatherIsMetric = line.substring(line.lastIndexOf("weatherIsMetric=") + 16).toInt();
            this->debugController->printLn("weatherIsMetric=" + String(this->WeatherIsMetric));
        }
        if (line.indexOf("weatherLang=") >= 0) {
            this->WeatherLang = line.substring(line.lastIndexOf("weatherLang=") + 12);
            this->WeatherLang.trim();
            this->debugController->printLn("weatherLang=" + this->WeatherLang);
        }
        if(line.indexOf("useLedFlash=") >= 0) {
            this->UseLedFlash = line.substring(line.lastIndexOf("useLedFlash=") + 12).toInt();
            this->debugController->printLn("useLedFlash=" + String(this->UseLedFlash));
        }
    }
    fr.close();
    this->getPrinterClient()->updatePrintClient();
    this->weatherClient->updateWeatherApiKey(this->WeatherApiKey);
    this->weatherClient->updateLanguage(this->WeatherLang);
    this->weatherClient->setMetric(this->WeatherIsMetric);
    this->weatherClient->updateCityId(this->WeatherCityId);
    this->timeClient->setUtcOffset(this->getClockUtcOffset());
}

void GlobalDataController::writeSettings() {
    // Save decoded message to SPIFFS file for playback on power up.
    File f = LittleFS.open(CONFIG, "w");
    if (!f) {
        this->debugController->printLn("File open failed!");
    } else {
        this->debugController->printLn("Saving settings now...");
        f.println("printerApiKey=" + this->PrinterApiKey);
        f.println("printerHostName=" + this->PrinterHostName);
        f.println("printerServer=" + this->PrinterServer);
        f.println("printerPort=" + String(this->PrinterPort));
        f.println("printerName=" + this->getPrinterClient()->getPrinterName());
        f.println("printerAuthUser=" + this->PrinterAuthUser);
        f.println("printerAuthPass=" + this->PrinterAuthPass);
        f.println("printerHasPsu=" + String(this->PrinterHasPsu));
        f.println("displayInvertDisplay=" + String(this->DisplayInvertDisplay));
        f.println("webserverTheme=" + this->WebserverTheme);
        f.println("webserverIsBasicAuth=" + String(this->WebserverIsBasicAuth));
        f.println("webserverUsername=" + String(this->WebserverUsername));
        f.println("webserverPassword=" + String(this->WebserverPassword));
        f.println("clockUtcOffset=" + String(this->ClockUtcOffset));
        f.println("clockResyncMinutes=" + String(this->ClockResyncMinutes));
        f.println("displayClock=" + String(this->DisplayClock));
        f.println("clockIs24h=" + String(this->ClockIs24h));
        f.println("weatherShow=" + String(this->WeatherShow));
        f.println("weatherApiKey=" + this->WeatherApiKey);
        f.println("weatherCityId=" + String(this->WeatherCityId));
        f.println("weatherIsMetric=" + String(this->WeatherIsMetric));
        f.println("weatherLang=" + this->WeatherLang);
        f.println("useLedFlash=" + String(this->UseLedFlash));
    }
    f.close();
    readSettings();
}

String GlobalDataController::getVersion() {
    return VERSION;
}

String GlobalDataController::getLastReportStatus() {
    return this->lastReportStatus;
}

TimeClient *GlobalDataController::getTimeClient() {
    return this->timeClient;
}

OpenWeatherMapClient *GlobalDataController::getWeatherClient() {
    return this->weatherClient;
}

void GlobalDataController::setPrinterClient(BasePrinterClient *basePrinterClient) {
    this->basePrinterClient = basePrinterClient;
}
    
BasePrinterClient *GlobalDataController::getPrinterClient() {
    return this->basePrinterClient;
}

void GlobalDataController::setDisplayClient(BaseDisplayClient *baseDisplayClient) {
    this->baseDisplayClient = baseDisplayClient;
}

BaseDisplayClient *GlobalDataController::getDisplayClient() {
    return this->baseDisplayClient;
}

/**
 * Configuration parameters
 */

String GlobalDataController::getPrinterApiKey() {
    return this->PrinterApiKey;
}

void GlobalDataController::setPrinterApiKey(String printerApiKey) {
    this->PrinterApiKey = printerApiKey;
}

String GlobalDataController::getPrinterHostName() {
    return this->PrinterHostName;
}

void GlobalDataController::setPrinterHostName(String printerHostName) {
    this->PrinterHostName = printerHostName;
}

String GlobalDataController::getPrinterServer() {
    return this->PrinterServer;
}

void GlobalDataController::setPrinterServer(String printerServer) {
    this->PrinterServer = printerServer;
}

int GlobalDataController::getPrinterPort() {
    return this->PrinterPort;
}

void GlobalDataController::setPrinterPort(int printerPort) {
    this->PrinterPort = printerPort;
}

String GlobalDataController::getPrinterAuthUser() {
    return this->PrinterAuthUser;
}

void GlobalDataController::setPrinterAuthUser(String printerAuthUser) {
    this->PrinterAuthUser = printerAuthUser;
}

String GlobalDataController::getPrinterAuthPass() {
    return this->PrinterAuthPass;
}

void GlobalDataController::setPrinterAuthPass(String printerAuthPass) {
    this->PrinterAuthPass = printerAuthPass;
}

bool GlobalDataController::hasPrinterPsu() {
    return this->PrinterHasPsu;
}

void GlobalDataController::setHasPrinterPsu(bool hasPsu) {
    this->PrinterHasPsu = hasPsu;
}

int GlobalDataController::getWebserverPort() {
    return this->WebserverPort;
}

bool GlobalDataController::getWebserverIsBasicAuth() {
    return this->WebserverIsBasicAuth;
}

void GlobalDataController::setWebserverIsBasicAuth(bool webserverIsBasicAuth) {
    this->WebserverIsBasicAuth = webserverIsBasicAuth;
}

String GlobalDataController::getWebserverUsername() {
    return this->WebserverUsername;
}

void GlobalDataController::setWebserverUsername(String webserverUsername) {
    this->WebserverUsername = webserverUsername;
}

String GlobalDataController::getWebserverPassword() {
    return this->WebserverPassword;
}

void GlobalDataController::setWebserverPassword(String webserverPassword) {
    this->WebserverPassword = webserverPassword;
}

String GlobalDataController::getWebserverTheme() {
    return this->WebserverTheme;
}

void GlobalDataController::setWebserverTheme(String webserverTheme) {
    this->WebserverTheme = webserverTheme;
}

bool GlobalDataController::isDisplayInverted() {
    return this->DisplayInvertDisplay;
}

void GlobalDataController::setIsDisplayInverted(bool displayInvertDisplay) {
    this->DisplayInvertDisplay = displayInvertDisplay;
}

int GlobalDataController::getClockUtcOffset() {
    return this->ClockUtcOffset;
}

void GlobalDataController::setClockUtcOffset(int clockUtcOffset) {
    this->ClockUtcOffset = clockUtcOffset;
}

bool GlobalDataController::getDisplayClock() {
    return this->DisplayClock;
}

void GlobalDataController::setDisplayClock(bool displayClock) {
    this->DisplayClock = displayClock;
}

bool GlobalDataController::getClockIs24h() {
    return this->ClockIs24h;
}

void GlobalDataController::setClockIs24h(bool clockIs24h) {
    this->ClockIs24h = clockIs24h;
}

int GlobalDataController::getClockResyncMinutes() {
    return this->ClockResyncMinutes;
}

void GlobalDataController::setClockResyncMinutes(int clockResyncMinutes) {
    this->ClockResyncMinutes = clockResyncMinutes;
}

bool GlobalDataController::useLedFlash() {
    return this->UseLedFlash;
}

void GlobalDataController::setUseLedFlash(bool useLedFlash) {
    this->UseLedFlash = useLedFlash;
}

bool GlobalDataController::getWeatherShow() {
    return this->WeatherShow;
}

void GlobalDataController::setWeatherShow(bool weatherShow) {
    this->WeatherShow = weatherShow;
}

String GlobalDataController::getWeatherApiKey() {
    return this->WeatherApiKey;
}

void GlobalDataController::setWeatherApiKey(String weatherApiKey) {
    this->WeatherApiKey = weatherApiKey;
}

int GlobalDataController::getWeatherCityId() {
    return this->WeatherCityId;
}

void GlobalDataController::setWeatherCityId(int weatherCityId) {
    this->WeatherCityId = weatherCityId;
}

bool GlobalDataController::getWeatherIsMetric() {
    return this->WeatherIsMetric;
}

void GlobalDataController::setWeatherIsMetric(bool weatherIsMetric) {
    this->WeatherIsMetric = weatherIsMetric;
}

String GlobalDataController::getWeatherLang() {
    return this->WeatherLang;
}
 
void GlobalDataController::setWeatherLang(String weatherLang) {
    this->WeatherLang = weatherLang;
}

/**
 * Notify LED
 */

void GlobalDataController::ledOnOff(boolean value) {
  if (this->useLedFlash()) {
    if (value) {
      digitalWrite(EXTERNAL_LED, LOW); // LED ON
    } else {
      digitalWrite(EXTERNAL_LED, HIGH);  // LED OFF
    }
  }
}

void GlobalDataController::flashLED(int number, int delayTime) {
  for (int inx = 0; inx <= number; inx++) {
      delay(delayTime);
      digitalWrite(EXTERNAL_LED, LOW); // ON
      delay(delayTime);
      digitalWrite(EXTERNAL_LED, HIGH); // OFF
      delay(delayTime);
  }
}

bool GlobalDataController::resetConfig() {
    return LittleFS.remove(CONFIG);
}

/**
 * Global used functions
 */

int8_t GlobalDataController::getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if(dbm <= -100) {
      return 0;
  } else if(dbm >= -50) {
      return 100;
  } else {
      return 2 * (dbm + 100);
  }
}