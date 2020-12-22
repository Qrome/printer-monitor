#include "WebServer.h"


String CHANGE_FORM =  ""; // moved to config to make it dynamic

WebServer::WebServer(GlobalDataController *globalDataController, DebugController *debugController) {
    this->globalDataController = globalDataController;
    this->debugController = debugController;
}

void WebServer::setup() {
    static WebServer* obj = this;

    this->server = new ESP8266WebServer(this->globalDataController->getSystemSettings()->webserverPort);
    this->serverUpdater = new ESP8266HTTPUpdateServer();

    this->server->on("/", []() { obj->handleMainPage(); });
    this->server->on("/systemreset", []() { obj->handleSystemReset(); });
    this->server->on("/forgetwifi", []() { obj->handleWifiReset(); });
    this->server->on("/configurestation/show", []() { obj->handleConfigureStation(); });
    this->server->on("/configurestation/update", []() { obj->handleUpdateStation(); });
    this->server->on("/configureprinter/show", []() { obj->handleConfigurePrinter(); });
    this->server->on("/configureprinter/edit", []() { obj->handleUpdatePrinter(); });
    this->server->on("/configureprinter/delete", []() { obj->handleDeletePrinter(); });
    this->server->on("/configureweather/show", []() { obj->handleConfigureWeather(); });
    this->server->on("/configureweather/update", []() { obj->handleUpdateWeather(); });
    this->server->on("/configuresensor/show", []() { obj->handleConfigureSensor(); });
    this->server->on("/configuresensor/update", []() { obj->handleConfigureSensor(); });
    this->server->on("/update", HTTP_GET, []() { obj->handleUpdatePage(); });

    this->server->onNotFound([]() { obj->redirectHome(); });
    this->serverUpdater->setup(
        this->server,
        "/update",
        this->globalDataController->getSystemSettings()->webserverUsername,
        this->globalDataController->getSystemSettings()->webserverPassword
    );
    

    // Start the server
    this->server->begin();
    this->debugController->printLn("Server started");
}


void WebServer::findMDNS() {
    return; // nothing to do here

    /*if (this->globalDataController->getPrinterHostName() == "" || ENABLE_OTA == false) {
        return; // nothing to do here
    }
    // We now query our network for 'web servers' service
    // over tcp, and get the number of available devices
    int n = MDNS.queryService("http", "tcp");
    if (n == 0) {
        this->debugController->printLn("no services found - make sure Printer server is turned on");
        return;
    }
    this->debugController->printLn("*** Looking for " + this->globalDataController->getPrinterHostName() + " over mDNS");
    for (int i = 0; i < n; ++i) {
        // Going through every available service,
        // we're searching for the one whose hostname
        // matches what we want, and then get its IP
        Serial.println("Found: " + MDNS.hostname(i));
        if (MDNS.hostname(i) == this->globalDataController->getPrinterHostName()) {
            IPAddress serverIp = MDNS.IP(i);
            this->globalDataController->setPrinterServer(serverIp.toString());
            this->globalDataController->setPrinterPort(MDNS.port(i));
            this->debugController->printLn(
                "*** Found Printer Server " + this->globalDataController->getPrinterHostName() +
                " http://" + this->globalDataController->getPrinterServer() + 
                ":" + this->globalDataController->getPrinterPort()
            );
            this->globalDataController->writeSettings(); // update the settings
        }
    }*/
}




/**
 * @brief Handle clients of webserver
 */
void WebServer::handleClient() {
    this->server->handleClient();
}

/**
 * @brief Redirect incomming transmission to other taget
 */
void WebServer::redirectTarget(String targetUri) {
    // Send them back to the Root Directory
    this->server->sendHeader("Location", targetUri, true);
    this->server->sendHeader("Cache-Control", "no-cache, no-store");
    this->server->sendHeader("Pragma", "no-cache");
    this->server->sendHeader("Expires", "-1");
    this->server->send(302, "text/plain", "");
    this->server->client().stop();
}

/**
 * @brief Redirect incomming transmission to dashboard
 */
void WebServer::redirectHome() {
    this->redirectTarget("/");
}

/**
 * @brief Handle authentification on all subsites
 * @return boolean 
 */
