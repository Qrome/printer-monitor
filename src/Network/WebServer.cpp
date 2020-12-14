#include "WebServer.h"

static const char WEB_ACTIONS[] PROGMEM =  "<a class='w3-bar-item w3-button' href='/'><i class='fa fa-home'></i> Home</a>"
                      "<a class='w3-bar-item w3-button' href='/configure'><i class='fa fa-cog'></i> Configure</a>"
                      "<a class='w3-bar-item w3-button' href='/configureweather'><i class='fa fa-cloud'></i> Weather</a>"
                      "<a class='w3-bar-item w3-button' href='/systemreset' onclick='return confirm(\"Do you want to reset to default settings?\")'><i class='fa fa-undo'></i> Reset Settings</a>"
                      "<a class='w3-bar-item w3-button' href='/forgetwifi' onclick='return confirm(\"Do you want to forget to WiFi connection?\")'><i class='fa fa-wifi'></i> Forget WiFi</a>"
                      "<a class='w3-bar-item w3-button' href='/update'><i class='fa fa-wrench'></i> Firmware Update</a>"
                      "<a class='w3-bar-item w3-button' href='https://github.com/Qrome' target='_blank'><i class='fa fa-question-circle'></i> About</a>";

String CHANGE_FORM =  ""; // moved to config to make it dynamic

static const char CLOCK_FORM[] PROGMEM = "<hr><p><input name='isClockEnabled' class='w3-check w3-margin-top' type='checkbox' %IS_CLOCK_CHECKED%> Display Clock when printer is off</p>"
                      "<p><input name='is24hour' class='w3-check w3-margin-top' type='checkbox' %IS_24HOUR_CHECKED%> Use 24 Hour Clock (military time)</p>"
                      "<p><input name='invDisp' class='w3-check w3-margin-top' type='checkbox' %IS_INVDISP_CHECKED%> Flip display orientation</p>"
                      "<p><input name='useFlash' class='w3-check w3-margin-top' type='checkbox' %USEFLASH%> Flash System LED on Service Calls</p>"
                      "<p><input name='hasPSU' class='w3-check w3-margin-top' type='checkbox' %HAS_PSU_CHECKED%> Use OctoPrint PSU control plugin for clock/blank</p>"
                      "<p>Clock Sync / Weather Refresh (minutes) <select class='w3-option w3-padding' name='refresh'>%OPTIONS%</select></p>";
                            
static const char THEME_FORM[] PROGMEM =   "<p>Theme Color <select class='w3-option w3-padding' name='theme'>%THEME_OPTIONS%</select></p>"
                      "<p><label>UTC Time Offset</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='utcoffset' value='%UTCOFFSET%' maxlength='12'></p><hr>"
                      "<p><input name='isBasicAuth' class='w3-check w3-margin-top' type='checkbox' %IS_BASICAUTH_CHECKED%> Use Security Credentials for Configuration Changes</p>"
                      "<p><label>User ID (for this interface)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='userid' value='%USERID%' maxlength='20'></p>"
                      "<p><label>Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='stationpassword' value='%STATIONPASSWORD%'></p>"
                      "<button class='w3-button w3-block w3-grey w3-section w3-padding' type='submit'>Save</button></form>";

static const char WEATHER_FORM[] PROGMEM = "<form class='w3-container' action='/updateweatherconfig' method='get'><h2>Weather Config:</h2>"
                      "<p><input name='isWeatherEnabled' class='w3-check w3-margin-top' type='checkbox' %IS_WEATHER_CHECKED%> Display Weather when printer is off</p>"
                      "<label>OpenWeatherMap API Key (get from <a href='https://openweathermap.org/' target='_BLANK'>here</a>)</label>"
                      "<input class='w3-input w3-border w3-margin-bottom' type='text' name='openWeatherMapApiKey' value='%WEATHERKEY%' maxlength='60'>"
                      "<p><label>%CITYNAME1% (<a href='http://openweathermap.org/find' target='_BLANK'><i class='fa fa-search'></i> Search for City ID</a>) "
                      "<input class='w3-input w3-border w3-margin-bottom' type='text' name='city1' value='%CITY1%' onkeypress='return isNumberKey(event)'></p>"
                      "<p><input name='metric' class='w3-check w3-margin-top' type='checkbox' %METRIC%> Use Metric (Celsius)</p>"
                      "<p>Weather Language <select class='w3-option w3-padding' name='language'>%LANGUAGEOPTIONS%</select></p>"
                      "<button class='w3-button w3-block w3-grey w3-section w3-padding' type='submit'>Save</button></form>"
                      "<script>function isNumberKey(e){var h=e.which?e.which:event.keyCode;return!(h>31&&(h<48||h>57))}</script>";

