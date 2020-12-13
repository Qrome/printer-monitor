#include "Includes.h"
#include <ArduinoOTA.h>


void configModeCallback(WiFiManager *myWiFiManager);



void setup() {
    LittleFS.begin();
    globalDataController.setup();
    displayClient.preSetup();
    displayClient.showBootScreen();

    // WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
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
    globalDataController.debugPrint("Signal Strength (RSSI): ");
    globalDataController.debugPrint(globalDataController.getWifiQuality());
    globalDataController.debugPrintLn("%");

    if (ENABLE_OTA) {
        ArduinoOTA.onStart([]() {
            globalDataController.debugPrintLn("Start");
        });
        ArduinoOTA.onEnd([]() {
            globalDataController.debugPrintLn("\nEnd");
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            globalDataController.debugPrintF("Progress: %u%%\r", (progress / (total / 100)));
        });
        ArduinoOTA.onError([](ota_error_t error) {
            globalDataController.debugPrintF("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) globalDataController.debugPrintLn("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) globalDataController.debugPrintLn("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) globalDataController.debugPrintLn("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) globalDataController.debugPrintLn("Receive Failed");
            else if (error == OTA_END_ERROR) globalDataController.debugPrintLn("End Failed");
        });
        ArduinoOTA.setHostname((const char *)hostname.c_str()); 
        if (String(OTA_Password) != "") {
            ArduinoOTA.setPassword((OTA_Password));
        }
        ArduinoOTA.begin();
    }

    globalDataController.debugPrintLn("local ip");
    globalDataController.debugPrintLn(WiFi.localIP().toString());

#if WEBSERVER_ENABLED
    webServer.setup();
    displayClient.showWebserverSplashScreen(true);
#else
    displayClient.showWebserverSplashScreen(false);
#endif

    globalDataController.flashLED(5, 100);
    globalDataController.debugPrintLn("*** Leaving setup()");
}

void loop() {
    // put your main code here, to run repeatedly:
}















void configModeCallback(WiFiManager *myWiFiManager) {
  globalDataController.debugPrintLn("Entered config mode");
  globalDataController.debugPrintLn(WiFi.softAPIP().toString());
  displayClient.showApAccessScreen(myWiFiManager->getConfigPortalSSID(), WiFi.softAPIP().toString());
  globalDataController.debugPrintLn("Wifi Manager");
  globalDataController.debugPrintLn("Please connect to AP");
  globalDataController.debugPrintLn(myWiFiManager->getConfigPortalSSID());
  globalDataController.debugPrintLn("To setup Wifi Configuration");
  globalDataController.flashLED(20, 50);
}