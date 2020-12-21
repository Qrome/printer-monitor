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

    this->server->on("/", []() { obj->displayPrinterStatus(); });
    this->server->on("/systemreset", []() { obj->handleSystemReset(); });
    this->server->on("/forgetwifi", []() { obj->handleWifiReset(); });
    this->server->on("/configurestation/show", []() { obj->handleConfigureStation(); });
    this->server->on("/configurestation/update", []() { obj->handleUpdateStation(); });
    this->server->on("/configureprinter/show", []() { obj->handleConfigurePrinter(); });
    this->server->on("/configureprinter/edit", []() { obj->handleUpdatePrinter(); });
    this->server->on("/configureprinter/delete", []() { obj->handleConfigurePrinter(); });
    this->server->on("/configureweather/show", []() { obj->handleConfigureWeather(); });
    this->server->on("/configureweather/update", []() { obj->handleUpdateWeather(); });
    this->server->on("/configuresensor/show", []() { obj->handleConfigureSensor(); });
    this->server->on("/configuresensor/update", []() { obj->handleConfigureSensor(); });
    this->server->on("/update", HTTP_GET, []() { obj->handleUpdatePage(); });

    this->server->on("/updateconfig", []() { obj->handleUpdateConfig(); });

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

void WebServer::handleClient() {
    this->server->handleClient();
}

void WebServer::redirectTarget(String targetUri) {
    // Send them back to the Root Directory
    this->server->sendHeader("Location", targetUri, true);
    this->server->sendHeader("Cache-Control", "no-cache, no-store");
    this->server->sendHeader("Pragma", "no-cache");
    this->server->sendHeader("Expires", "-1");
    this->server->send(302, "text/plain", "");
    this->server->client().stop();
}


void WebServer::redirectHome() {
    this->redirectTarget("/");
}