static const char LANG_OPTIONS[] PROGMEM = "<option>ar</option>"
                      "<option>bg</option>"
                      "<option>ca</option>"
                      "<option>cz</option>"
                      "<option>de</option>"
                      "<option>el</option>"
                      "<option>en</option>"
                      "<option>fa</option>"
                      "<option>fi</option>"
                      "<option>fr</option>"
                      "<option>gl</option>"
                      "<option>hr</option>"
                      "<option>hu</option>"
                      "<option>it</option>"
                      "<option>ja</option>"
                      "<option>kr</option>"
                      "<option>la</option>"
                      "<option>lt</option>"
                      "<option>mk</option>"
                      "<option>nl</option>"
                      "<option>pl</option>"
                      "<option>pt</option>"
                      "<option>ro</option>"
                      "<option>ru</option>"
                      "<option>se</option>"
                      "<option>sk</option>"
                      "<option>sl</option>"
                      "<option>es</option>"
                      "<option>tr</option>"
                      "<option>ua</option>"
                      "<option>vi</option>"
                      "<option>zh_cn</option>"
                      "<option>zh_tw</option>";

static const char COLOR_THEMES[] PROGMEM = "<option>red</option>"
                      "<option>pink</option>"
                      "<option>purple</option>"
                      "<option>deep-purple</option>"
                      "<option>indigo</option>"
                      "<option>blue</option>"
                      "<option>light-blue</option>"
                      "<option>cyan</option>"
                      "<option>teal</option>"
                      "<option>green</option>"
                      "<option>light-green</option>"
                      "<option>lime</option>"
                      "<option>khaki</option>"
                      "<option>yellow</option>"
                      "<option>amber</option>"
                      "<option>orange</option>"
                      "<option>deep-orange</option>"
                      "<option>blue-grey</option>"
                      "<option>brown</option>"
                      "<option>grey</option>"
                      "<option>dark-grey</option>"
                      "<option>black</option>"
                      "<option>w3schools</option>";

WebServer::WebServer(GlobalDataController *globalDataController, DebugController *debugController) {
    this->globalDataController = globalDataController;
    this->debugController = debugController;
}

void WebServer::setup() {
    static WebServer* obj = this;

    this->server = new ESP8266WebServer(this->globalDataController->getWebserverPort());
    this->serverUpdater = new ESP8266HTTPUpdateServer();

    this->server->on("/", []() { obj->displayPrinterStatus(); });
    this->server->on("/systemreset", []() { obj->handleSystemReset(); });
    this->server->on("/forgetwifi", []() { obj->handleWifiReset(); });
    this->server->on("/updateconfig", []() { obj->handleUpdateConfig(); });
    this->server->on("/updateweatherconfig", []() { obj->handleUpdateWeather(); });
    this->server->on("/configure", []() { obj->handleConfigure(); });
    this->server->on("/configureweather", []() { obj->handleWeatherConfigure(); });
    this->server->onNotFound([]() { obj->redirectHome(); });
    this->serverUpdater->setup(this->server, "/update", this->globalDataController->getWebserverUsername(), this->globalDataController->getWebserverPassword());

    // Start the server
    this->server->begin();
    this->debugController->printLn("Server started");
}

void WebServer::handleClient() {
    this->server->handleClient();
}

