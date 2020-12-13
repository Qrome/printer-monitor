#include "GlobalDataController.h"

GlobalDataController::GlobalDataController() {
    
}



void GlobalDataController::setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println("Debugger started");
    this->listSettingFiles();
    this->readSettings();
}

void GlobalDataController::listSettingFiles() {
    this->debugPrintLn("========= FileSystem Files =================");
    Dir dir = LittleFS.openDir("/");
    while (dir.next())
    {
        this->debugPrintLn(dir.fileName());
    }
}

void GlobalDataController::readSettings() {
    if (LittleFS.exists(CONFIG) == false) {
        this->debugPrintLn("Settings File does not yet exists.");
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
            this->debugPrintLn("PrinterApiKey=" + this->PrinterApiKey);
        }
        if (line.indexOf("printerHostName=") >= 0) {
            this->PrinterHostName = line.substring(line.lastIndexOf("printerHostName=") + 16);
            this->PrinterHostName.trim();
            this->debugPrintLn("PrinterHostName=" + this->PrinterHostName);
        }
        if (line.indexOf("printerServer=") >= 0) {
            this->PrinterServer = line.substring(line.lastIndexOf("printerServer=") + 14);
            this->PrinterServer.trim();
            this->debugPrintLn("PrinterServer=" + this->PrinterServer);
        }
        if (line.indexOf("printerPort=") >= 0) {
            this->PrinterPort = line.substring(line.lastIndexOf("printerPort=") + 12).toInt();
            this->debugPrintLn("PrinterPort=" + String(this->PrinterPort));
        }
        /*if (line.indexOf("printerName=") >= 0) {
            String printer = line.substring(line.lastIndexOf("printerName=") + 12);
            printer.trim();
            printerClient.setPrinterName(printer);
            this->debugPrintLn("PrinterName=" + printerClient.getPrinterName());
        }*/
        if (line.indexOf("printerAuthUser=") >= 0) {
            this->PrinterAuthUser = line.substring(line.lastIndexOf("printerAuthUser=") + 16);
            this->PrinterAuthUser.trim();
            this->debugPrintLn("PrinterAuthUser=" + this->PrinterAuthUser);
        }
        if (line.indexOf("printerAuthPass=") >= 0) {
            this->PrinterAuthPass = line.substring(line.lastIndexOf("printerAuthPass=") + 16);
            this->PrinterAuthPass.trim();
            this->debugPrintLn("PrinterAuthPass=" + this->PrinterAuthPass);
        }
        if (line.indexOf("printerHasPsu=") >= 0) {
            this->PrinterHasPsu = line.substring(line.lastIndexOf("printerHasPsu=") + 14).toInt();
            this->debugPrintLn("PrinterHasPsu=" + String(this->PrinterHasPsu));
        }


#ifndef USE_NEXTION_DISPLAY
        if(line.indexOf("displayInvertDisplay=") >= 0) {
            this->DisplayInvertDisplay = line.substring(line.lastIndexOf("displayInvertDisplay=") + 21).toInt();
            this->debugPrintLn("DisplayInvertDisplay=" + String(this->DisplayInvertDisplay));
        }
#endif

        if (line.indexOf("webserverTheme=") >= 0) {
            this->WebserverTheme = line.substring(line.lastIndexOf("webserverTheme=") + 15);
            this->WebserverTheme.trim();
            this->debugPrintLn("webserverTheme=" + this->WebserverTheme);
        }
        if (line.indexOf("webserverIsBasicAuth=") >= 0) {
            this->WebserverIsBasicAuth = line.substring(line.lastIndexOf("webserverIsBasicAuth=") + 21).toInt();
            this->debugPrintLn("webserverIsBasicAuth=" + String(this->WebserverIsBasicAuth));
        }
        if (line.indexOf("webserverUsername=") >= 0) {
            this->WebserverUsername = line.substring(line.lastIndexOf("webserverUsername=") + 18);
            this->WebserverUsername.trim();
            this->debugPrintLn("webserverUsername=" + this->WebserverUsername);
        }
        if (line.indexOf("webserverPassword=") >= 0) {
            this->WebserverPassword = line.substring(line.lastIndexOf("webserverPassword=") + 18);
            this->WebserverPassword.trim();
            this->debugPrintLn("webserverPassword=" + this->WebserverPassword);
        }




        /*
        if (line.indexOf("UtcOffset=") >= 0) {
        UtcOffset = line.substring(line.lastIndexOf("UtcOffset=") + 10).toFloat();
        debugHandle->printLn("UtcOffset=" + String(UtcOffset));
        }
        
        
        
        if (line.indexOf("refreshRate=") >= 0) {
        minutesBetweenDataRefresh = line.substring(line.lastIndexOf("refreshRate=") + 12).toInt();
        debugHandle->printLn("minutesBetweenDataRefresh=" + String(minutesBetweenDataRefresh));
        }
        
        if (line.indexOf("DISPLAYCLOCK=") >= 0) {
        DISPLAYCLOCK = line.substring(line.lastIndexOf("DISPLAYCLOCK=") + 13).toInt();
        debugHandle->printLn("DISPLAYCLOCK=" + String(DISPLAYCLOCK));
        }
        if (line.indexOf("is24hour=") >= 0) {
        IS_24HOUR = line.substring(line.lastIndexOf("is24hour=") + 9).toInt();
        debugHandle->printLn("IS_24HOUR=" + String(IS_24HOUR));
        }
    
        if(line.indexOf("USE_FLASH=") >= 0) {
        USE_FLASH = line.substring(line.lastIndexOf("USE_FLASH=") + 10).toInt();
        debugHandle->printLn("USE_FLASH=" + String(USE_FLASH));
        }
        
        if (line.indexOf("isWeather=") >= 0) {
        DISPLAYWEATHER = line.substring(line.lastIndexOf("isWeather=") + 10).toInt();
        debugHandle->printLn("DISPLAYWEATHER=" + String(DISPLAYWEATHER));
        }
        if (line.indexOf("weatherKey=") >= 0) {
        WeatherApiKey = line.substring(line.lastIndexOf("weatherKey=") + 11);
        WeatherApiKey.trim();
        debugHandle->printLn("WeatherApiKey=" + WeatherApiKey);
        }
        if (line.indexOf("CityID=") >= 0) {
        CityIDs[0] = line.substring(line.lastIndexOf("CityID=") + 7).toInt();
        debugHandle->printLn("CityID: " + String(CityIDs[0]));
        }
        if (line.indexOf("isMetric=") >= 0) {
        IS_METRIC = line.substring(line.lastIndexOf("isMetric=") + 9).toInt();
        debugHandle->printLn("IS_METRIC=" + String(IS_METRIC));
        }
        if (line.indexOf("language=") >= 0) {
        WeatherLanguage = line.substring(line.lastIndexOf("language=") + 9);
        WeatherLanguage.trim();
        debugHandle->printLn("WeatherLanguage=" + WeatherLanguage);
        }*/
    }
    fr.close();
    //printerClient.updatePrintClient(PrinterApiKey, PrinterServer, PrinterPort, PrinterAuthUser, PrinterAuthPass, HAS_PSU);
    //weatherClient.updateWeatherApiKey(WeatherApiKey);
    //weatherClient.updateLanguage(WeatherLanguage);
    //weatherClient.setMetric(IS_METRIC);
    //weatherClient.updateCityIdList(CityIDs, 1);
    //timeClient.setUtcOffset(UtcOffset);
}