boolean WebServer::authentication() {
    SystemDataStruct *systemData = this->globalDataController->getSystemSettings();
    if (systemData->hasBasicAuth && (systemData->webserverUsername.length() >= 1 && systemData->webserverPassword.length() >= 1)
    ) {
        return this->server->authenticate(systemData->webserverUsername.c_str(), systemData->webserverPassword.c_str());
    } 
    return true; // Authentication not required
}

/**
 * @brief Send main page to client
 */
void WebServer::handleMainPage() {
    WebserverMemoryVariables::sendHeader(this->server, this->globalDataController, "Status", "Monitor", true);
    WebserverMemoryVariables::sendMainPage(this->server, this->globalDataController);
    WebserverMemoryVariables::sendFooter(this->server, this->globalDataController);
}

/**
 * @brief Send sensor configuration page to client
 */
void WebServer::handleConfigureSensor() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    WebserverMemoryVariables::sendHeader(this->server, this->globalDataController, "Configure", "Sensor");
    
    WebserverMemoryVariables::sendFooter(this->server, this->globalDataController);
}

/**
 * @brief Send printer configuration page to client
 */
void WebServer::handleConfigurePrinter() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    WebserverMemoryVariables::sendHeader(this->server, this->globalDataController, "Configure", "Printers");
    WebserverMemoryVariables::sendPrinterConfigForm(this->server, this->globalDataController);
    WebserverMemoryVariables::sendFooter(this->server, this->globalDataController);
}

/**
 * @brief Update configuration for Printer
 */
void WebServer::handleUpdatePrinter() {
    
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }

    PrinterDataStruct *targetPrinter = NULL;
    int targetPrinterId = this->server->arg("id").toInt() - 1;
    if (targetPrinterId >= 0) {
        PrinterDataStruct *existPrinters = this->globalDataController->getPrinterSettings();
        for(int i=0; i<this->globalDataController->getNumPrinters(); i++) {
            if (i == targetPrinterId) {
                targetPrinter = &(existPrinters[i]);
                break;
            }
        }
    } else {
        targetPrinter = this->globalDataController->addPrinterSetting();
    }
    if (targetPrinter == NULL) {
        this->globalDataController->getSystemSettings()->lastError = FPSTR(ERROR_MESSAGES_ERR1);
        this->redirectTarget("/configureprinter/show");
        return;
    }
    this->globalDataController->getSystemSettings()->lastError = "";

    // Set data
    MemoryHelper::stringToChar(this->server->arg("e-tname"), targetPrinter->customName, 20);
    targetPrinter->apiType = this->server->arg("e-tapi").toInt();
    MemoryHelper::stringToChar(this->server->arg("e-tapikey"), targetPrinter->apiKey, 60);
    MemoryHelper::stringToChar(this->server->arg("e-taddr"), targetPrinter->remoteAddress, 60);
    targetPrinter->remotePort = this->server->arg("e-tport").toInt();
    targetPrinter->hasPsuControl = this->server->hasArg("e-tpsu");
    targetPrinter->basicAuthNeeded = this->server->hasArg("e-tapipw");
    MemoryHelper::stringToChar(this->server->arg("e-tapiuser"), targetPrinter->basicAuthUsername, 30);
    MemoryHelper::stringToChar(this->server->arg("e-tapipass"), targetPrinter->basicAuthPassword, 60);

    // Reset error data
    MemoryHelper::stringToChar("", targetPrinter->error, 120);
    targetPrinter->state = PRINTER_STATE_OFFLINE;

    // Save
    this->globalDataController->getSystemSettings()->lastOk = FPSTR(OK_MESSAGES_SAVE1);
    this->globalDataController->writeSettings();
    this->redirectTarget("/configureprinter/show");
}

/**
 * @brief Delete single configuration for Printer
 */
void WebServer::handleDeletePrinter() {
    int targetPrinterId = this->server->arg("id").toInt() - 1;
    if (this->globalDataController->removePrinterSettingByIdx(targetPrinterId)) {
        this->globalDataController->getSystemSettings()->lastOk = FPSTR(OK_MESSAGES_DELETEPRINTER);
        this->globalDataController->getSystemSettings()->lastError = "";
        this->globalDataController->writeSettings();
    } else {
        this->globalDataController->getSystemSettings()->lastError = FPSTR(ERROR_MESSAGES_ERR2);
    }    
    this->redirectTarget("/configureprinter/show");
}