boolean WebServer::authentication() {
    if (this->globalDataController->getWebserverIsBasicAuth() &&
        (this->globalDataController->getWebserverUsername().length() >= 1 && this->globalDataController->getWebserverPassword().length() >= 1)
    ) {
        return this->server->authenticate(
            this->globalDataController->getWebserverUsername().c_str(),
            this->globalDataController->getWebserverPassword().c_str()
        );
    } 
    return true; // Authentication not required
}

void WebServer::redirectHome() {
    // Send them back to the Root Directory
    this->server->sendHeader("Location", String("/"), true);
    this->server->sendHeader("Cache-Control", "no-cache, no-store");
    this->server->sendHeader("Pragma", "no-cache");
    this->server->sendHeader("Expires", "-1");
    this->server->send(302, "text/plain", "");
    this->server->client().stop();
}

void WebServer::displayPrinterStatus() {
    this->globalDataController->ledOnOff(true);
    BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();
    String html = "";

    this->server->sendHeader("Cache-Control", "no-cache, no-store");
    this->server->sendHeader("Pragma", "no-cache");
    this->server->sendHeader("Expires", "-1");
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/html", "");
    this->server->sendContent(String(getHeader(true)));

    String displayTime = 
        this->globalDataController->getTimeClient()->getAmPmHours() + ":" + 
        this->globalDataController->getTimeClient()->getMinutes() + ":" + 
        this->globalDataController->getTimeClient()->getSeconds() + " " + 
        this->globalDataController->getTimeClient()->getAmPm();
    if (this->globalDataController->getClockIs24h()) {
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
        html += "Status: " + printerClient->getState();
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
    
    if (this->globalDataController->getWeatherShow()) {
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
    }

    this->server->sendContent(String(getFooter()));
    this->server->sendContent("");
    this->server->client().stop();
    this->globalDataController->ledOnOff(false);
}

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

void WebServer::handleUpdateConfig() {
    boolean flipOld = this->globalDataController->isDisplayInverted();
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
    this->globalDataController->setUseLedFlash(this->server->hasArg("useFlash"));

    this->globalDataController->writeSettings();
    this->findMDNS();

    this->globalDataController->getPrinterClient()->getPrinterJobResults();
    this->globalDataController->getPrinterClient()->getPrinterPsuState();
    if (this->globalDataController->isDisplayInverted() != flipOld) {
        this->globalDataController->getDisplayClient()->flipDisplayUpdate();
    }
    this->globalDataController->getDisplayClient()->handleUpdate();
    this->globalDataController->getTimeClient()->resetLastEpoch();
    this->redirectHome();
}

void WebServer::findMDNS() {
    if (this->globalDataController->getPrinterHostName() == "" || ENABLE_OTA == false) {
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
    }
}

void WebServer::handleUpdateWeather() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    this->globalDataController->setWeatherShow(this->server->hasArg("isWeatherEnabled"));
    this->globalDataController->setWeatherApiKey(this->server->arg("openWeatherMapApiKey"));
    this->globalDataController->setWeatherCityId(this->server->arg("city1").toInt());
    this->globalDataController->setWeatherIsMetric(this->server->hasArg("metric"));
    this->globalDataController->setWeatherLang(this->server->arg("language"));
    this->globalDataController->writeSettings();

    //isClockOn = false; // this will force a check for the display
    //checkDisplay();
    this->globalDataController->getTimeClient()->resetLastEpoch();
    this->redirectHome();
}

