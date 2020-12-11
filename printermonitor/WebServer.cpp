#include "WebServer.h"

static const char WEB_ACTIONS[] PROGMEM =  "<a class='w3-bar-item w3-button' href='/'><i class='fa fa-home'></i> Home</a>"
                      "<a class='w3-bar-item w3-button' href='/configure'><i class='fa fa-cog'></i> Configure</a>"
                      "<a class='w3-bar-item w3-button' href='/configureweather'><i class='fa fa-cloud'></i> Weather</a>"
                      "<a class='w3-bar-item w3-button' href='/systemreset' onclick='return confirm(\"Do you want to reset to default settings?\")'><i class='fa fa-undo'></i> Reset Settings</a>"
                      "<a class='w3-bar-item w3-button' href='/forgetwifi' onclick='return confirm(\"Do you want to forget to WiFi connection?\")'><i class='fa fa-wifi'></i> Forget WiFi</a>"
                      "<a class='w3-bar-item w3-button' href='/update'><i class='fa fa-wrench'></i> Firmware Update</a>"
                      "<a class='w3-bar-item w3-button' href='https://github.com/Qrome' target='_blank'><i class='fa fa-question-circle'></i> About</a>";

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

WebServer::WebServer(ESP8266WebServer *serverHandle, Debug *debugHandle, PrinterClientInterface *printerClient, TimeClient *timeClient, String themeColor, String swVersion, boolean hasPsu) {
  this->serverHandle = serverHandle;
  this->themeColor = themeColor;
  this->debugHandle = debugHandle;
  this->swVersion = swVersion;
  this->printerClient = printerClient;
  this->timeClient = timeClient;
  this->hostName = "";
  this->hasPsu = hasPsu;
  this->isBasicAuth = false;
  this->wwwUsername = "";
  this->wwwPassword = "";
}

void WebServer::setHostname(String hostName) {
  this->hostName = hostName;
}

void WebServer::redirectHome() {
  // Send them back to the Root Directory
  this->serverHandle->sendHeader("Location", String("/"), true);
  this->serverHandle->sendHeader("Cache-Control", "no-cache, no-store");
  this->serverHandle->sendHeader("Pragma", "no-cache");
  this->serverHandle->sendHeader("Expires", "-1");
  this->serverHandle->send(302, "text/plain", "");
  this->serverHandle->client().stop();
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
  html += "<link rel='stylesheet' href='https://www.w3schools.com/lib/w3-theme-" + this->themeColor + ".css'>";
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
  int8_t rssi = this->getWifiQuality();
  this->debugHandle->print("Signal Strength (RSSI): ");
  this->debugHandle->print(rssi);
  this->debugHandle->printLn("%");
  String html = "<br><br><br>";
  html += "</div>";
  html += "<footer class='w3-container w3-bottom w3-theme w3-margin-top'>";
  if (this->lastReportStatus != "") {
    html += "<i class='fa fa-external-link'></i> Report Status: " + this->lastReportStatus + "<br>";
  }
  html += "<i class='fa fa-paper-plane-o'></i> Version: " + this->swVersion + "<br>";
  html += "<i class='fa fa-rss'></i> Signal Strength: ";
  html += String(rssi) + "%";
  html += "</footer>";
  html += "</body></html>";
  return html;
}

