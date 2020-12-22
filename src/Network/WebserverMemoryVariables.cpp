#include "WebserverMemoryVariables.h"

String WebserverMemoryVariables::rowExtraClass = "";

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
    if (globalDataController->getSystemSettings()->lastOk.length() > 0) {
        String okBlock = FPSTR(HEADER_BLOCK_OK);
        okBlock.replace("%OKMSG%", globalDataController->getSystemSettings()->lastOk);
        server->sendContent(okBlock);
        globalDataController->getSystemSettings()->lastOk = "";
    }
}

/**
 * @brief send out footer content for webpage
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendFooter(ESP8266WebServer *server, GlobalDataController *globalDataController) {
    WebserverMemoryVariables::sendModalDanger(
        server,
        "resetSettingsModal",
        FPSTR(GLOBAL_TEXT_WARNING),
        FPSTR(GLOBAL_TEXT_TRESET),
        FPSTR(GLOBAL_TEXT_CRESET),
        FPSTR(GLOBAL_TEXT_ABORT),
        FPSTR(GLOBAL_TEXT_RESET),
        "onclick='openUrl(\"/systemreset\")'"
    );
    WebserverMemoryVariables::sendModalDanger(
        server,
        "resetWifiModal",
        FPSTR(GLOBAL_TEXT_WARNING),
        FPSTR(GLOBAL_TEXT_TFWIFI),
        FPSTR(GLOBAL_TEXT_CFWIFI),
        FPSTR(GLOBAL_TEXT_ABORT),
        FPSTR(GLOBAL_TEXT_RESET),
        "onclick='openUrl(\"/forgetwifi\")'"
    );
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
        FPSTR(WEATHER_FORM1_LABEL),
        true,
        ""
    );
    WebserverMemoryVariables::sendFormCheckbox(
        server,
        FPSTR(WEATHER_FORM2_ID),
        globalDataController->getWeatherSettings()->isMetric,
        FPSTR(WEATHER_FORM2_LABEL_ON),
        FPSTR(WEATHER_FORM2_LABEL_OFF),
        true,
        ""
    );
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(WEATHER_FORM3_ID),
        FPSTR(WEATHER_FORM3_LABEL),
        globalDataController->getWeatherSettings()->apiKey,
        "",
        60,
        "",
        false,
        true,
        ""
    );
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(WEATHER_FORM4_ID),
        globalDataController->getWeatherClient()->getCity(0) + FPSTR(WEATHER_FORM4_LABEL),
        String(globalDataController->getWeatherSettings()->cityId),
        "",
        120,
        "onkeypress='return isNumberKey(event)'",
        false,
        true,
        ""
    );
    WebserverMemoryVariables::sendFormSelect(
        server,
        FPSTR(WEATHER_FORM5_ID),
        FPSTR(WEATHER_FORM5_LABEL),
        String(globalDataController->getWeatherSettings()->lang),
        "",
        FPSTR(WEATHER_FORM5_OPTIONS),
        true,
        ""
    );
    WebserverMemoryVariables::sendFormSubmitButton(server, true);
    server->sendContent(FPSTR(WEATHER_FORM_END));
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
        FPSTR(STATION_CONFIG_FORM1_LABEL),
        true,
        ""
    );
    WebserverMemoryVariables::sendFormCheckbox(
        server,
        FPSTR(STATION_CONFIG_FORM2_ID),
        globalDataController->getClockSettings()->is24h,
        FPSTR(STATION_CONFIG_FORM2_LABEL),
        true,
        ""
    );
    WebserverMemoryVariables::sendFormCheckbox(
        server,
        FPSTR(STATION_CONFIG_FORM3_ID),
        globalDataController->getSystemSettings()->invertDisplay,
        FPSTR(STATION_CONFIG_FORM3_LABEL),
        true,
        ""
    );
    WebserverMemoryVariables::sendFormCheckbox(
        server,
        FPSTR(STATION_CONFIG_FORM4_ID),
        globalDataController->getSystemSettings()->useLedFlash,
        FPSTR(STATION_CONFIG_FORM4_LABEL),
        true,
        ""
    );
    WebserverMemoryVariables::sendFormSelect(
        server,
        FPSTR(STATION_CONFIG_FORM5_ID),
        FPSTR(STATION_CONFIG_FORM5_LABEL),
        String(globalDataController->getSystemSettings()->clockWeatherResyncMinutes),
        "",
        FPSTR(STATION_CONFIG_FORM5_OPTIONS),
        true,
        ""
    );
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(STATION_CONFIG_FORM6_ID),
        FPSTR(STATION_CONFIG_FORM6_LABEL),
        String(globalDataController->getClockSettings()->utcOffset),
        "",
        120,
        "onkeypress='return isNumberKey(event)'",
        false,
        true,
        ""
    );
    WebserverMemoryVariables::sendFormCheckboxEvent(
        server,
        FPSTR(STATION_CONFIG_FORM7_ID),
        globalDataController->getSystemSettings()->hasBasicAuth,
        FPSTR(STATION_CONFIG_FORM7_LABEL),
        "showhide('isBasicAuth', 'uspw')",
        true,
        ""
    );
    WebserverMemoryVariables::rowExtraClass = "data-sh='uspw'";
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(STATION_CONFIG_FORM8_ID),
        FPSTR(STATION_CONFIG_FORM8_LABEL),
        globalDataController->getSystemSettings()->webserverUsername,
        "",
        20,
        "",
        false,
        true,
        ""
    );
    WebserverMemoryVariables::rowExtraClass = "data-sh='uspw'";
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(STATION_CONFIG_FORM9_ID),
        FPSTR(STATION_CONFIG_FORM9_LABEL),
        globalDataController->getSystemSettings()->webserverPassword,
        "",
        120,
        "",
        true,
        true,
        ""
    );
    WebserverMemoryVariables::sendFormSubmitButton(server, true);
    server->sendContent(FPSTR(STATION_CONFIG_FORM_END));
}

/**
 * @brief Send out configuration for printer
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendPrinterConfigForm(ESP8266WebServer *server, GlobalDataController *globalDataController) {   
    int totalPrinters = globalDataController->getNumPrinters();
    PrinterDataStruct *printerConfigs = globalDataController->getPrinterSettings();

    // Show all errors if printers have one
    for(int i=0; i<totalPrinters; i++) {
        if (printerConfigs[i].state == PRINTER_STATE_ERROR) {
            String errorBlock = FPSTR(HEADER_BLOCK_ERROR);
            errorBlock.replace("%ERRORMSG%", "[" + String(printerConfigs[i].customName) + "] " + String(printerConfigs[i].error));
            server->sendContent(errorBlock);
        }
    }

    // Show printers
    server->sendContent(FPSTR(CONFPRINTER_FORM_START));
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
        WebserverMemoryVariables::sendPrinterConfigFormAEModal(server, i + 1, &printerConfigs[i], globalDataController);
        String textForDelete = FPSTR(GLOBAL_TEXT_CDPRINTER);
        textForDelete.replace("%PRINTERNAME%", String(printerConfigs[i].customName));
        WebserverMemoryVariables::sendModalDanger(
            server,
            "deletePrinterModal-" + String(i + 1),
            FPSTR(GLOBAL_TEXT_WARNING),
            FPSTR(GLOBAL_TEXT_TDPRINTER),
            textForDelete,
            FPSTR(GLOBAL_TEXT_ABORT),
            FPSTR(GLOBAL_TEXT_DELETE),
            "onclick='openUrl(\"/configureprinter/delete?id=" + String(i + 1) + "\")'"
        );
    }
    WebserverMemoryVariables::sendPrinterConfigFormAEModal(server, 0, NULL, globalDataController);
    server->sendContent(FPSTR(CONFPRINTER_FORM_END));
} 

/**
 * @brief Modal for printer edit/add
 * 
 * @param server 
 * @param id 
 * @param forPrinter 
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendPrinterConfigFormAEModal(ESP8266WebServer *server, int id, PrinterDataStruct *forPrinter, GlobalDataController *globalDataController) {
    
    String modalData = FPSTR(CONFPRINTER_FORM_ADDEDIT_START);
    modalData.replace("%ID%", String(id));
    if (id == 0) {
        modalData.replace("%TITLE%", FPSTR(CONFPRINTER_FORM_ADDEDIT_TA));
    } else {
        modalData.replace("%TITLE%", FPSTR(CONFPRINTER_FORM_ADDEDIT_TE));
    }
    server->sendContent(modalData);
    
    String optionData = "";
    BasePrinterClient** printerInstances = globalDataController->getRegisteredPrinterClients();
    for (int i=0; i<globalDataController->getRegisteredPrinterClientsNum(); i++) {
        if (printerInstances[i] == NULL) {
            continue;
        }
        optionData += "<option class='bx--select-option' value='" + String(i) + "'";
        
        if (printerInstances[i]->clientNeedApiKey()) {
            optionData += " data-need-api='true'";
        }
        if ((forPrinter != NULL) && (forPrinter->apiType == i)) {
            optionData += " selected='selected'";
        }
        optionData += ">" + printerInstances[i]->getClientType() + "</option>";
    }
    
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(CONFPRINTER_FORM_ADDEDIT1_ID),
        FPSTR(CONFPRINTER_FORM_ADDEDIT1_LABEL),
        id > 0 ? String(forPrinter->customName) : "",
        FPSTR(CONFPRINTER_FORM_ADDEDIT1_PH),
        20,
        "",
        false,
        false,
        String(id)
    );
    WebserverMemoryVariables::sendFormSelect(
        server,
        FPSTR(CONFPRINTER_FORM_ADDEDIT2_ID),
        FPSTR(CONFPRINTER_FORM_ADDEDIT2_LABEL),
        "",
        "onchange='apiTypeSelect(\"" + String(FPSTR(CONFPRINTER_FORM_ADDEDIT2_ID)) + "\", \"apacapi-" + String(id) + "\")'",
        optionData,
        false,
        String(id)
    );
    WebserverMemoryVariables::rowExtraClass = "data-sh='apacapi-" + String(id) + "'";
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(CONFPRINTER_FORM_ADDEDIT3_ID),
        FPSTR(CONFPRINTER_FORM_ADDEDIT3_LABEL),
        id > 0 ? String(forPrinter->apiKey) : "",
        FPSTR(CONFPRINTER_FORM_ADDEDIT3_PH),
        60,
        "",
        false,
        false,
        String(id)
    );
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(CONFPRINTER_FORM_ADDEDIT4_ID),
        FPSTR(CONFPRINTER_FORM_ADDEDIT4_LABEL),
        id > 0 ? String(forPrinter->remoteAddress) : "",
        FPSTR(CONFPRINTER_FORM_ADDEDIT4_PH),
        60,
        "",
        false,
        false,
        String(id)
    );
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(CONFPRINTER_FORM_ADDEDIT5_ID),
        FPSTR(CONFPRINTER_FORM_ADDEDIT5_LABEL),
        id > 0 ? String(forPrinter->remotePort) : "80",
        "",
        5,
        "onkeypress='return isNumberKey(event)'",
        false,
        false,
        String(id)
    );
    WebserverMemoryVariables::sendFormCheckboxEvent(
        server,
        FPSTR(CONFPRINTER_FORM_ADDEDIT6_ID),
        id > 0 ? forPrinter->basicAuthNeeded : true,
        FPSTR(CONFPRINTER_FORM_ADDEDIT6_LABEL),
        "showhide('" + String(FPSTR(CONFPRINTER_FORM_ADDEDIT6_ID)) + "-" + String(id) + "', 'apac-" + String(id) + "')",
        false,
        String(id)
    );
    WebserverMemoryVariables::rowExtraClass = "data-sh='apac-" + String(id) + "'";
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(STATION_CONFIG_FORM7_ID),
        FPSTR(STATION_CONFIG_FORM7_LABEL),
        id > 0 ? String(forPrinter->basicAuthUsername) : "",
        FPSTR(CONFPRINTER_FORM_ADDEDIT7_PH),
        30,
        "",
        false,
        false,
        String(id)
    );
    WebserverMemoryVariables::rowExtraClass = "data-sh='apac-" + String(id) + "'";
    WebserverMemoryVariables::sendFormInput(
        server,
        FPSTR(STATION_CONFIG_FORM8_ID),
        FPSTR(STATION_CONFIG_FORM8_LABEL),
        id > 0 ? String(forPrinter->basicAuthPassword) : "",
        FPSTR(CONFPRINTER_FORM_ADDEDIT8_PH),
        120,
        "",
        true,
        false,
        String(id)
    );
    modalData = FPSTR(CONFPRINTER_FORM_ADDEDIT_END);
    modalData.replace("%ID%", String(id));
    server->sendContent(modalData);
}























/**
 * @brief Send out an single checkbox form row
 * @param server                    Send out instancce
 * @param formId                    Form id/name
 * @param isChecked                 Checkbox checked
 * @param label                     Text for activated/deactivated
 * @param inRow                     Extend the field with row div
 */