void WebServer::handleConfigure() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    this->globalDataController->ledOnOff(true);
    BasePrinterClient *printerClient = this->globalDataController->getPrinterClient();
    String html = "";

    this->server->sendHeader("Cache-Control", "no-cache, no-store");
    this->server->sendHeader("Pragma", "no-cache");
    this->server->sendHeader("Expires", "-1");
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/html", "");

    html = this->getHeader();
    this->server->sendContent(html);

    // send javascript functions
    if (printerClient->getPrinterType() == "Repetier") {
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
    } 
    else if (printerClient->getPrinterType() == "Klipper") {
        html = "<script>function testKlipper(){var e=document.getElementById(\"KlipperTest\"),t=document.getElementById(\"PrinterAddress\").value,"
            "n=document.getElementById(\"PrinterPort\").value;if(e.innerHTML=\"\",\"\"==t||\"\"==n)return e.innerHTML=\"* Address and Port are required\","
            "void(e.style.background=\"\");var r=\"http://\"+t+\":\"+n;r+=\"/printer/info\",window.open(r,\"_blank\").focus()}</script>";
        this->server->sendContent(html);
    }    
    else {
        html = "<script>function testOctoPrint(){var e=document.getElementById(\"OctoPrintTest\"),t=document.getElementById(\"PrinterAddress\").value,"
            "n=document.getElementById(\"PrinterPort\").value;if(e.innerHTML=\"\",\"\"==t||\"\"==n)return e.innerHTML=\"* Address and Port are required\","
            "void(e.style.background=\"\");var r=\"http://\"+t+\":\"+n;r+=\"/api/job?apikey=\"+document.getElementById(\"PrinterApiKey\").value,window.open(r,\"_blank\").focus()}</script>";
        this->server->sendContent(html);
    }


    String form =       "<form class='w3-container' action='/updateconfig' method='get'><h2>Station Config:</h2>";
    
    
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

    form = FPSTR(CLOCK_FORM);
    
    String isClockChecked = "";
    if (DISPLAYCLOCK) {
        isClockChecked = "checked='checked'";
    }
    form.replace("%IS_CLOCK_CHECKED%", isClockChecked);
    String is24hourChecked = "";
    if (this->globalDataController->getClockIs24h()) {
        is24hourChecked = "checked='checked'";
    }
    form.replace("%IS_24HOUR_CHECKED%", is24hourChecked);
    String isInvDisp = "";
    if (this->globalDataController->isDisplayInverted()) {
        isInvDisp = "checked='checked'";
    }
    form.replace("%IS_INVDISP_CHECKED%", isInvDisp);
    String isFlashLED = "";
    if (USE_FLASH) {
        isFlashLED = "checked='checked'";
    }
    form.replace("%USEFLASH%", isFlashLED);
    String hasPSUchecked = "";
    if (this->globalDataController->hasPrinterPsu()) {
        hasPSUchecked = "checked='checked'";
    }
    form.replace("%HAS_PSU_CHECKED%", hasPSUchecked);
    
    String options = "<option>10</option><option>15</option><option>20</option><option>30</option><option>60</option>";
    options.replace(">"+String(this->globalDataController->getClockResyncMinutes())+"<", " selected>"+String(this->globalDataController->getClockResyncMinutes())+"<");
    form.replace("%OPTIONS%", options);

    this->server->sendContent(form);

    form = FPSTR(THEME_FORM);
    
    String themeOptions = FPSTR(COLOR_THEMES);
    themeOptions.replace(">"+String(this->globalDataController->getWebserverTheme())+"<", " selected>"+String(this->globalDataController->getWebserverTheme())+"<");
    form.replace("%THEME_OPTIONS%", themeOptions);
    form.replace("%UTCOFFSET%", String(this->globalDataController->getClockUtcOffset()));
    String isUseSecurityChecked = "";
    if (this->globalDataController->getWebserverIsBasicAuth()) {
        isUseSecurityChecked = "checked='checked'";
    }
    form.replace("%IS_BASICAUTH_CHECKED%", isUseSecurityChecked);
    form.replace("%USERID%", String(this->globalDataController->getWebserverUsername()));
    form.replace("%STATIONPASSWORD%", String(this->globalDataController->getWebserverPassword()));

    this->server->sendContent(form);
    
    html = this->getFooter();
    this->server->sendContent(html);
    this->server->sendContent("");
    this->server->client().stop();
    this->globalDataController->ledOnOff(false);
}

