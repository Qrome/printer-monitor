#include "WebserverMemoryVariables.h"

/**
 * @brief Send out header for webpage
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendHeader(ESP8266WebServer *server, GlobalDataController *globalDataController, String pageLabel, String pageTitle) {
    WebserverMemoryVariables::sendHeader(server, globalDataController, pageLabel, pageTitle, false);
}

/**
 * @brief Send out header for webpage
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 * @param pageLabel                 Title label
 * @param pageTitle                 Title
 * @param refresh                   if true, auto refresh in header is set
 */
void WebserverMemoryVariables::sendHeader(
    ESP8266WebServer *server,
    GlobalDataController *globalDataController,
    String pageLabel,
    String pageTitle,
    boolean refresh
) {
    globalDataController->ledOnOff(true);
    int8_t rssi = globalDataController->getWifiQuality();

    server->sendHeader("Cache-Control", "no-cache, no-store");
    server->sendHeader("Pragma", "no-cache");
    server->sendHeader("Expires", "-1");
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    server->send(200, "text/html", "");

    server->sendContent(String(FPSTR(HEADER_BLOCK1)));
    if (refresh) {
        server->sendContent("<meta http-equiv=\"refresh\" content=\"30\">");
    }
    server->sendContent(String(FPSTR(HEADER_BLOCK2)));
    server->sendContent("<span class='bx--header__name--prefix'>PrintBuddy&nbsp;</span>V" + String(VERSION));
    server->sendContent(String(FPSTR(HEADER_BLOCK3)));
    server->sendContent(String(FPSTR(MENUE_ITEMS)));
    server->sendContent(String(FPSTR(HEADER_BLOCK4)));

    uint32_t heapFree = 0;
    uint16_t heapMax = 0;
    uint8_t heapFrag = 0;
    ESP.getHeapStats(&heapFree, &heapMax, &heapFrag);

    server->sendContent("<div>WiFi Signal Strength: " + String(rssi) + "%</div>");
    server->sendContent("<div>ESP ChipID: " + String(ESP.getChipId()) + "</div>");
    server->sendContent("<div>ESP CoreVersion: " + String(ESP.getCoreVersion()) + "</div>");
    server->sendContent("<div>Heap (frag/free/max): " + String(heapFrag) + "|" +  String(heapFree) + "|" + String(heapMax) + "</div>");
    server->sendContent(String(FPSTR(HEADER_BLOCK5)));
    server->sendContent(pageLabel);
    server->sendContent("</h4><h1 id='page-title' class='page-header__title'>");
    server->sendContent(pageTitle);
    server->sendContent("</h1></div>");
}

/**
 * @brief send out footer content for webpage
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendFooter(ESP8266WebServer *server, GlobalDataController *globalDataController) {
    server->sendContent(String(FPSTR(FOOTER_BLOCK)));
    server->sendContent("");
    server->client().stop();
    globalDataController->ledOnOff(false);
}

/**
 * @brief Send out upload form for updates
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendUpdateForm(ESP8266WebServer *server, GlobalDataController *globalDataController) {
    server->sendContent(FPSTR(UPDATE_FORM));
}

/**
 * @brief Send out configuration for weather
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendWeatherConfigForm(ESP8266WebServer *server, GlobalDataController *globalDataController) {
    String isWeatherChecked = "";
    String isMetricChecked = "";
    if (globalDataController->getWeatherShow()) {
        isWeatherChecked = "checked='checked'";
    }
    if (globalDataController->getWeatherIsMetric()) {
        isMetricChecked = "checked='checked'";
    }

    String form = FPSTR(WEATHER_FORM1);
    form.replace("%IS_WEATHER_CHECKED%", isWeatherChecked);
    server->sendContent(form);

    form = FPSTR(WEATHER_FORM2);
    form.replace("%METRIC%", isMetricChecked);
    server->sendContent(form);

    form = FPSTR(WEATHER_FORM3);
    form.replace("%WEATHERKEY%", globalDataController->getWeatherApiKey());
    server->sendContent(form);

    form = FPSTR(WEATHER_FORM4);
    form.replace("%CITY1%", String(globalDataController->getWeatherCityId()));
    form.replace("%CITYNAME1%", globalDataController->getWeatherClient()->getCity(0));
    server->sendContent(form);

    form = FPSTR(WEATHER_FORM_OPTIONS);
    form.replace(">"+String(globalDataController->getWeatherLang())+"<", " selected>"+String(globalDataController->getWeatherLang())+"<");
    server->sendContent(form);

    form = FPSTR(WEATHER_FORM5);
    server->sendContent(form);
}

/**
 * @brief Send out configuration for station
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendStationConfigForm(ESP8266WebServer *server, GlobalDataController *globalDataController) {
    String isClockChecked = "";
    String is24hourChecked = "";
    String isInvDisp = "";
    String isFlashLED = "";
    String isUseSecurityChecked = "";
    if (globalDataController->isDisplayInverted()) {
        isInvDisp = "checked='checked'";
    }
    if (globalDataController->useLedFlash()) {
        isFlashLED = "checked='checked'";
    }
    if (globalDataController->getDisplayClock()) {
        isClockChecked = "checked='checked'";
    }
    if (globalDataController->getClockIs24h()) {
        is24hourChecked = "checked='checked'";
    }
    if (globalDataController->getWebserverIsBasicAuth()) {
        isUseSecurityChecked = "checked='checked'";
    }

    String form = FPSTR(STATION_CONFIG_FORM1);
    form.replace("%IS_CLOCK_CHECKED%", isClockChecked);
    server->sendContent(form);

    form = FPSTR(STATION_CONFIG_FORM2);
    form.replace("%IS_24HOUR_CHECKED%", is24hourChecked);
    server->sendContent(form);

    form = FPSTR(STATION_CONFIG_FORM3);
    form.replace("%IS_INVDISP_CHECKED%", isInvDisp);
    server->sendContent(form);

    form = FPSTR(STATION_CONFIG_FORM4);
    form.replace("%USEFLASH%", isFlashLED);
    server->sendContent(form);

    form = FPSTR(STATION_CONFIG_FORM5);
    String options = FPSTR(STATION_CONFIG_FORM5OPT);
    options.replace(">"+String(globalDataController->getClockResyncMinutes())+"<", " selected>"+String(globalDataController->getClockResyncMinutes())+"<");
    form.replace("%OPTIONS%", options);
    server->sendContent(form);

    form = FPSTR(STATION_CONFIG_FORM6);
    form.replace("%UTCOFFSET%", String(globalDataController->getClockUtcOffset()));
    server->sendContent(form);

    form = FPSTR(STATION_CONFIG_FORM7);
    form.replace("%IS_BASICAUTH_CHECKED%", isUseSecurityChecked);
    server->sendContent(form);

    form = FPSTR(STATION_CONFIG_FORM8);
    form.replace("%USERID%", globalDataController->getWebserverUsername());
    form.replace("%STATIONPASSWORD%", globalDataController->getWebserverPassword());
    server->sendContent(form);
}