void WebserverMemoryVariables::sendFormCheckbox(ESP8266WebServer *server, String formId, bool isChecked, String label, bool inRow, String uniqueId = "") {
    WebserverMemoryVariables::sendFormCheckboxEvent(server, formId, isChecked, label, "", inRow, uniqueId);
}

/**
 * @brief Send out an single checkbox form row
 * @param server                    Send out instancce
 * @param formId                    Form id/name
 * @param isChecked                 Checkbox checked
 * @param labelOn                   Text for activated
 * @param labelOff                  Text for deactivated
 * @param inRow                     Extend the field with row div
 */
void WebserverMemoryVariables::sendFormCheckbox(ESP8266WebServer *server, String formId, bool isChecked, String labelOn, String labelOff, bool inRow, String uniqueId = "") {
    WebserverMemoryVariables::sendFormCheckboxEvent(server, formId, isChecked, labelOn, labelOff, "", inRow, uniqueId);
}

/**
 * @brief Send out an single checkbox form row with onChangeEvent
 * @param server                    Send out instancce
 * @param formId                    Form id/name
 * @param isChecked                 Checkbox checked
 * @param label                     Text for activated/deactivated
 * @param onChange                  Javascript function
 * @param inRow                     Extend the field with row div
 */
void WebserverMemoryVariables::sendFormCheckboxEvent(
    ESP8266WebServer *server,
    String formId,
    bool isChecked,
    String label,
    String onChange,
    bool inRow,
    String uniqueId = ""
) {
    String onAdd = FPSTR(FORM_ITEM_CHECKBOX_ON);
    String offAdd = FPSTR(FORM_ITEM_CHECKBOX_OFF);
    WebserverMemoryVariables::sendFormCheckboxEvent(server, formId, isChecked, label + onAdd, label + offAdd, onChange, inRow, uniqueId);
}

