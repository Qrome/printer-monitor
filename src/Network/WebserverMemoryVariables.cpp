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
    int8_t rssi = EspController::getWifiQuality();

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
    server->sendContent("<span class='bx--header__name--prefix'>PrintBuddy&nbsp;</span>V" + String(globalDataController->getSystemSettings()->version));
    server->sendContent(String(FPSTR(HEADER_BLOCK3)));
    server->sendContent(String(FPSTR(MENUE_ITEMS)));
    server->sendContent(String(FPSTR(HEADER_BLOCK4)));

    uint32_t heapFree = 0;
    uint16_t heapMax = 0;
    uint8_t heapFrag = 0;
    EspController::getHeap(&heapFree, &heapMax, &heapFrag);

    server->sendContent("<div>WiFi Signal Strength: " + String(rssi) + "%</div>");
    server->sendContent("<div>ESP ChipID: " + String(ESP.getChipId()) + "</div>");
    server->sendContent("<div>ESP CoreVersion: " + String(ESP.getCoreVersion()) + "</div>");
    server->sendContent("<div>Heap (frag/free/max): " + String(heapFrag) + "% |" +  String(heapFree) + " b|" + String(heapMax) + " b</div>");
    server->sendContent(String(FPSTR(HEADER_BLOCK5)));
    server->sendContent(pageLabel);
    server->sendContent("</h4><h1 id='page-title' class='page-header__title'>");
    server->sendContent(pageTitle);
    server->sendContent("</h1></div>");

    if (globalDataController->getSystemSettings()->lastError.length() > 0) {
        String errorBlock = FPSTR(HEADER_BLOCK_ERROR);
        errorBlock.replace("%ERRORMSG%", globalDataController->getSystemSettings()->lastError);
        server->sendContent(errorBlock);
    }
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
    server->sendContent(FPSTR(WEATHER_FORM_START));
    WebserverMemoryVariables::sendFormCheckbox(
        server,
        FPSTR(WEATHER_FORM1_ID),
        globalDataController->getWeatherSettings()->show,
        FPSTR(WEATHER_FORM1_LABEL)
    );
    WebserverMemoryVariables::sendFormCheckbox(
        server,
        FPSTR(WEATHER_FORM2_ID),
        globalDataController->getWeatherSettings()->isMetric,
        FPSTR(WEATHER_FORM2_LABEL_ON),
        FPSTR(WEATHER_FORM2_LABEL_OFF)
    );

    String form = FPSTR(WEATHER_FORM3);
    form.replace("%WEATHERKEY%", globalDataController->getWeatherSettings()->apiKey);
    server->sendContent(form);

    form = FPSTR(WEATHER_FORM4);
    form.replace("%CITY1%", String(globalDataController->getWeatherSettings()->cityId));
    form.replace("%CITYNAME1%", globalDataController->getWeatherClient()->getCity(0));
    server->sendContent(form);

    form = FPSTR(WEATHER_FORM_OPTIONS);
    form.replace(
        ">"+String(globalDataController->getWeatherSettings()->lang)+"<",
        " selected>"+String(globalDataController->getWeatherSettings()->lang)+"<"
    );
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
    server->sendContent(FPSTR(STATION_CONFIG_FORM_START));
    WebserverMemoryVariables::sendFormCheckbox(
        server,
        FPSTR(STATION_CONFIG_FORM1_ID),
        globalDataController->getClockSettings()->show,
        FPSTR(STATION_CONFIG_FORM1_LABEL)
    );
    WebserverMemoryVariables::sendFormCheckbox(
        server,
        FPSTR(STATION_CONFIG_FORM2_ID),
        globalDataController->getClockSettings()->is24h,
        FPSTR(STATION_CONFIG_FORM2_LABEL)
    );
    WebserverMemoryVariables::sendFormCheckbox(
        server,
        FPSTR(STATION_CONFIG_FORM3_ID),
        globalDataController->getSystemSettings()->invertDisplay,
        FPSTR(STATION_CONFIG_FORM3_LABEL)
    );
    WebserverMemoryVariables::sendFormCheckbox(
        server,
        FPSTR(STATION_CONFIG_FORM4_ID),
        globalDataController->getSystemSettings()->useLedFlash,
        FPSTR(STATION_CONFIG_FORM4_LABEL)
    );

    String form = FPSTR(STATION_CONFIG_FORM5);
    String options = FPSTR(STATION_CONFIG_FORM5OPT);
    options.replace(
        ">"+String(globalDataController->getSystemSettings()->clockWeatherResyncMinutes)+"<",
        " selected>"+String(globalDataController->getSystemSettings()->clockWeatherResyncMinutes)+"<"
    );
    form.replace("%OPTIONS%", options);
    server->sendContent(form);

    form = FPSTR(STATION_CONFIG_FORM6);
    form.replace("%UTCOFFSET%", String(globalDataController->getClockSettings()->utcOffset));
    server->sendContent(form);

    WebserverMemoryVariables::sendFormCheckboxEvent(
        server,
        FPSTR(STATION_CONFIG_FORM7_ID),
        globalDataController->getSystemSettings()->hasBasicAuth,
        FPSTR(STATION_CONFIG_FORM7_LABEL),
        "showhide(this, 'uspw')"
    );

    form = FPSTR(STATION_CONFIG_FORM8);
    form.replace("%USERID%", globalDataController->getSystemSettings()->webserverUsername);
    form.replace("%STATIONPASSWORD%", globalDataController->getSystemSettings()->webserverPassword);
    server->sendContent(form);
}