/**
 * @brief Send station configuration page to client
 */
void WebServer::handleConfigureStation() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    WebserverMemoryVariables::sendHeader(this->server, this->globalDataController, "Configure", "Station");
    WebserverMemoryVariables::sendStationConfigForm(this->server, this->globalDataController);
    WebserverMemoryVariables::sendFooter(this->server, this->globalDataController);
}

/**
 * @brief Update configuration for station
 */
void WebServer::handleUpdateStation() {
    
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    SystemDataStruct *systemSettings = this->globalDataController->getSystemSettings();
    ClockDataStruct *clockSettings = this->globalDataController->getClockSettings();
    boolean flipOld = systemSettings->invertDisplay;

    clockSettings->show = this->server->hasArg("isClockEnabled");
    clockSettings->is24h = this->server->hasArg("is24hour");
    clockSettings->utcOffset = this->server->arg("utcoffset").toInt();
    systemSettings->clockWeatherResyncMinutes = this->server->arg("refresh").toInt();
    systemSettings->hasBasicAuth = this->server->hasArg("isBasicAuth");
    systemSettings->invertDisplay = this->server->hasArg("invDisp");
    systemSettings->useLedFlash = this->server->hasArg("useFlash");
    systemSettings->webserverPassword = this->server->arg("stationpassword");
    systemSettings->webserverUsername = this->server->arg("userid");
    this->globalDataController->writeSettings();
    this->findMDNS();

    if (systemSettings->invertDisplay != flipOld) {
        this->globalDataController->getDisplayClient()->flipDisplayUpdate();
    }
    
    this->globalDataController->getDisplayClient()->handleUpdate();
    this->globalDataController->getTimeClient()->resetLastEpoch();
    this->globalDataController->getSystemSettings()->lastOk = FPSTR(OK_MESSAGES_SAVE3);
    this->redirectHome();
}

/**
 * @brief Send weather configuration page to client
 */
void WebServer::handleConfigureWeather() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    WebserverMemoryVariables::sendHeader(this->server, this->globalDataController, "Configure", "Weather");
    WebserverMemoryVariables::sendWeatherConfigForm(this->server, this->globalDataController);
    WebserverMemoryVariables::sendFooter(this->server, this->globalDataController);
}

/**
 * @brief Update configuration for weather
 */
void WebServer::handleUpdateWeather() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    WeatherDataStruct *weatherSettings = this->globalDataController->getWeatherSettings();

    weatherSettings->show = this->server->hasArg("isWeatherEnabled");
    weatherSettings->apiKey = this->server->arg("openWeatherMapApiKey");
    weatherSettings->cityId = this->server->arg("city1").toInt();
    weatherSettings->isMetric = this->server->hasArg("metric");
    weatherSettings->lang = this->server->arg("language");
    this->globalDataController->writeSettings();

    this->globalDataController->getDisplayClient()->handleUpdate();
    this->globalDataController->getTimeClient()->resetLastEpoch();
    this->globalDataController->getSystemSettings()->lastOk = FPSTR(OK_MESSAGES_SAVE2);
    this->redirectHome();
}

/**
 * @brief Reset internal configuration data
 */
void WebServer::handleSystemReset() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    this->debugController->printLn("Reset System Configuration");
    if (this->globalDataController->resetConfig()) {
        redirectHome();
        ESP.restart();
    }
}

/**
 * @brief Reset internal WiFi configuration data
 */
void WebServer::handleWifiReset() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    redirectHome();
    WiFiManager wifiManager;
    wifiManager.resetSettings();
    ESP.restart();
}

/**
 * @brief Send firmware/filesystem update page to client
 */
void WebServer::handleUpdatePage() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    WebserverMemoryVariables::sendHeader(this->server, this->globalDataController, "Firmware", "Update");
    WebserverMemoryVariables::sendUpdateForm(this->server, this->globalDataController);
    WebserverMemoryVariables::sendFooter(this->server, this->globalDataController);
}