void WebServer::displayPrinterStatus() {
    WebserverMemoryVariables::sendHeader(this->server, this->globalDataController, "Status", "Monitor", true);

    /*BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();
    String html = "";

    

    String displayTime = 
        this->globalDataController->getTimeClient()->getAmPmHours() + ":" + 
        this->globalDataController->getTimeClient()->getMinutes() + ":" + 
        this->globalDataController->getTimeClient()->getSeconds() + " " + 
        this->globalDataController->getTimeClient()->getAmPm();
    if (this->globalDataController->getClockSettings()->is24h) {
        displayTime =
            this->globalDataController->getTimeClient()->getHours() + ":" +
            this->globalDataController->getTimeClient()->getMinutes() + ":" +
            this->globalDataController->getTimeClient()->getSeconds();
    }
    
    html += "<div class='w3-cell-row' style='width:100%'><h2>" + printerClient->getPrinterType() + " Monitor</h2></div><div class='w3-cell-row'>";
    html += "<div class='w3-cell w3-container' style='width:100%'><p>";
    if ((printerClient->getPrinterType() == "Repetier") || (printerClient->getPrinterType() == "Klipper")) {
        html += "Printer Name: " + printerClient->getPrinterName() + " <a href='/configure' title='Configure'><i class='fa fa-cog'></i></a><br>";
    }
    else {
        html += "Host Name: " + this->globalDataController->getPrinterHostName() + " <a href='/configure' title='Configure'><i class='fa fa-cog'></i></a><br>";
    }
    
    if (printerClient->getError() != "") {
        html += "Status: Offline<br>";
        html += "Reason: " + printerClient->getError() + "<br>";
    } else {
        html += "Status: " + printerClient->getStateAsText();
        if (printerClient->isPSUoff() && this->globalDataController->hasPrinterPsu()) {  
        html += ", PSU off";
        }
        html += "<br>";
    }
    
    if (printerClient->isPrinting()) {
        html += "File: " + printerClient->getFileName() + "<br>";
        float fileSize = printerClient->getFileSize().toFloat();
        if (fileSize > 0) {
        fileSize = fileSize / 1024;
        html += "File Size: " + String(fileSize) + "KB<br>";
        }
        int filamentLength = printerClient->getFilamentLength().toInt();
        if (filamentLength > 0) {
        float fLength = float(filamentLength) / 1000;
        html += "Filament: " + String(fLength) + "m<br>";
        }
    
        html += "Tool Temperature: " + printerClient->getTempToolActual() + "&#176; C<br>";
        if (printerClient->getTempBedActual() != 0 ) {
            html += "Bed Temperature: " + printerClient->getTempBedActual() + "&#176; C<br>";
        }
        
        int val = printerClient->getProgressPrintTimeLeft().toInt();
        int hours = this->globalDataController->numberOfHours(val);
        int minutes = this->globalDataController->numberOfMinutes(val);
        int seconds = this->globalDataController->numberOfSeconds(val);
        html += "Est. Print Time Left: "
                + this->globalDataController->zeroPad(hours) + ":"
                + this->globalDataController->zeroPad(minutes) + ":"
                + this->globalDataController->zeroPad(seconds) + "<br>";
    
        val = printerClient->getProgressPrintTime().toInt();
        hours = this->globalDataController->numberOfHours(val);
        minutes = this->globalDataController->numberOfMinutes(val);
        seconds = this->globalDataController->numberOfSeconds(val);
        html += "Printing Time: " + this->globalDataController->zeroPad(hours) + ":" + this->globalDataController->zeroPad(minutes) + ":" + this->globalDataController->zeroPad(seconds) + "<br>";
        html += "<style>#myProgress {width: 100%;background-color: #ddd;}#myBar {width: " + printerClient->getProgressCompletion() + "%;height: 30px;background-color: #4CAF50;}</style>";
        html += "<div id=\"myProgress\"><div id=\"myBar\" class=\"w3-medium w3-center\">" + printerClient->getProgressCompletion() + "%</div></div>";
    } else {
        html += "<hr>";
    }

    html += "</p></div></div>";

    html += "<div class='w3-cell-row' style='width:100%'><h2>Time: " + displayTime + "</h2></div>";

    this->server->sendContent(html); // spit out what we got
    html = "";
    
    if (this->globalDataController->getWeatherSettings()->show) {
        OpenWeatherMapClient *weatherClient = this->globalDataController->getWeatherClient();
        if (weatherClient->getCity(0) == "") {
            html += "<p>Please <a href='/configureweather'>Configure Weather</a> API</p>";
            if (weatherClient->getError() != "") {
                html += "<p>Weather Error: <strong>" + weatherClient->getError() + "</strong></p>";
            }
        } else {
            html += "<div class='w3-cell-row' style='width:100%'><h2>" + weatherClient->getCity(0) + ", " + weatherClient->getCountry(0) + "</h2></div><div class='w3-cell-row'>";
            html += "<div class='w3-cell w3-left w3-medium' style='width:120px'>";
            html += "<img src='http://openweathermap.org/img/w/" + weatherClient->getIcon(0) + ".png' alt='" + weatherClient->getDescription(0) + "'><br>";
            html += weatherClient->getHumidity(0) + "% Humidity<br>";
            html += weatherClient->getWind(0) + " <span class='w3-tiny'>" + weatherClient->getSpeedSymbol() + "</span> Wind<br>";
            html += "</div>";
            html += "<div class='w3-cell w3-container' style='width:100%'><p>";
            html += weatherClient->getCondition(0) + " (" + weatherClient->getDescription(0) + ")<br>";
            html += weatherClient->getTempRounded(0) + weatherClient->getTempSymbol(true) + "<br>";
            html += "<a href='https://www.google.com/maps/@" + weatherClient->getLat(0) + "," + weatherClient->getLon(0) + ",10000m/data=!3m1!1e3' target='_BLANK'><i class='fa fa-map-marker' style='color:red'></i> Map It!</a><br>";
            html += "</p></div></div>";
        }
        
        this->server->sendContent(html); // spit out what we got
        html = ""; // fresh start
    }*/

    WebserverMemoryVariables::sendFooter(this->server, this->globalDataController);
}