/**
 * @brief Send out an single checkbox form row with onChangeEvent
 * @param server                    Send out instancce
 * @param formId                    Form id/name
 * @param isChecked                 Checkbox checked
 * @param labelOn                   Text for activated
 * @param labelOff                  Text for deactivated
 * @param onChange                  Javascript function
 * @param inRow                     Extend the field with row div
 */
void WebserverMemoryVariables::sendFormCheckboxEvent(
    ESP8266WebServer *server,
    String formId,
    bool isChecked,
    String labelOn,
    String labelOff,
    String onChange,
    bool inRow,
    String uniqueId = ""
) {
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
    WebserverMemoryVariables::sendForm(server, formId, form, inRow, uniqueId);
}


/**
 * @brief Send out an single input field form row
 * @param server                    Send out instancce
 * @param formId                    Form id/name
 * @param label                     Text for label head
 * @param value                     Value in field
 * @param placeholder               Placeholder text for input field
 * @param maxLen                    Max text len in input field
 * @param events                    Extra events for input field
 * @param isPassword                True if password field
 * @param inRow                     Extend the field with row div
 * @param uniqueId                  Unique key for ids
 */
void WebserverMemoryVariables::sendFormInput(
    ESP8266WebServer *server,
    String formId,
    String label,
    String value,
    String placeholder,
    int maxLen,
    String events,
    bool isPassword,
    bool inRow,
    String uniqueId = ""
) {
    String form = FPSTR(FORM_ITEM_INPUT);
    form.replace("%FORMID%", formId);
    form.replace("%LABEL%", label);
    form.replace("%VALUE%", value);
    form.replace("%MAXLEN%", String(maxLen));
    form.replace("%EVENTS%", events);
    form.replace("%PLACEHOLDER%", placeholder);
    if (isPassword) {
        form.replace("%FIELDTYPE%", "password");
    }
    else {
        form.replace("%FIELDTYPE%", "text");
    }
    WebserverMemoryVariables::sendForm(server, formId, form, inRow, uniqueId);
}

