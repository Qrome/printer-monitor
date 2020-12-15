// Octoprint API: https://docs.octoprint.org/en/master/api/index.html
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
        this->debugController->printLn("Status: " + this->getStateAsText());
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

    String printing = (const char*)(*jsonDoc)["state"]["flags"]["printing"];
    if (printing == "true") {
        printerData.isPrinting = true;
    } else {
        printerData.isPrinting = false;
    }
    printerData.toolTemp = (const char*)(*jsonDoc)["temperature"]["tool0"]["actual"];
    printerData.toolTargetTemp = (const char*)(*jsonDoc)["temperature"]["tool0"]["target"];
    printerData.bedTemp = (const char*)(*jsonDoc)["temperature"]["bed"]["actual"];
    printerData.bedTargetTemp = (const char*)(*jsonDoc)["temperature"]["bed"]["target"];

    if (BasePrinterClientImpl::isOperational()) {
        this->debugController->printLn("Status: " + this->getStateAsText() + " " + printerData.fileName + "(" + printerData.progressCompletion + "%)");
    }
}

void OctoPrintClient::getPrinterPsuState() {
    const size_t bufferSize = 364; // according to ArduinoJson assistant
    DynamicJsonDocument *jsonDoc;

    // get the PSU state (if enabled and printer operational)
    if (pollPsu && BasePrinterClientImpl::isOperational()) {
        if (!validate()) {
            printerData.isPSUoff = false; // we do not know PSU state, so assume on.
            return;
        }
        // Req 2
        jsonDoc = this->jsonRequestClient->requestJson(
            PRINTER_REQUEST_POST,
            this->getInstanceServerTarget(),
            this->getInstanceServerPort(),
            this->encodedAuth,
            "/api/plugin/psucontrol",
            "{\"command\":\"getPSUState\"}",
            bufferSize,
            true
        );
        if (this->jsonRequestClient->getLastError() != "") {
            // we do not know PSU state, so assume on.
            printerData.isPSUoff = false;
            return;
        }
    
        String psu = (const char*)(*jsonDoc)["isPSUOn"];
        if (psu == "true") {
            printerData.isPSUoff = false; // PSU checked and is on
        } else {
            printerData.isPSUoff = true; // PSU checked and is off, set flag
        }
    } else {
        printerData.isPSUoff = false; // we are not checking PSU state, so assume on
    }
}

/**
 * We translate the avail states 
 */
int OctoPrintClient::translateState(String stateText) {
    stateText.toLowerCase();
    stateText.trim();
    if (stateText == "operational" || stateText == "ready" || stateText == "sdready") {
        return PRINTER_STATE_STANDBY;
    }
    if (stateText == "printing" || stateText == "cancelling") {
        return PRINTER_STATE_PRINTING;
    }
    if (stateText == "paused") {
        return PRINTER_STATE_PAUSED;
    }
    if (stateText == "error" || stateText == "closedorerror") {
        return PRINTER_STATE_ERROR;
    }
    return PRINTER_STATE_OFFLINE;
}