void WebServer::handleWeatherConfigure() {
    if (!this->authentication()) {
        return this->server->requestAuthentication();
    }
    this->globalDataController->ledOnOff(true);
    String html = "";

    this->server->sendHeader("Cache-Control", "no-cache, no-store");
    this->server->sendHeader("Pragma", "no-cache");
    this->server->sendHeader("Expires", "-1");
    this->server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    this->server->send(200, "text/html", "");

    html = getHeader();
    this->server->sendContent(html);
    
    String form = FPSTR(WEATHER_FORM);
    String isWeatherChecked = "";
    if (DISPLAYWEATHER) {
        isWeatherChecked = "checked='checked'";
    }
    form.replace("%IS_WEATHER_CHECKED%", this->globalDataController->getWeatherShow() ? "1" : "0");
    form.replace("%WEATHERKEY%", this->globalDataController->getWeatherApiKey());
    form.replace("%CITYNAME1%", this->globalDataController->getWeatherClient()->getCity(0));
    form.replace("%CITY1%", String(this->globalDataController->getWeatherCityId()));
    String checked = "";
    if (this->globalDataController->getWeatherIsMetric()) {
        checked = "checked='checked'";
    }
    form.replace("%METRIC%", checked);
    String options = FPSTR(LANG_OPTIONS);
    options.replace(">"+String(this->globalDataController->getWeatherLang())+"<", " selected>"+String(this->globalDataController->getWeatherLang())+"<");
    form.replace("%LANGUAGEOPTIONS%", options);
    this->server->sendContent(form);
    
    html = this->getFooter();
    this->server->sendContent(html);
    this->server->sendContent("");
    this->server->client().stop();
    this->globalDataController->ledOnOff(false);
}

String WebServer::getHeader() {
  return this->getHeader(false);
}

String WebServer::getHeader(boolean refresh) {
    String menu = FPSTR(WEB_ACTIONS);

    String html = "<!DOCTYPE HTML>";
    html += "<html><head><title>Printer Monitor</title><link rel='icon' href='data:;base64,='>";
    html += "<meta charset='UTF-8'>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    if (refresh) {
        html += "<meta http-equiv=\"refresh\" content=\"30\">";
    }
    html += "<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>";
    html += "<link rel='stylesheet' href='https://www.w3schools.com/lib/w3-theme-" + this->globalDataController->getWebserverTheme() + ".css'>";
    html += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>";
    html += "</head><body>";
    html += "<nav class='w3-sidebar w3-bar-block w3-card' style='margin-top:88px' id='mySidebar'>";
    html += "<div class='w3-container w3-theme-d2'>";
    html += "<span onclick='closeSidebar()' class='w3-button w3-display-topright w3-large'><i class='fa fa-times'></i></span>";
    html += "<div class='w3-cell w3-left w3-xxxlarge' style='width:60px'><i class='fa fa-cube'></i></div>";
    html += "<div class='w3-padding'>Menu</div></div>";
    html += menu;
    html += "</nav>";
    html += "<header class='w3-top w3-bar w3-theme'><button class='w3-bar-item w3-button w3-xxxlarge w3-hover-theme' onclick='openSidebar()'><i class='fa fa-bars'></i></button><h2 class='w3-bar-item'>Printer Monitor</h2></header>";
    html += "<script>";
    html += "function openSidebar(){document.getElementById('mySidebar').style.display='block'}function closeSidebar(){document.getElementById('mySidebar').style.display='none'}closeSidebar();";
    html += "</script>";
    html += "<br><div class='w3-container w3-large' style='margin-top:88px'>";
    return html;
}

String WebServer::getFooter() {
  int8_t rssi = this->globalDataController->getWifiQuality();
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(rssi);
  this->debugController->printLn("%");
  String html = "<br><br><br>";
  html += "</div>";
  html += "<footer class='w3-container w3-bottom w3-theme w3-margin-top'>";
  if (this->globalDataController->getLastReportStatus() != "") {
    html += "<i class='fa fa-external-link'></i> Report Status: " + this->globalDataController->getLastReportStatus() + "<br>";
  }
  html += "<i class='fa fa-paper-plane-o'></i> Version: " + String(VERSION) + "<br>";
  html += "<i class='fa fa-rss'></i> Signal Strength: ";
  html += String(rssi) + "%";
  html += "</footer>";
  html += "</body></html>";
  return html;
}