/**
 * @brief Send out an single input field form row
 * @param server                    Send out instancce
 * @param formId                    Form id/name
 * @param label                     Text for label head
 * @param value                     Value in field
 * @param events                    Extra events for input field
 * @param inRow                     Extend the field with row div
 * @param uniqueId                  Unique key for ids
 */
void WebserverMemoryVariables::sendFormSelect(
    ESP8266WebServer *server,
    String formId,
    String label,
    String value,
    String events,
    String options,
    bool inRow,
    String uniqueId = ""
) {
    String form = FPSTR(FORM_ITEM_SELECT_START);
    form.replace("%FORMID%", formId);
    form.replace("%LABEL%", label);
    form.replace("%EVENTS%", events);

    if (value.length() > 0) {
        options.replace(
            ">"+ value + "<",
            " selected>" + String(value) + "<"
        );
    }
    
    WebserverMemoryVariables::sendForm(
        server,
        formId,
        form + options + FPSTR(FORM_ITEM_SELECT_END),
        inRow,
        uniqueId
    );   
}

/**
 * @brief Send form out to client
 * 
 * @param server                    Send out instancce
 * @param formElement               Form element
 * @param inRow                     True if in row
 */
void WebserverMemoryVariables::sendFormSubmitButton(
    ESP8266WebServer *server,
    bool inRow
) {
    WebserverMemoryVariables::sendForm(
        server,
        "",
        FPSTR(FORM_ITEM_SUBMIT),
        inRow,
        ""
    );
}