void WebServer::displayPrinterStatus() {
  String html = "";

  this->serverHandle->sendHeader("Cache-Control", "no-cache, no-store");
  this->serverHandle->sendHeader("Pragma", "no-cache");
  this->serverHandle->sendHeader("Expires", "-1");
  this->serverHandle->setContentLength(CONTENT_LENGTH_UNKNOWN);
  this->serverHandle->send(200, "text/html", "");
  this->serverHandle->sendContent(String(this->getHeader(true)));

  String displayTime = this->timeClient->getAmPmHours() + ":" + this->timeClient->getMinutes() + ":" + this->timeClient->getSeconds() + " " + this->timeClient->getAmPm();
  if (this->timeClient->isHour24()) {
    displayTime = this->timeClient->getHours() + ":" + this->timeClient->getMinutes() + ":" + this->timeClient->getSeconds();
  }
  
  html += "<div class='w3-cell-row' style='width:100%'><h2>" + this->printerClient->getPrinterType() + " Monitor</h2></div><div class='w3-cell-row'>";
  html += "<div class='w3-cell w3-container' style='width:100%'><p>";
  if (this->printerClient->getPrinterType() == "Repetier") {
    html += "Printer Name: " + this->printerClient->getPrinterName() + " <a href='/configure' title='Configure'><i class='fa fa-cog'></i></a><br>";
  } else {
    html += "Host Name: " + this->hostName + " <a href='/configure' title='Configure'><i class='fa fa-cog'></i></a><br>";
  }
 
  if (this->printerClient->getError() != "") {
    html += "Status: Offline<br>";
    html += "Reason: " + this->printerClient->getError() + "<br>";
  } else {
    html += "Status: " + this->printerClient->getState();
    if (this->printerClient->isPSUoff() && this->hasPsu) {  
      html += ", PSU off";
    }
    html += "<br>";
  }
  
  if (this->printerClient->isPrinting()) {
    html += "File: " + this->printerClient->getFileName() + "<br>";
    float fileSize = this->printerClient->getFileSize().toFloat();
    if (fileSize > 0) {
      fileSize = fileSize / 1024;
      html += "File Size: " + String(fileSize) + "KB<br>";
    }
    int filamentLength = this->printerClient->getFilamentLength().toInt();
    if (filamentLength > 0) {
      float fLength = float(filamentLength) / 1000;
      html += "Filament: " + String(fLength) + "m<br>";
    }
  
    html += "Tool Temperature: " + this->printerClient->getTempToolActual() + "&#176; C<br>";
    if ( this->printerClient->getTempBedActual() != 0 ) {
        html += "Bed Temperature: " + this->printerClient->getTempBedActual() + "&#176; C<br>";
    }
    
    int val = this->printerClient->getProgressPrintTimeLeft().toInt();
    int hours = numberOfHours(val);
    int minutes = numberOfMinutes(val);
    int seconds = numberOfSeconds(val);
    html += "Est. Print Time Left: " + zeroPad(hours) + ":" + zeroPad(minutes) + ":" + zeroPad(seconds) + "<br>";
  
    val = this->printerClient->getProgressPrintTime().toInt();
    hours = numberOfHours(val);
    minutes = numberOfMinutes(val);
    seconds = numberOfSeconds(val);
    html += "Printing Time: " + zeroPad(hours) + ":" + zeroPad(minutes) + ":" + zeroPad(seconds) + "<br>";
    html += "<style>#myProgress {width: 100%;background-color: #ddd;}#myBar {width: " + this->printerClient->getProgressCompletion() + "%;height: 30px;background-color: #4CAF50;}</style>";
    html += "<div id=\"myProgress\"><div id=\"myBar\" class=\"w3-medium w3-center\">" + this->printerClient->getProgressCompletion() + "%</div></div>";
  } else {
    html += "<hr>";
  }

  html += "</p></div></div>";

  html += "<div class='w3-cell-row' style='width:100%'><h2>Time: " + displayTime + "</h2></div>";

  this->serverHandle->sendContent(html); // spit out what we got
  html = "";

  /*
  if (DISPLAYWEATHER) {
    if (weatherClient.getCity(0) == "") {
      html += "<p>Please <a href='/configureweather'>Configure Weather</a> API</p>";
      if (weatherClient.getError() != "") {
        html += "<p>Weather Error: <strong>" + weatherClient.getError() + "</strong></p>";
      }
    } else {
      html += "<div class='w3-cell-row' style='width:100%'><h2>" + weatherClient.getCity(0) + ", " + weatherClient.getCountry(0) + "</h2></div><div class='w3-cell-row'>";
      html += "<div class='w3-cell w3-left w3-medium' style='width:120px'>";
      html += "<img src='http://openweathermap.org/img/w/" + weatherClient.getIcon(0) + ".png' alt='" + weatherClient.getDescription(0) + "'><br>";
      html += weatherClient.getHumidity(0) + "% Humidity<br>";
      html += weatherClient.getWind(0) + " <span class='w3-tiny'>" + getSpeedSymbol() + "</span> Wind<br>";
      html += "</div>";
      html += "<div class='w3-cell w3-container' style='width:100%'><p>";
      html += weatherClient.getCondition(0) + " (" + weatherClient.getDescription(0) + ")<br>";
      html += weatherClient.getTempRounded(0) + getTempSymbol(true) + "<br>";
      html += "<a href='https://www.google.com/maps/@" + weatherClient.getLat(0) + "," + weatherClient.getLon(0) + ",10000m/data=!3m1!1e3' target='_BLANK'><i class='fa fa-map-marker' style='color:red'></i> Map It!</a><br>";
      html += "</p></div></div>";
    }
    
    this->serverHandle->sendContent(html); // spit out what we got
    html = ""; // fresh start
  }*/

  this->serverHandle->sendContent(String(getFooter()));
  this->serverHandle->sendContent("");
  this->serverHandle->client().stop();
}


void WebServer::displayMessage(String message) {
  this->serverHandle->sendHeader("Cache-Control", "no-cache, no-store");
  this->serverHandle->sendHeader("Pragma", "no-cache");
  this->serverHandle->sendHeader("Expires", "-1");
  this->serverHandle->setContentLength(CONTENT_LENGTH_UNKNOWN);
  this->serverHandle->send(200, "text/html", "");
  String html = this->getHeader();
  this->serverHandle->sendContent(String(html));
  this->serverHandle->sendContent(String(message));
  html = this->getFooter();
  this->serverHandle->sendContent(String(html));
  this->serverHandle->sendContent("");
  this->serverHandle->client().stop();
}

void WebServer::setAuthentication(boolean enableBasicAuth, char *username, char *password) {
  this->isBasicAuth = enableBasicAuth;
  this->wwwUsername = username;
  this->wwwPassword = password;
}

boolean WebServer::authentication() {
  if (this->isBasicAuth && (strlen(this->wwwUsername) >= 1 && strlen(this->wwwPassword) >= 1)) {
    return this->serverHandle->authenticate(this->wwwUsername, this->wwwPassword);
  } 
  return true; // Authentication not required
}

void WebServer::handleWifiReset() {
  if (!this->authentication()) {
    return this->serverHandle->requestAuthentication();
  }
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  this->redirectHome();
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.restart();
}

void WebServer::handleSystemReset() {
  if (!this->authentication()) {
    return this->serverHandle->requestAuthentication();
  }
  this->debugHandle->printLn("Reset System Configuration");
  //if (SPIFFS.remove(CONFIG)) {
    this->redirectHome();
    ESP.restart();
  //}
}

int8_t WebServer::getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if(dbm <= -100) {
      return 0;
  } else if(dbm >= -50) {
      return 100;
  } else {
      return 2 * (dbm + 100);
  }
}

String WebServer::zeroPad(int value) {
  String rtnValue = String(value);
  if (value < 10) {
    rtnValue = "0" + rtnValue;
  }
  return rtnValue;
}
