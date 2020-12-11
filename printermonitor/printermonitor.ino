/** The MIT License (MIT)

Copyright (c) 2018 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Additional Contributions:
/* 15 Jan 2019 : Owen Carter : Add psucontrol option and processing */
/* 12/6/20 : Matthias Grimm : Add Duet Monitor */

 /**********************************************
 * Edit Settings.h for personalization
 ***********************************************/
#include "Settings.h"









#include "Global.h"

// Debug output
Debug *debugHandle = new Debug();

// Time 
TimeClient timeClient(UtcOffset, IS_24HOUR, debugHandle);
long lastEpoch = 0;
long firstEpoch = 0;
long displayOffEpoch = 0;
String lastMinute = "xx";
String lastSecond = "xx";
String lastReportStatus = "";
boolean displayOn = true;

// Printer Client
#if defined(USE_REPETIER_CLIENT)
  RepetierClient printerClient(PrinterApiKey, PrinterServer, PrinterPort, PrinterAuthUser, PrinterAuthPass, HAS_PSU, debugHandle);
#elif defined(USE_KLIPPER_CLIENT)
  KlipperClient printerClient(PrinterApiKey, PrinterServer, PrinterPort, PrinterAuthUser, PrinterAuthPass, HAS_PSU, debugHandle);
#elif defined(USE_DUET_CLIENT)
  DuetClient printerClient(PrinterApiKey, PrinterServer, PrinterPort, PrinterAuthUser, PrinterAuthPass, HAS_PSU, debugHandle);
#else
  OctoPrintClient printerClient(PrinterApiKey, PrinterServer, PrinterPort, PrinterAuthUser, PrinterAuthPass, HAS_PSU, debugHandle);
#endif
int printerCount = 0;

// Weather Client
OpenWeatherMapClient weatherClient(WeatherApiKey, CityIDs, 1, IS_METRIC, WeatherLanguage, debugHandle);

// Webserver
ESP8266WebServer server(WEBSERVER_PORT);
ESP8266HTTPUpdateServer serverUpdater;
WebServer webServer(&server, debugHandle, &printerClient, &timeClient, themeColor, String(VERSION), HAS_PSU);

// Display
#ifdef USE_NEXTION_DISPLAY
  NextionDisplayClient displayHandle(RX_PIN, TX_PIN, &printerClient);
#else
  OledDisplayClient displayHandle(SDA_PIN, SCL_PIN, I2C_DISPLAY_ADDRESS, DISPLAY_SH1106, INVERT_DISPLAY, &printerClient);
#endif

//declairing prototypes
void configModeCallback(WiFiManager *myWiFiManager);
int8_t getWifiQuality();
void flashLED(int number, int delayTime);
void ledOnOff(boolean value);
void readSettings();
void writeSettings();