void WebServer::handleUpdateConfig() {
    /*boolean flipOld = this->globalDataController->isDisplayInverted();
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    if (this->server->hasArg("printer")) {
        this->globalDataController->getPrinterClient()->setPrinterName(
            this->server->arg("printer")
        );
    }
    
    this->globalDataController->setPrinterApiKey(this->server->arg("PrinterApiKey"));
    this->globalDataController->setPrinterHostName(this->server->arg("PrinterHostName"));
    this->globalDataController->setPrinterServer(this->server->arg("PrinterAddress"));
    this->globalDataController->setPrinterPort(this->server->arg("PrinterPort").toInt());
    this->globalDataController->setPrinterAuthUser(this->server->arg("octoUser"));
    this->globalDataController->setPrinterAuthPass(this->server->arg("octoPass"));
    this->globalDataController->setHasPrinterPsu(this->server->hasArg("hasPSU"));
    this->globalDataController->setDisplayClock(this->server->hasArg("isClockEnabled"));
    this->globalDataController->setIsDisplayInverted(this->server->hasArg("invDisp"));
    this->globalDataController->setClockIs24h(this->server->hasArg("is24hour"));
    this->globalDataController->setClockResyncMinutes(this->server->arg("refresh").toInt());
    this->globalDataController->setWebserverTheme(this->server->arg("theme"));
    this->globalDataController->setClockUtcOffset(this->server->arg("utcoffset").toFloat());
    String temp = this->server->arg("userid");
    this->globalDataController->setWebserverUsername(temp);
    temp = this->server->arg("stationpassword");
    this->globalDataController->setWebserverPassword(temp);
    //this->globalDataController->setUseLedFlash(this->server->hasArg("useFlash"));

    this->globalDataController->writeSettings();
    this->findMDNS();

    this->globalDataController->getPrinterClient()->getPrinterJobResults();
    this->globalDataController->getPrinterClient()->getPrinterPsuState();
    if (this->globalDataController->isDisplayInverted() != flipOld) {
        this->globalDataController->getDisplayClient()->flipDisplayUpdate();
    }
    this->globalDataController->getDisplayClient()->handleUpdate();
    this->globalDataController->getTimeClient()->resetLastEpoch();*/
    this->redirectHome();
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

/*
void WebServer::handleConfigure() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();
    String html = "";
    
    WebserverMemoryVariables::sendHeader(this->server, this->globalDataController, "Configure", "Station");


    // send javascript functions for repetier server test
    html = "<script>function testRepetier(){var e=document.getElementById(\"RepetierTest\"),r=document.getElementById(\"PrinterAddress\").value,"
        "t=document.getElementById(\"PrinterPort\").value;if(\"\"==r||\"\"==t)return e.innerHTML=\"* Address and Port are required\","
        "void(e.style.background=\"\");var n=\"http://\"+r+\":\"+t;n+=\"/printer/api/?a=listPrinter&apikey=\"+document.getElementById(\"PrinterApiKey\").value,"
        "console.log(n);var o=new XMLHttpRequest;o.open(\"GET\",n,!0),o.onload=function(){if(200===o.status){var r=JSON.parse(o.responseText);"
        "if(!r.error&&r.length>0){var t=\"<label>Connected -- Select Printer</label> \";t+=\"<select class='w3-option w3-padding' name='printer'>\";"
        "var n=document.getElementById(\"selectedPrinter\").value,i=\"\";for(printer in r)i=r[printer].slug==n?\"selected\":\"\","
        "t+=\"<option value='\"+r[printer].slug+\"' \"+i+\">\"+r[printer].name+\"</option>\";t+=\"</select>\","
        "e.innerHTML=t,e.style.background=\"lime\"}else e.innerHTML=\"Error invalid API Key: \"+r.error,"
        "e.style.background=\"red\"}else e.innerHTML=\"Error: \"+o.statusText,e.style.background=\"red\"},"
        "o.onerror=function(){e.innerHTML=\"Error connecting to server -- check IP and Port\",e.style.background=\"red\"},o.send(null)}</script>";
    this->server->sendContent(html);

    // send javascript functions for klipper test
    html = "<script>function testKlipper(){var e=document.getElementById(\"KlipperTest\"),t=document.getElementById(\"PrinterAddress\").value,"
        "n=document.getElementById(\"PrinterPort\").value;if(e.innerHTML=\"\",\"\"==t||\"\"==n)return e.innerHTML=\"* Address and Port are required\","
        "void(e.style.background=\"\");var r=\"http://\"+t+\":\"+n;r+=\"/printer/info\",window.open(r,\"_blank\").focus()}</script>";
    this->server->sendContent(html);
    
    // send javascript functions for octoprint test
    html = "<script>function testOctoPrint(){var e=document.getElementById(\"OctoPrintTest\"),t=document.getElementById(\"PrinterAddress\").value,"
        "n=document.getElementById(\"PrinterPort\").value;if(e.innerHTML=\"\",\"\"==t||\"\"==n)return e.innerHTML=\"* Address and Port are required\","
        "void(e.style.background=\"\");var r=\"http://\"+t+\":\"+n;r+=\"/api/job?apikey=\"+document.getElementById(\"PrinterApiKey\").value,window.open(r,\"_blank\").focus()}</script>";
    this->server->sendContent(html);
    
    // Let us create a form for all printers
    html  = "<form class='w3-container' action='/updateconfig' method='get'><h2>Station Config:</h2>";
    html += "<table id='printerData' class='table table-striped table-condensed'></table><script>var target ='#printerData'; var customFields = { data: {";
    this->server->sendContent(html);
    
    for (int i=0; i<10; i++) {
        html = "";
        if (i > 0) {
            html = ",";
        }
        html += String(i + 1) + ": { enabled:true, fieldName:'COMPANYCODE',prettyName:'Company Name', fieldType:'OEM', fieldValue:'' }";
        this->server->sendContent(html);
    }
    html  = "},";
    html += "xref: { // Key ['Label','column width',0|1] 1=admin only";
    html += "  custom: ['#','10%',0],";
    html += "  actions: ['Actions','5%',1],";
    html += "  enabled: ['Enabled','5%',0],";
    html += "  fieldName: ['Field','20%',0],";
    html += "  prettyName: ['Pretty Name','20%',0],";
    html += "  fieldType: ['Type','30%',0],";
    html += "  fieldValue: ['Value','10%',0]";
    html += "},";
    html += "fieldTypes: [";
    html += "  [' ',' '],";
    html += "  ['ENV','Environment (ENV)'],";
    html += "  ['REG','Registry (REG)'],";
    html += "  ['WMI','Windows Management Inst. (WMI)'],";
    html += "  ['OEM','Original Equipment Manuf.(OEM)']";
    html += "],";
    html += "admin:true,";
    html += "multiedit:false";
    html += "}";
    html += "</script>";
    this->server->sendContent(html);


    
    String form = "";
    if (printerClient->getPrinterType() != "Klipper") {
        form    +=      "<p><label>" + printerClient->getPrinterType() + " API Key (get from your server)</label>"
                        "<input class='w3-input w3-border w3-margin-bottom' type='text' name='PrinterApiKey' id='PrinterApiKey' value='%PRINTERAPIKEY%' maxlength='60'></p>";
    }
    if ((printerClient->getPrinterType() == "OctoPrint") || (printerClient->getPrinterType() == "Klipper")) {
        form    +=      "<p><label>" + printerClient->getPrinterType() + " Host Name</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='PrinterHostName' value='%PRINTERHOST%' maxlength='60'></p>";                        
    }
    form        +=      "<p><label>" + printerClient->getPrinterType() + " Address (do not include http://)</label>"
                        "<input class='w3-input w3-border w3-margin-bottom' type='text' name='PrinterAddress' id='PrinterAddress' value='%PRINTERADDRESS%' maxlength='60'></p>"
                        "<p><label>" + printerClient->getPrinterType() + " Port</label>"
                        "<input class='w3-input w3-border w3-margin-bottom' type='text' name='PrinterPort' id='PrinterPort' value='%PRINTERPORT%' maxlength='5'  onkeypress='return isNumberKey(event)'></p>";
    if ((printerClient->getPrinterType() == "Repetier") || (printerClient->getPrinterType() == "Klipper")) {
        form    +=     "<input type='button' value='Test Connection' onclick='test" + printerClient->getPrinterName() + "()'>"
                       "<input type='hidden' id='selectedPrinter' value='" + printerClient->getPrinterName() + "'><p id='" + printerClient->getPrinterName() + "Test'></p>"
                       "<script>test" + printerClient->getPrinterName() + "();</script>";                        
    } 
    else {
        form    +=     "<input type='button' value='Test Connection and API JSON Response' onclick='testOctoPrint()'><p id='OctoPrintTest'></p>";
    }
    form        +=     "<p><label>" + printerClient->getPrinterType() + " User (only needed if you have haproxy or basic auth turned on)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='octoUser' value='%PRINTERUSER%' maxlength='30'></p>"
                       "<p><label>" + printerClient->getPrinterType() + " Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='octoPass' value='%PRINTERPASS%'></p>";

    form.replace("%PRINTERAPIKEY%", this->globalDataController->getPrinterApiKey());
    form.replace("%PRINTERHOST%", this->globalDataController->getPrinterHostName());
    form.replace("%PRINTERADDRESS%", this->globalDataController->getPrinterServer());
    form.replace("%PRINTERPORT%", String(this->globalDataController->getPrinterPort()));
    form.replace("%PRINTERUSER%", this->globalDataController->getPrinterAuthUser());
    form.replace("%PRINTERPASS%", this->globalDataController->getPrinterAuthPass());
    this->server->sendContent(form);

    
    this->server->sendContent(form);
    
    WebserverMemoryVariables::sendFooter(this->server, this->globalDataController);
}*/














void WebServer::handleConfigureSensor() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    WebserverMemoryVariables::sendHeader(this->server, this->globalDataController, "Configure", "Sensor");
    
    WebserverMemoryVariables::sendFooter(this->server, this->globalDataController);
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

    // Set data
    MemoryHelper::stringToChar(this->server->arg("e-tname"), targetPrinter->customName, 20);
    targetPrinter->apiType = this->server->arg("e-tapi").toInt();
    MemoryHelper::stringToChar("", targetPrinter->apiKey, 60);
    MemoryHelper::stringToChar(this->server->arg("e-taddr"), targetPrinter->remoteAddress, 60);
    targetPrinter->remotePort = this->server->arg("e-tport").toInt();
    targetPrinter->hasPsuControl = this->server->hasArg("e-tpsu");
    targetPrinter->basicAuthNeeded = this->server->hasArg("e-tapipw");
    MemoryHelper::stringToChar(this->server->arg("e-tapiuser"), targetPrinter->basicAuthUsername, 30);
    MemoryHelper::stringToChar(this->server->arg("e-tapipass"), targetPrinter->basicAuthPassword, 60);
    this->globalDataController->getSystemSettings()->lastOk = FPSTR(OK_MESSAGES_SAVE1);

    //http://192.168.0.239/configureprinter/show?id=0&e-tname=asdasd&e-tapi=Klipper&e-taddr=123.213.123.121&e-tport=80&e-tapipw=on&e-tapiuser=admin&e-tapipass=admin
    

    this->globalDataController->writeSettings();
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
