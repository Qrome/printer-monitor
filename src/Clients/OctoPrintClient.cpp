// ArduinoJSON Assistant: https://arduinojson.org/v6/assistant/

#include "OctoPrintClient.h"

OctoPrintClient::OctoPrintClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient)
: BasePrinterClientImpl("Klipper", globalDataController, debugController, jsonRequestClient) {
    this->updatePrintClient();
}

void OctoPrintClient::updatePrintClient() {
    encodedAuth = "";
    if (this->globalDataController->getPrinterAuthUser() != "") {
        String userpass = this->globalDataController->getPrinterAuthUser() + ":" + this->globalDataController->getPrinterAuthPass();
        base64 b64;
        encodedAuth = b64.encode(userpass, true);
    }
    pollPsu = this->globalDataController->hasPrinterPsu();
}

boolean OctoPrintClient::validate() {
    boolean rtnValue = false;
    printerData.error = "";
    if ((this->globalDataController->getPrinterServer() == "") && (this->globalDataController->getPrinterHostName() == "")) {
        printerData.error += "Server address or host name is required; ";
    }
     if (this->globalDataController->getPrinterApiKey() == "") {
        printerData.error += "ApiKey is required; ";
    }
    if (printerData.error == "") {
        rtnValue = true;
    }
    return rtnValue;
}

void OctoPrintClient::getPrinterJobResults() {
    const size_t bufferSize = 1414; // according to ArduinoJson assistant
    DynamicJsonDocument *jsonDoc;
    if (!validate()) {
        return;
    }

    // Req 1
    this->debugController->printLn("Get OctoPrint Data: " + this->getInstanceServerTarget() + ":" + String(this->getInstanceServerPort()));
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        this->getInstanceServerTarget(),
        this->getInstanceServerPort(),
        this->encodedAuth,
        "/api/job",
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

    printerData.averagePrintTime = (const char*)(*jsonDoc)["job"]["averagePrintTime"];
    printerData.estimatedPrintTime = (const char*)(*jsonDoc)["job"]["estimatedPrintTime"];
    printerData.fileName = (const char*)(*jsonDoc)["job"]["file"]["name"];
    printerData.fileSize = (const char*)(*jsonDoc)["job"]["file"]["size"];
    printerData.lastPrintTime = (const char*)(*jsonDoc)["job"]["lastPrintTime"];
    printerData.progressCompletion = (const char*)(*jsonDoc)["progress"]["completion"];
    printerData.progressFilepos = (const char*)(*jsonDoc)["progress"]["filepos"];
    printerData.progressPrintTime = (const char*)(*jsonDoc)["progress"]["printTime"];
    printerData.progressPrintTimeLeft = (const char*)(*jsonDoc)["progress"]["printTimeLeft"];
    printerData.filamentLength = (const char*)(*jsonDoc)["job"]["filament"]["tool0"]["length"];
    printerData.state = this->translateState((const char*)(*jsonDoc)["state"]);

    if (BasePrinterClientImpl::isOperational()) {
        this->debugController->printLn("Status: " + printerData.state);
    } else {
        this->debugController->printLn("Printer Not Operational");
    }





    // Req 2
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        this->getInstanceServerTarget(),
        this->getInstanceServerPort(),
        this->encodedAuth,
        "/api/printer?exclude=sd,history",
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

void OctoPrintClient::getPrinterPsuState() {
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
int OctoPrintClient::translateState(String stateText) {
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