//************************************************************
// Setup
//************************************************************
void setup() {  
  debugHandle->initialize();
  webServer.setAuthentication(IS_BASIC_AUTH, www_username, www_password);
  SPIFFS.begin();

  // Initialize digital pin for LED (little blue light on the Wemos D1 Mini)
  pinMode(externalLight, OUTPUT);

  // Some Defaults before loading from Config.txt and load
  PrinterPort = printerClient.getPrinterPort();
  readSettings();

  // Show boot screen
  displayHandle.showPageInit(printerClient.getPrinterType(), String(VERSION));
  
  // WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //wifiManager.resetSettings();  // Uncomment for testing wifi manager
  wifiManager.setAPCallback(configModeCallback);
  
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  if (!wifiManager.autoConnect((const char *)hostname.c_str())) {// new addition
    delay(3000);
    WiFi.disconnect(true);
    ESP.reset();
    delay(5000);
  }

  // Display PostSetup
  displayHandle.postSetup();

  // print the received signal strength:
  debugHandle->print("Signal Strength (RSSI): ");
  debugHandle->print(getWifiQuality());
  debugHandle->printLn("%");
  
  if (ENABLE_OTA) {
    ArduinoOTA.onStart([]() {
      debugHandle->printLn("Start");
    });
    ArduinoOTA.onEnd([]() {
      debugHandle->printLn("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      debugHandle->printF("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      debugHandle->printF("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) debugHandle->printLn("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) debugHandle->printLn("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) debugHandle->printLn("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) debugHandle->printLn("Receive Failed");
      else if (error == OTA_END_ERROR) debugHandle->printLn("End Failed");
    });
    ArduinoOTA.setHostname((const char *)hostname.c_str()); 
    if (OTA_Password != "") {
      ArduinoOTA.setPassword(((const char *)OTA_Password.c_str()));
    }
    ArduinoOTA.begin();
  }

  if (WEBSERVER_ENABLED) {
    webServer.setHostname(hostname);
    server.on("/", []() { webServer.displayPrinterStatus(); });
    server.on("/systemreset", []() { webServer.handleSystemReset(); });
    server.on("/forgetwifi", []() { webServer.handleWifiReset(); });
    //server.on("/updateconfig", handleUpdateConfig);
    //server.on("/updateweatherconfig", handleUpdateWeather);
    //server.on("/configure", handleConfigure);
    //server.on("/configureweather", handleWeatherConfigure);
    server.onNotFound([]() { webServer.redirectHome(); });   
    //serverUpdater.setup(&server, "/update", www_username, www_password);
    // Start the server
    server.begin();
    debugHandle->printLn("Server started");
    // Print the IP address
    String webAddress = "http://" + WiFi.localIP().toString() + ":" + String(WEBSERVER_PORT) + "/";
    debugHandle->printLn("Use this URL : " + webAddress);
    //display.clear();
    //display.setTextAlignment(TEXT_ALIGN_CENTER);
    //display.setFont(ArialMT_Plain_10);
    //display.drawString(64, 10, "Web Interface On");
    //display.drawString(64, 20, "You May Connect to IP");
    //display.setFont(ArialMT_Plain_16);
    //display.drawString(64, 30, WiFi.localIP().toString());
    //display.drawString(64, 46, "Port: " + String(WEBSERVER_PORT));
    //display.display();
  } else {
    debugHandle->printLn("Web Interface is Disabled");
    //display.clear();
    //display.setTextAlignment(TEXT_ALIGN_CENTER);
    //display.setFont(ArialMT_Plain_10);
    //display.drawString(64, 10, "Web Interface is Off");
    //display.drawString(64, 20, "Enable in Settings.h");
    //display.display(); 
  }
  flashLED(5, 100);
  findMDNS();  //go find Printer Server by the hostname
  debugHandle->printLn("*** Leaving setup()");
}


//************************************************************
// Main Loop
//************************************************************
void loop() {


  //displayHandle.showPageInit();
  //delay(1000);

  if (WEBSERVER_ENABLED) {
    server.handleClient();
  }
  if (ENABLE_OTA) {
    ArduinoOTA.handle();
  }
}












//************************************************************
// MDNS for printers
//************************************************************
void findMDNS() {
  if (PrinterHostName == "" || ENABLE_OTA == false) {
    return; // nothing to do here
  }
  // We now query our network for 'web servers' service
  // over tcp, and get the number of available devices
  int n = MDNS.queryService("http", "tcp");
  if (n == 0) {
    debugHandle->printLn("no services found - make sure Printer server is turned on");
    return;
  }
  debugHandle->printLn("*** Looking for " + PrinterHostName + " over mDNS");
  for (int i = 0; i < n; ++i) {
    // Going through every available service,
    // we're searching for the one whose hostname
    // matches what we want, and then get its IP
    debugHandle->printLn("Found: " + MDNS.hostname(i));
    if (MDNS.hostname(i) == PrinterHostName) {
      IPAddress serverIp = MDNS.IP(i);
      PrinterServer = serverIp.toString();
      PrinterPort = MDNS.port(i); // save the port
      debugHandle->printLn("*** Found Printer Server " + PrinterHostName + " http://" + PrinterServer + ":" + PrinterPort);
      writeSettings(); // update the settings
    }
  }
}

//************************************************************
// Converts the dBm to a range between 0 and 100%
//************************************************************
int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if(dbm <= -100) {
      return 0;
  } else if(dbm >= -50) {
      return 100;
  } else {
      return 2 * (dbm + 100);
  }
}

//************************************************************
// Wifi config mode
//************************************************************
void configModeCallback(WiFiManager *myWiFiManager) {
  debugHandle->printLn("Entered config mode");
  debugHandle->printLn(WiFi.softAPIP().toString());

  displayHandle.showPageApMode(myWiFiManager->getConfigPortalSSID());
  
  
  debugHandle->printLn("Wifi Manager");
  debugHandle->printLn("Please connect to AP");
  debugHandle->printLn(myWiFiManager->getConfigPortalSSID());
  debugHandle->printLn("To setup Wifi Configuration");
  flashLED(20, 50);
}

