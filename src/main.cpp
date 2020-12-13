#include "Includes.h"
#include <ArduinoOTA.h>




void configModeCallback(WiFiManager *myWiFiManager);



void setup() {
    LittleFS.begin();
    debugController.setup();
    globalDataController.setPrinterClient(&printerClient);
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
    debugController.printLn("*** Leaving setup()");
}

void loop() {

    // Handle update of time
    timeClient.handleSync(globalDataController.getClockResyncMinutes());

 

    // put your main code here, to run repeatedly:









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