/**
 * @brief Send out configuration for printer
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendPrinterConfigForm(ESP8266WebServer *server, GlobalDataController *globalDataController) {
    server->sendContent(FPSTR(CONFPRINTER_FORM_START));

    int totalPrinters = globalDataController->getNumPrinters();
    PrinterDataStruct *printerConfigs = globalDataController->getPrinterSettings();
    for(int i=0; i<totalPrinters; i++) {
        String printerEntryRow = FPSTR(CONFPRINTER_FORM_ROW);
        printerEntryRow.replace("%ID%", String(i+1));
        printerEntryRow.replace("%NAME%", String(printerConfigs[i].customName));
        printerEntryRow.replace("%TYPE%", globalDataController->getPrinterClientType(&printerConfigs[i]));

        String state = FPSTR(CONFPRINTER_FORM_ROW_OK);
        if ((printerConfigs[i].state == PRINTER_STATE_OFFLINE) || (printerConfigs[i].state == PRINTER_STATE_ERROR)) {
            state = FPSTR(CONFPRINTER_FORM_ROW_ERROR);
        }
        state.replace("%STATUS%", globalDataController->getPrinterStateAsText(&printerConfigs[i]));
        printerEntryRow.replace("%STATE%", state);
        server->sendContent(printerEntryRow);
    }

    // Generate all modals
    for(int i=0; i<totalPrinters; i++) {
        WebserverMemoryVariables::sendPrinterConfigFormAEModal(server, i + 1, &printerConfigs[i]);
    }
    WebserverMemoryVariables::sendPrinterConfigFormAEModal(server, 0, NULL);

    

/*static const char CONFPRINTER_FORM_ROW_OFFLINE[] PROGMEM =  "<div class='bx--tag bx--tag--magenta'>"
                                        "Offline"
                                    "</div>";
static const char CONFPRINTER_FORM_ROW_ONLINE[] PROGMEM =  "<div class='bx--tag bx--tag--green'>"
                                        "Online"
                                    "</div>";
*/



    server->sendContent(FPSTR(CONFPRINTER_FORM_END));
} 

void WebserverMemoryVariables::sendPrinterConfigFormAEModal(ESP8266WebServer *server, int id, PrinterDataStruct *forPrinter) {
    String printerEditModal = FPSTR(CONFPRINTER_FORM_ADDEDIT1);
    printerEditModal.replace("%ID%", String(id));
    if (id == 0) {
        printerEditModal.replace("%TITLE%", FPSTR(CONFPRINTER_FORM_ADDEDIT_TA));
    } else {
        printerEditModal.replace("%TITLE%", FPSTR(CONFPRINTER_FORM_ADDEDIT_TE));
    }

    if (forPrinter == NULL) {
        printerEditModal.replace("%NAME%", "");
        printerEditModal.replace("%TARGETADDR%", "");
        printerEditModal.replace("%TARGETPORT%", "80");
    } else {
        printerEditModal.replace("%NAME%", String(forPrinter->customName));
        printerEditModal.replace("%TARGETADDR%", String(forPrinter->remoteAddress));
        printerEditModal.replace("%TARGETPORT%", String(forPrinter->remotePort));
    }
    

    server->sendContent(printerEditModal);
}























/**
 * @brief Send out an single checkbox form row
 * @param server                    Send out instancce
 * @param formId                    Form id/name
 * @param isChecked                 Checkbox checked
 * @param label                     Text for activated/deactivated
 */
void WebserverMemoryVariables::sendFormCheckbox(ESP8266WebServer *server, String formId, bool isChecked, String label) {
    WebserverMemoryVariables::sendFormCheckboxEvent(server, formId, isChecked, label, "");
}

/**
 * @brief Send out an single checkbox form row
 * @param server                    Send out instancce
 * @param formId                    Form id/name
 * @param isChecked                 Checkbox checked
 * @param labelOn                   Text for activated
 * @param labelOff                  Text for deactivated
 */
void WebserverMemoryVariables::sendFormCheckbox(ESP8266WebServer *server, String formId, bool isChecked, String labelOn, String labelOff) {
    WebserverMemoryVariables::sendFormCheckboxEvent(server, formId, isChecked, labelOn, labelOff, "");
}

/**
 * @brief Send out an single checkbox form row with onChangeEvent
 * @param server                    Send out instancce
 * @param formId                    Form id/name
 * @param isChecked                 Checkbox checked
 * @param label                     Text for activated/deactivated
 * @param onChange                  Javascript function
 */
void WebserverMemoryVariables::sendFormCheckboxEvent(ESP8266WebServer *server, String formId, bool isChecked, String label, String onChange) {
    String onAdd = FPSTR(FORM_ITEM_CHECKBOX_ON);
    String offAdd = FPSTR(FORM_ITEM_CHECKBOX_OFF);
    WebserverMemoryVariables::sendFormCheckboxEvent(server, formId, isChecked, label + onAdd, label + offAdd, onChange);
}


/**
 * @brief Send out an single checkbox form row with onChangeEvent
 * @param server                    Send out instancce
 * @param formId                    Form id/name
 * @param isChecked                 Checkbox checked
 * @param labelOn                   Text for activated
 * @param labelOff                  Text for deactivated
 * @param onChange                  Javascript function
 */
void WebserverMemoryVariables::sendFormCheckboxEvent(ESP8266WebServer *server, String formId, bool isChecked, String labelOn, String labelOff, String onChange) {
    String isCheckedText = "";
    String onChangeText = "";
    if (isChecked) {
        isCheckedText = "checked='checked'";
    }
    if (onChange.length() > 0) {
        onChangeText = "onchange=\"" + onChange + "\"";
    }
    String form = FPSTR(FORM_ITEM_CHECKBOX);
    form.replace("%FORMID%", formId);
    form.replace("%CHECKED%", isCheckedText);
    form.replace("%LABELON%", labelOn);
    form.replace("%LABELOFF%", labelOff);
    form.replace("%ONCHANGE%", onChangeText);
    server->sendContent(form);
}