void GlobalDataController::writeSettings() {
  // Save decoded message to SPIFFS file for playback on power up.
  File f = LittleFS.open(CONFIG, "w");
  if (!f) {
    this->debugPrintLn("File open failed!");
  } else {
    this->debugPrintLn("Saving settings now...");
    f.println("printerApiKey=" + this->PrinterApiKey);
    f.println("printerHostName=" + this->PrinterHostName);
    f.println("printerServer=" + this->PrinterServer);
    f.println("printerPort=" + String(this->PrinterPort));
    //f.println("printerName=" + printerClient.getPrinterName());
    f.println("printerAuthUser=" + this->PrinterAuthUser);
    f.println("printerAuthPass=" + this->PrinterAuthPass);
    f.println("printerHasPsu=" + String(this->PrinterHasPsu));
#ifndef USE_NEXTION_DISPLAY
    f.println("displayInvertDisplay=" + String(this->DisplayInvertDisplay));
#endif
    f.println("webserverTheme=" + this->WebserverTheme);
    f.println("webserverIsBasicAuth=" + String(this->WebserverIsBasicAuth));
    f.println("webserverUsername=" + String(this->WebserverUsername));
    f.println("webserverPassword=" + String(this->WebserverPassword));


    /*f.println("UtcOffset=" + String(UtcOffset));
    f.println("refreshRate=" + String(minutesBetweenDataRefresh));  
    f.println("DISPLAYCLOCK=" + String(DISPLAYCLOCK));
    f.println("is24hour=" + String(IS_24HOUR));
    f.println("USE_FLASH=" + String(USE_FLASH));
    f.println("isWeather=" + String(DISPLAYWEATHER));
    f.println("weatherKey=" + WeatherApiKey);
    f.println("CityID=" + String(CityIDs[0]));
    f.println("isMetric=" + String(IS_METRIC));
    f.println("language=" + String(WeatherLanguage));
    */
  }
  f.close();
  readSettings();
  //timeClient.setUtcOffset(UtcOffset);
}

