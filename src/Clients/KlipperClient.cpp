// Moonraker API: https://github.com/Arksine/moonraker/blob/master/docs/web_api.md
// ArduinoJSON Assistant: https://arduinojson.org/v6/assistant/

#include "KlipperClient.h"

KlipperClient::KlipperClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient)
: BasePrinterClientImpl("Klipper", globalDataController, debugController, jsonRequestClient) {
    this->updatePrintClient();
}

void KlipperClient::updatePrintClient() {
    encodedAuth = "";
    if (this->globalDataController->getPrinterAuthUser() != "") {
        String userpass = this->globalDataController->getPrinterAuthUser() + ":" + this->globalDataController->getPrinterAuthPass();
        base64 b64;
        encodedAuth = b64.encode(userpass, true);
    }
    pollPsu = this->globalDataController->hasPrinterPsu();
}

boolean KlipperClient::validate() {
    boolean rtnValue = false;
    printerData.error = "";
    if ((this->globalDataController->getPrinterServer() == "") && (this->globalDataController->getPrinterHostName() == "")) {
        printerData.error += "Server address or host name is required; ";
    }
    if (printerData.error == "") {
        rtnValue = true;
    }
    return rtnValue;
}

void KlipperClient::getPrinterJobResults() {
    // const size_t bufferSize = JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(1) + 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(9) + 426;
    const size_t bufferSize = 1536; // according to ArduinoJson assistant
    DynamicJsonDocument *jsonDoc;
    if (!validate()) {
        return;
    }

    // Req 1
    this->debugController->printLn("Get Klipper Data: " + this->getInstanceServerTarget() + ":" + String(this->getInstanceServerPort()));
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        this->getInstanceServerTarget(),
        this->getInstanceServerPort(),
        this->encodedAuth,
        "/printer/objects/query?print_stats",
        "",
        bufferSize,
        true
    );
    if (this->jsonRequestClient->getLastError() != "") {
        this->debugController->printLn(this->jsonRequestClient->getLastError());
        printerData.error = this->jsonRequestClient->getLastError();
        printerData.state = PRINTER_STATE_OFFLINE;
        printerData.isPrinting = false;
        printerData.toolTemp = "";
        printerData.toolTargetTemp = "";
        printerData.bedTemp = "";
        printerData.bedTargetTemp = "";
        return;
    }

    printerData.state = this->translateState((const char*)(*jsonDoc)["result"]["status"]["print_stats"]["state"]);
    printerData.filamentLength = (const char*)(*jsonDoc)["result"]["status"]["job"]["print_stats"]["filament_used"];
    printerData.progressPrintTime = (const char*)(*jsonDoc)["result"]["status"]["print_stats"]["print_duration"];
    printerData.fileName = (const char*)(*jsonDoc)["result"]["status"]["print_stats"]["filename"];

    if (BasePrinterClientImpl::isOperational()) {
        this->debugController->printLn("Status: " + this->getStateAsText());
    } else {
        this->debugController->printLn("Printer Not Operational");
    }

    if (printerData.state == PRINTER_STATE_PRINTING) {
        printerData.isPrinting = true;
    } else {
        // We dont printing, so abort function here
        printerData.isPrinting = false;
        return;
    }

    // Req 2
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        this->getInstanceServerTarget(),
        this->getInstanceServerPort(),
        this->encodedAuth,
        "/printer/objects/query?heater_bed&extruder&display_status&toolhead&virtual_sdcard",
        "",
        bufferSize,
        true
    );
    if (this->jsonRequestClient->getLastError() != "") {
        this->debugController->printLn(this->jsonRequestClient->getLastError());
        printerData.error = this->jsonRequestClient->getLastError();
        printerData.state = PRINTER_STATE_OFFLINE;
        printerData.isPrinting = false;
        printerData.toolTemp = "";
        printerData.toolTargetTemp = "";
        printerData.bedTemp = "";
        printerData.bedTargetTemp = "";
        return;
    }

    printerData.progressCompletion = (int)(*jsonDoc)["result"]["status"]["display_status"]["progress"];
    printerData.toolTemp = (int)(*jsonDoc)["result"]["status"]["extruder"]["temperature"];
    printerData.toolTargetTemp = (int)(*jsonDoc)["result"]["status"]["extruder"]["target"];
    printerData.bedTemp = (int)(*jsonDoc)["result"]["status"]["heater_bed"]["temperature"];
    printerData.bedTargetTemp = (int)(*jsonDoc)["result"]["status"]["heater_bed"]["target"];
    float fileProgress = (float)(*jsonDoc)["result"]["status"]["virtual_sdcard"]["progress"];
    printerData.progressFilepos = (const char*)(*jsonDoc)["result"]["status"]["virtual_sdcard"]["file_position"];
    printerData.estimatedPrintTime = (float)(*jsonDoc)["result"]["status"]["toolhead"]["estimated_print_time"];

    /*
    printerData.progressPrintTimeLeft : No metadata is available, print duration and progress can be used to calculate the ETA:
    */
    float totalPrintTime = printerData.progressPrintTime.toFloat() / fileProgress;
    printerData.progressPrintTimeLeft = String(totalPrintTime - printerData.progressPrintTime.toFloat());
    
    if (BasePrinterClientImpl::isOperational()) {
        this->debugController->printLn("Status: " + this->getStateAsText() + " " + printerData.fileName + "(" + printerData.progressCompletion + "%)");
    }
}

void KlipperClient::getPrinterPsuState() {
    /*// get the PSU state (if enabled and printer operational)
    if (pollPsu && BasePrinterClientImpl::isOperational()) {
        if (!validate()) {
            printerData.isPSUoff = false; // we do not know PSU state, so assume on.
            return;
        }
        String apiPostData = "POST /api/plugin/psucontrol HTTP/1.1";
        String apiPostBody = "{\"command\":\"getPSUState\"}";
        WiFiClient printClient = getPostRequest(apiPostData,apiPostBody);
        if (printerData.error != "") {
            printerData.isPSUoff = false; // we do not know PSU state, so assume on.
            return;
        }
        const size_t bufferSize3 = JSON_OBJECT_SIZE(2) + 300;
        DynamicJsonDocument jsonBuffer(bufferSize3);
    
        // Parse JSON object
        DeserializationError error = deserializeJson(jsonBuffer, printClient);
        if (error) {
            printerData.isPSUoff = false; // we do not know PSU state, so assume on
            return;
        }
    
        String psu = (const char*)jsonBuffer["isPSUOn"];
        if (psu == "true") {
            printerData.isPSUoff = false; // PSU checked and is on
        } else {
            printerData.isPSUoff = true; // PSU checked and is off, set flag
        }
        printClient.stop(); //stop client
    } else {
        printerData.isPSUoff = false; // we are not checking PSU state, so assume on
    } */
}

/**
 * We translate the avail states 
 *  - "standby": No print in progress
 *  - "printing": The printer is currently printing
 *  - "paused": A print in progress has been paused
 *  - "error": The print exited with an error. print_stats.message contains a related error message
 *  - "complete": The last print has completed
 */
int KlipperClient::translateState(String stateText) {
    stateText.toLowerCase();
    stateText.trim();
    if (stateText == "standby") {
        return PRINTER_STATE_STANDBY;
    }
    if (stateText == "printing") {
        return PRINTER_STATE_PRINTING;
    }
    if (stateText == "paused") {
        return PRINTER_STATE_PAUSED;
    }
    if (stateText == "complete") {
        return PRINTER_STATE_COMPLETED;
    }
    if (stateText == "error") {
        return PRINTER_STATE_ERROR;
    }
    return PRINTER_STATE_OFFLINE;
}