//************************************************************
// Flash LED
//************************************************************
void flashLED(int number, int delayTime) {
  for (int inx = 0; inx <= number; inx++) {
      delay(delayTime);
      digitalWrite(externalLight, LOW); // ON
      delay(delayTime);
      digitalWrite(externalLight, HIGH); // OFF
      delay(delayTime);
  }
}

//************************************************************
// Toggle LED
//************************************************************
void ledOnOff(boolean value) {
  if (USE_FLASH) {
    if (value) {
      digitalWrite(externalLight, LOW); // LED ON
    } else {
      digitalWrite(externalLight, HIGH);  // LED OFF
    }
  }
}

//************************************************************
// Read settings from internal eeprom
//************************************************************
void readSettings() {
  if (SPIFFS.exists(CONFIG) == false) {
    debugHandle->printLn("Settings File does not yet exists.");
    writeSettings();
    return;
  }
  File fr = SPIFFS.open(CONFIG, "r");
  String line;
  while(fr.available()) {
    line = fr.readStringUntil('\n');

    if (line.indexOf("UtcOffset=") >= 0) {
      UtcOffset = line.substring(line.lastIndexOf("UtcOffset=") + 10).toFloat();
      debugHandle->printLn("UtcOffset=" + String(UtcOffset));
    }
    if (line.indexOf("printerApiKey=") >= 0) {
      PrinterApiKey = line.substring(line.lastIndexOf("printerApiKey=") + 14);
      PrinterApiKey.trim();
      debugHandle->printLn("PrinterApiKey=" + PrinterApiKey);
    }
    if (line.indexOf("printerHostName=") >= 0) {
      PrinterHostName = line.substring(line.lastIndexOf("printerHostName=") + 16);
      PrinterHostName.trim();
      debugHandle->printLn("PrinterHostName=" + PrinterHostName);
    }
    if (line.indexOf("printerServer=") >= 0) {
      PrinterServer = line.substring(line.lastIndexOf("printerServer=") + 14);
      PrinterServer.trim();
      debugHandle->printLn("PrinterServer=" + PrinterServer);
    }
    if (line.indexOf("printerPort=") >= 0) {
      PrinterPort = line.substring(line.lastIndexOf("printerPort=") + 12).toInt();
      debugHandle->printLn("PrinterPort=" + String(PrinterPort));
    }
    if (line.indexOf("printerName=") >= 0) {
      String printer = line.substring(line.lastIndexOf("printerName=") + 12);
      printer.trim();
      printerClient.setPrinterName(printer);
      debugHandle->printLn("PrinterName=" + printerClient.getPrinterName());
    }
    if (line.indexOf("printerAuthUser=") >= 0) {
      PrinterAuthUser = line.substring(line.lastIndexOf("printerAuthUser=") + 16);
      PrinterAuthUser.trim();
      debugHandle->printLn("PrinterAuthUser=" + PrinterAuthUser);
    }
    if (line.indexOf("printerAuthPass=") >= 0) {
      PrinterAuthPass = line.substring(line.lastIndexOf("printerAuthPass=") + 16);
      PrinterAuthPass.trim();
      debugHandle->printLn("PrinterAuthPass=" + PrinterAuthPass);
    }
    if (line.indexOf("refreshRate=") >= 0) {
      minutesBetweenDataRefresh = line.substring(line.lastIndexOf("refreshRate=") + 12).toInt();
      debugHandle->printLn("minutesBetweenDataRefresh=" + String(minutesBetweenDataRefresh));
    }
    if (line.indexOf("themeColor=") >= 0) {
      themeColor = line.substring(line.lastIndexOf("themeColor=") + 11);
      themeColor.trim();
      debugHandle->printLn("themeColor=" + themeColor);
    }
    if (line.indexOf("IS_BASIC_AUTH=") >= 0) {
      IS_BASIC_AUTH = line.substring(line.lastIndexOf("IS_BASIC_AUTH=") + 14).toInt();
      debugHandle->printLn("IS_BASIC_AUTH=" + String(IS_BASIC_AUTH));
      webServer.setAuthentication(IS_BASIC_AUTH, www_username, www_password);
    }
    if (line.indexOf("www_username=") >= 0) {
      String temp = line.substring(line.lastIndexOf("www_username=") + 13);
      temp.trim();
      temp.toCharArray(www_username, sizeof(temp));
      debugHandle->printLn("www_username=" + String(www_username));
      webServer.setAuthentication(IS_BASIC_AUTH, www_username, www_password);
    }
    if (line.indexOf("www_password=") >= 0) {
      String temp = line.substring(line.lastIndexOf("www_password=") + 13);
      temp.trim();
      temp.toCharArray(www_password, sizeof(temp));
      debugHandle->printLn("www_password=" + String(www_password));
      webServer.setAuthentication(IS_BASIC_AUTH, www_username, www_password);
    }
    if (line.indexOf("DISPLAYCLOCK=") >= 0) {
      DISPLAYCLOCK = line.substring(line.lastIndexOf("DISPLAYCLOCK=") + 13).toInt();
      debugHandle->printLn("DISPLAYCLOCK=" + String(DISPLAYCLOCK));
    }
    if (line.indexOf("is24hour=") >= 0) {
      IS_24HOUR = line.substring(line.lastIndexOf("is24hour=") + 9).toInt();
      debugHandle->printLn("IS_24HOUR=" + String(IS_24HOUR));
    }
#ifndef USE_NEXTION_DISPLAY
    if(line.indexOf("invertDisp=") >= 0) {
      INVERT_DISPLAY = line.substring(line.lastIndexOf("invertDisp=") + 11).toInt();
      debugHandle->printLn("INVERT_DISPLAY=" + String(INVERT_DISPLAY));
    }
#endif
    if(line.indexOf("USE_FLASH=") >= 0) {
      USE_FLASH = line.substring(line.lastIndexOf("USE_FLASH=") + 10).toInt();
      debugHandle->printLn("USE_FLASH=" + String(USE_FLASH));
    }
    if (line.indexOf("hasPSU=") >= 0) {
      HAS_PSU = line.substring(line.lastIndexOf("hasPSU=") + 7).toInt();
      debugHandle->printLn("HAS_PSU=" + String(HAS_PSU));
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
    }
  }
  fr.close();
  printerClient.updatePrintClient(PrinterApiKey, PrinterServer, PrinterPort, PrinterAuthUser, PrinterAuthPass, HAS_PSU);
  weatherClient.updateWeatherApiKey(WeatherApiKey);
  weatherClient.updateLanguage(WeatherLanguage);
  weatherClient.setMetric(IS_METRIC);
  weatherClient.updateCityIdList(CityIDs, 1);
  timeClient.setUtcOffset(UtcOffset);
}

