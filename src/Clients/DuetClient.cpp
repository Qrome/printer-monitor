// DUET API: https://reprap.org/wiki/RepRap_Firmware_Status_responses
// ArduinoJSON Assistant: https://arduinojson.org/v6/assistant/

#include "DuetClient.h"

DuetClient::DuetClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient)
: BasePrinterClientImpl("Duet", globalDataController, debugController, jsonRequestClient) {
    this->updatePrintClient();
}

void DuetClient::updatePrintClient() {
    encodedAuth = "";
    if (this->globalDataController->getPrinterAuthUser() != "") {
        String userpass = this->globalDataController->getPrinterAuthUser() + ":" + this->globalDataController->getPrinterAuthPass();
        base64 b64;
        encodedAuth = b64.encode(userpass, true);
    }
    pollPsu = this->globalDataController->hasPrinterPsu();
}

boolean DuetClient::validate() {
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

void DuetClient::getPrinterJobResults() {
    // const size_t bufferSize = JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(1) + 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(9) + 426;
    const size_t bufferSize = 2048; // according to ArduinoJson assistant
    DynamicJsonDocument *jsonDoc;
    if (!validate()) {
        return;
    }

    // Req 1
    this->debugController->printLn("Get Duet Data: " + this->getInstanceServerTarget() + ":" + String(this->getInstanceServerPort()));
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        this->getInstanceServerTarget(),
        this->getInstanceServerPort(),
        this->encodedAuth,
        "/rr_status?type=1",
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

    printerData.state = this->translateState((const char*)(*jsonDoc)["status"]);

    if (BasePrinterClientImpl::isOperational()) {
        this->debugController->printLn("Status: " + printerData.state);
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
        "/rr_status?type=3",
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

    printerData.filamentLength = (const char*)(*jsonDoc)["result"]["status"]["job"]["print_stats"]["filament_used"];
    printerData.progressPrintTime = (const char*)(*jsonDoc)["printDuration"];
    printerData.fileName = (const char*)(*jsonDoc)["result"]["status"]["print_stats"]["filename"];
    printerData.progressCompletion = (int)(*jsonDoc)["fractionPrinted"];
    printerData.toolTemp = (int)(*jsonDoc)["temps"]["current"][1];
    printerData.toolTargetTemp = (int)(*jsonDoc)["temps"]["tools"]["active"][0][0];
    printerData.bedTemp = (int)(*jsonDoc)["temps"]["bed"]["current"];
    printerData.bedTargetTemp = (int)(*jsonDoc)["temps"]["bed"]["active"];
    float fileProgress = (float)(*jsonDoc)["fractionPrinted"];
    printerData.progressFilepos = (const char*)(*jsonDoc)["filePosition"];
    printerData.estimatedPrintTime = (float)(*jsonDoc)["file"];

    /*
    printerData.progressPrintTimeLeft : No metadata is available, print duration and progress can be used to calculate the ETA:
    */
    float totalPrintTime = printerData.progressPrintTime.toFloat() / fileProgress;
    printerData.progressPrintTimeLeft = String(totalPrintTime - printerData.progressPrintTime.toFloat());

    if (BasePrinterClientImpl::isOperational()) {
        this->debugController->printLn("Status: " + this->getStateAsText() + " " + printerData.fileName + "(" + printerData.progressCompletion + "%)");
    }
}

void DuetClient::getPrinterPsuState() {
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
 *  - C (configuration file is being processed)
 *  - I (idle, no movement or code is being performed)
 *  - B (busy, live movement is in progress or a macro file is being run)
 *  - P (printing a file)
 *  - D (decelerating, pausing a running print)
 *  - S (stopped, live print has been paused)
 *  - R (resuming a paused print)
 *  - H (halted, after emergency stop)
 *  - F (flashing new firmware)
 *  - T (changing tool, new in 1.17b)
 */
int DuetClient::translateState(String stateText) {
    stateText.toLowerCase();
    stateText.trim();
    if (stateText == "i" || stateText == "t" || stateText == "b" || stateText == "c" || stateText == "f" || stateText == "t") {
        return PRINTER_STATE_STANDBY;
    }
    if (stateText == "p" || stateText == "r") {
        return PRINTER_STATE_PRINTING;
    }
    if (stateText == "d" || stateText == "s") {
        return PRINTER_STATE_PAUSED;
    }
    if (stateText == "h") {
        return PRINTER_STATE_ERROR;
    }
    return PRINTER_STATE_OFFLINE;
}
