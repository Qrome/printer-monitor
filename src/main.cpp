#include "Includes.h"
#include <ArduinoOTA.h>

String lastMinute = "xx";
String lastSecond = "xx";
int knownPrintersToSync = 0;
int currentRefreshPrinter = 0;

void configModeCallback(WiFiManager *myWiFiManager);

/**
 * @brief Setup/Initialize ESP
 */
void setup() {
    LittleFS.begin();
    debugController.setup();
    globalDataController.registerPrinterClient(PRINTER_CLIENT_REPETIER, &printerClient3);
    globalDataController.registerPrinterClient(PRINTER_CLIENT_OCTOPRINT, &printerClient2);
    globalDataController.registerPrinterClient(PRINTER_CLIENT_KLIPPER, &printerClient1);
    globalDataController.registerPrinterClient(PRINTER_CLIENT_DUET, &printerClient0);
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
    debugController.print(EspController::getWifiQuality());
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

/**
 * @brief Loop trough all
 */
void loop() {

    // Handle update of time
    if(timeClient.handleSync(globalDataController.getSystemSettings()->clockWeatherResyncMinutes) 
        && globalDataController.getWeatherSettings()->show
    ) {
        // We sync time? Ok, sync weather also!
        debugController.printLn("Updating weather...");
        weatherClient.updateWeather();
    }

    // Check printers 
    if (knownPrintersToSync != globalDataController.getNumPrinters()) {
        knownPrintersToSync = globalDataController.getNumPrinters();
        currentRefreshPrinter = 0;
    }

    // Sync only if we have printers
    if (knownPrintersToSync > 0) {
        if (currentRefreshPrinter >= globalDataController.getNumPrinters()) {
            currentRefreshPrinter = 0;
        }
        PrinterDataStruct *refPrinter = &globalDataController.getPrinterSettings()[currentRefreshPrinter];

        bool syncPrinter = false;
        if ((refPrinter->lastSyncEpoch == 0)
            || (!refPrinter->isPrinting && (timeClient.getSecondsFromLast(refPrinter->lastSyncEpoch) >= PRINTER_SYNC_SEC))
            || (refPrinter->isPrinting && (timeClient.getSecondsFromLast(refPrinter->lastSyncEpoch) >= PRINTER_SYNC_SEC_PRINTING))
        ) {
            syncPrinter = true;
        }
        if (syncPrinter) {
            globalDataController.ledOnOff(true);
            refPrinter->lastSyncEpoch = timeClient.getCurrentEpoch();
            globalDataController.syncPrinter(refPrinter);
            globalDataController.ledOnOff(false);
        }

        // Next Time, next printer 
        currentRefreshPrinter++;
    }

    // Handle Display
    displayClient.handleUpdate();

    // Handle all Web stuff
    if (WEBSERVER_ENABLED) {
        webServer.handleClient();
    }
    if (ENABLE_OTA) {
        ArduinoOTA.handle();
    }
}

/**
 * @brief AP Mode for WiFi configuration
 * @param myWiFiManager 
 */
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