//************************************************************
// Save settings to internal eeprom
//************************************************************
void writeSettings() {
  // Save decoded message to SPIFFS file for playback on power up.
  File f = SPIFFS.open(CONFIG, "w");
  if (!f) {
    debugHandle->printLn("File open failed!");
  } else {
    debugHandle->printLn("Saving settings now...");
    f.println("UtcOffset=" + String(UtcOffset));
    f.println("printerApiKey=" + PrinterApiKey);
    f.println("printerHostName=" + PrinterHostName);
    f.println("printerServer=" + PrinterServer);
    f.println("printerPort=" + String(PrinterPort));
    f.println("printerName=" + printerClient.getPrinterName());
    f.println("printerAuthUser=" + PrinterAuthUser);
    f.println("printerAuthPass=" + PrinterAuthPass);
    f.println("refreshRate=" + String(minutesBetweenDataRefresh));
    f.println("themeColor=" + themeColor);
    f.println("IS_BASIC_AUTH=" + String(IS_BASIC_AUTH));
    f.println("www_username=" + String(www_username));
    f.println("www_password=" + String(www_password));
    f.println("DISPLAYCLOCK=" + String(DISPLAYCLOCK));
    f.println("is24hour=" + String(IS_24HOUR));
#ifndef USE_NEXTION_DISPLAY
    f.println("invertDisp=" + String(INVERT_DISPLAY));
#endif
    f.println("USE_FLASH=" + String(USE_FLASH));
    f.println("isWeather=" + String(DISPLAYWEATHER));
    f.println("weatherKey=" + WeatherApiKey);
    f.println("CityID=" + String(CityIDs[0]));
    f.println("isMetric=" + String(IS_METRIC));
    f.println("language=" + String(WeatherLanguage));
    f.println("hasPSU=" + String(HAS_PSU));
  }
  f.close();
  readSettings();
  timeClient.setUtcOffset(UtcOffset);
}