/**
 * @brief Send form out to client
 * 
 * @param server                    Send out instance
 * @param formId                    Form id/name
 * @param formElement               Form element
 * @param inRow                     True if in row
 * @param uniqueId                  Unique key for ids
 */
void WebserverMemoryVariables::sendForm(
    ESP8266WebServer *server,
    String formId,
    String formElement,
    bool inRow,
    String uniqueId
) {
    if (uniqueId.length() > 0) {
        formElement.replace("id='" + formId + "'", "id='" + formId + "-" + uniqueId + "'");
        formElement.replace("for='" + formId + "'", "for='" + formId + "-"  + uniqueId + "'");
        formElement.replace("\"" + formId + "\"", "\"" + formId + "-"  + uniqueId + "\"");
    }
    if (inRow) {
        String rowStartData = FPSTR(FORM_ITEM_ROW_START);
        rowStartData.replace("%ROWEXTRACLASS%", WebserverMemoryVariables::rowExtraClass);
        server->sendContent(rowStartData);
        formElement.replace("%ROWEXT%", FPSTR(FORM_ITEM_ROW_EXT));
        formElement.replace("%DIVEXTRACLASS%", "");
    } else {
        formElement.replace("%ROWEXT%", "");
        formElement.replace("%DIVEXTRACLASS%", WebserverMemoryVariables::rowExtraClass);
    }
    WebserverMemoryVariables::rowExtraClass = "";
    server->sendContent(formElement);
    if (inRow) {
        server->sendContent(FPSTR(FORM_ITEM_ROW_END));
    }
}

/**
 * @brief Send danger modal out to client
 * 
 * @param server                    Send out instancce
 * @param formId                    ID of element
 * @param label                     Label top
 * @param title                     Dialog title
 * @param content                   Dialog detailed content
 * @param secActionTitle            Title of secondary button
 * @param primActionTitle           Title of primary button
 * @param primActionEvent           Event of primary button
 */
void WebserverMemoryVariables::sendModalDanger(
    ESP8266WebServer *server,
    String formId,
    String label,
    String title,
    String content,
    String secActionTitle,
    String primActionTitle,
    String primActionEvent
) {
    String modalDialog = FPSTR(MODAL_DANGER);
    modalDialog.replace("%ID%", formId);
    modalDialog.replace("%LABEL%", label);
    modalDialog.replace("%HEADING%", title);
    modalDialog.replace("%CONTENT%", content);
    modalDialog.replace("%SECACTION%", secActionTitle);
    modalDialog.replace("%MAINACTION%", primActionTitle);
    modalDialog.replace("%MAINEVENT%", primActionEvent);
    server->sendContent(modalDialog);
}

