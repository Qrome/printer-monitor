#include "Includes.h"
#include <ArduinoOTA.h>

String lastMinute = "xx";
String lastSecond = "xx";
void configModeCallback(WiFiManager *myWiFiManager);

void setup() {
    LittleFS.begin();
    debugController.setup();
    globalDataController.setPrinterClient(&printerClient);
    globalDataController.setDisplayClient(&displayClient);
    globalDataController.setup();
    displayClient.preSetup();
    displayClient.showBootScreen();

    // WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(DEBUG_MODE_ENABLE);
    //wifiManager.resetSettings();    // Uncomment for testing wifi manager
    wifiManager.setAPCallback(configModeCallback);
    String hostname(HOSTNAME);
    hostname += String(ESP.getChipId(), HEX);
    WiFi.hostname(hostname);
    if (!wifiManager.autoConnect((const char *)hostname.c_str())) { // new addition
      delay(3000);
      WiFi.disconnect(true);
      ESP.reset();
      delay(5000);
    }
    
    displayClient.postSetup();

    // print the received signal strength:
    debugController.print("Signal Strength (RSSI): ");
    debugController.print(globalDataController.getWifiQuality());
    debugController.printLn("%");

    if (ENABLE_OTA) {
        ArduinoOTA.onStart([]() {
            debugController.printLn("Start");
        });
        ArduinoOTA.onEnd([]() {
            debugController.printLn("\nEnd");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            debugController.printF("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            debugController.printF("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) debugController.printLn("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) debugController.printLn("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) debugController.printLn("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) debugController.printLn("Receive Failed");
            else if (error == OTA_END_ERROR) debugController.printLn("End Failed");
        });
        ArduinoOTA.setHostname((const char *)hostname.c_str()); 
        if (String(OTA_Password) != "") {
            ArduinoOTA.setPassword((OTA_Password));
        }
        ArduinoOTA.begin();
    }

    debugController.printLn("local ip");
    debugController.printLn(WiFi.localIP().toString());

#if WEBSERVER_ENABLED
    webServer.setup();
    displayClient.showWebserverSplashScreen(true);
#else
    displayClient.showWebserverSplashScreen(false);
#endif

    globalDataController.flashLED(5, 100);
    webServer.findMDNS();
    debugController.printLn("*** Leaving setup()");
}

void loop() {

    // Handle update of time
    if(timeClient.handleSync(globalDataController.getClockResyncMinutes()) && globalDataController.getWeatherShow()) {
        // We sync time? Ok, sync weather also!
        debugController.printLn("Updating weather...");
        weatherClient.updateWeather();
    }

    if (lastMinute != timeClient.getMinutes() && !printerClient.isPrinting()) {
        // Check status every 60 seconds
        globalDataController.ledOnOff(true);
        lastMinute = timeClient.getMinutes(); // reset the check value
        printerClient.getPrinterJobResults();
        printerClient.getPrinterPsuState();
        globalDataController.ledOnOff(false);
    } else if (printerClient.isPrinting()) {
        if (lastSecond != timeClient.getSeconds() && timeClient.getSeconds().endsWith("0")) {
            lastSecond = timeClient.getSeconds();
            // every 10 seconds while printing get an update
            globalDataController.ledOnOff(true);
            printerClient.getPrinterJobResults();
            printerClient.getPrinterPsuState();
            globalDataController.ledOnOff(false);
        }
    }

    displayClient.handleUpdate();

    if (WEBSERVER_ENABLED) {
        webServer.handleClient();
    }
    if (ENABLE_OTA) {
        ArduinoOTA.handle();
    }
}

void configModeCallback(WiFiManager *myWiFiManager) {
    debugController.printLn("Entered config mode");
    debugController.printLn(WiFi.softAPIP().toString());
    displayClient.showApAccessScreen(myWiFiManager->getConfigPortalSSID(), WiFi.softAPIP().toString());
    debugController.printLn("Wifi Manager");
    debugController.printLn("Please connect to AP");
    debugController.printLn(myWiFiManager->getConfigPortalSSID());
    debugController.printLn("To setup Wifi Configuration");
    globalDataController.flashLED(20, 50);
}