String GlobalDataController::getVersion() {
    return VERSION;
}

String GlobalDataController::getPrinterClientType() {
    return this->printerClientTypeName;
}

void GlobalDataController::setPrinterClientType(String clientTypeName) {
    this->printerClientTypeName = clientTypeName;
}












String GlobalDataController::getPrinterApiKey() {
    return this->PrinterApiKey;
}

String GlobalDataController::getPrinterHostName() {
    return this->PrinterHostName;
}

String GlobalDataController::getPrinterServer() {
    return this->PrinterServer;
}

int GlobalDataController::getPrinterPort() {
    return this->PrinterPort;
}

String GlobalDataController::getPrinterAuthUser() {
    return this->PrinterAuthUser;
}

String GlobalDataController::getPrinterAuthPass() {
    return this->PrinterAuthPass;
}

bool GlobalDataController::hasPrinterPsu() {
    return this->PrinterHasPsu;
}

int GlobalDataController::getWebserverPort() {
    return this->WebserverPort;
}

bool GlobalDataController::getWebserverIsBasicAuth() {
    return this->WebserverIsBasicAuth;
}

String GlobalDataController::getWebserverUsername() {
    return this->WebserverUsername;
}

String GlobalDataController::getWebserverPassword() {
    return this->WebserverPassword;
}

String GlobalDataController::getWebserverTheme() {
    return this->WebserverTheme;
}


bool GlobalDataController::isDisplayInverted() {
    return this->DisplayInvertDisplay;
}

/**
 * Debug outputs
 */

void GlobalDataController::debugPrint(const char *data) {
    Serial.print(data);
}

void GlobalDataController::debugPrint(String data) {
    Serial.print(data);
}

void GlobalDataController::debugPrint(int8_t data) {
    Serial.print(data);
}

void GlobalDataController::debugPrintF(const char *data, unsigned int uInt) {
    Serial.printf(data, uInt);
}

void GlobalDataController::debugPrintLn(const char *data) {
    Serial.println(data);
}

void GlobalDataController::debugPrintLn(String data) {
    Serial.println(data);
}

void GlobalDataController::debugPrintLn(long int data) {
    Serial.println(data);
}

void GlobalDataController::debugPrintLn(int8_t data) {
    Serial.println(data);
}

/**
 * Notify LED
 */

void GlobalDataController::ledOnOff(boolean value) {
  if (USE_FLASH) {
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