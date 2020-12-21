// Octoprint API: https://docs.octoprint.org/en/master/api/index.html
// ArduinoJSON Assistant: https://arduinojson.org/v6/assistant/

#include "OctoPrintClient.h"

/**
 * @brief Construct a new OctoPrint Client:: OctoPrint Client object
 * 
 * @param globalDataController  Handle to global data controller
 * @param debugController       Handle to debug controller
 * @param jsonRequestClient     Handle to json request instance
 */
OctoPrintClient::OctoPrintClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient)
    : BasePrinterClientImpl("OctoPrint", globalDataController, debugController, jsonRequestClient) {
}

/**
 * @brief Refresh job and state data for printer
 * @param printerData       Handle to printer struct
 */
void OctoPrintClient::getPrinterJobResults(PrinterDataStruct *printerData) {
    const size_t bufferSize = 1414; // according to ArduinoJson assistant
    DynamicJsonDocument *jsonDoc;
    if (!this->isValidConfig(printerData)) {
        return;
    }

    // Req 1
    this->debugController->printLn("Get OctoPrint Data: " + String(printerData->remoteAddress) + ":" + String(printerData->remotePort));
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        String(printerData->remoteAddress),
        printerData->remotePort,
        String(printerData->encAuth),
        "/api/job",
        "",
        bufferSize,
        true
    );
    if (this->jsonRequestClient->getLastError() != "") {
        this->debugController->printLn(this->jsonRequestClient->getLastError());
        BasePrinterClient::resetPrinterData(printerData);
        if (this->jsonRequestClient->getLastError().indexOf("PARSER") == 0) {
            MemoryHelper::stringToChar(this->jsonRequestClient->getLastError(), printerData->error, 120);
            printerData->state = PRINTER_STATE_ERROR;
        }
        return;
    }

    printerData->state = OctoPrintClient::translateState((const char*)(*jsonDoc)["state"]);
    //printerData.averagePrintTime = (const char*)(*jsonDoc)["job"]["averagePrintTime"];
    //printerData.estimatedPrintTime = (const char*)(*jsonDoc)["job"]["estimatedPrintTime"];
    //printerData.fileName = (const char*)(*jsonDoc)["job"]["file"]["name"];
    //printerData.fileSize = (const char*)(*jsonDoc)["job"]["file"]["size"];
    //printerData.lastPrintTime = (const char*)(*jsonDoc)["job"]["lastPrintTime"];
    //printerData.progressCompletion = (const char*)(*jsonDoc)["progress"]["completion"];
    //printerData.progressFilepos = (const char*)(*jsonDoc)["progress"]["filepos"];
    //printerData.progressPrintTime = (const char*)(*jsonDoc)["progress"]["printTime"];
    //printerData.progressPrintTimeLeft = (const char*)(*jsonDoc)["progress"]["printTimeLeft"];
    //printerData.filamentLength = (const char*)(*jsonDoc)["job"]["filament"]["tool0"]["length"];

    if (this->isOperational(printerData)) {
        this->debugController->printLn("Status: " + this->globalDataController->getPrinterStateAsText(printerData));
    } else {
        this->debugController->printLn("Printer Not Operational");
    }

    if (this->isOperational(printerData)) {
        this->debugController->printLn("Status: "
            + this->globalDataController->getPrinterStateAsText(printerData) + " "
            + String(printerData->fileName) + "("
            + String(printerData->progressCompletion) + "%)"
        );
    }
}

/**
 * @brief Refresh psu state data for printer
 * @param printerData       Handle to printer struct
 */
void OctoPrintClient::getPrinterPsuState(PrinterDataStruct *printerData)  {
    const size_t bufferSize = 364; // according to ArduinoJson assistant
    DynamicJsonDocument *jsonDoc;

    // get the PSU state (if enabled and printer operational)
    if (printerData->hasPsuControl && this->isOperational(printerData)) {
        if (!this->isValidConfig(printerData)) {
            // we do not know PSU state, so assume on.
            printerData->isPSUoff = false;
            return;
        }
        // Req 2
        jsonDoc = this->jsonRequestClient->requestJson(
            PRINTER_REQUEST_POST,
            String(printerData->remoteAddress),
            printerData->remotePort,
            String(printerData->encAuth),
            "/api/plugin/psucontrol",
            "{\"command\":\"getPSUState\"}",
            bufferSize,
            true
        );
        if (this->jsonRequestClient->getLastError() != "") {
            // we do not know PSU state, so assume on.
            printerData->isPSUoff = false;
            return;
        }
    
        String psu = (const char*)(*jsonDoc)["isPSUOn"];
        if (psu == "true") {
            printerData->isPSUoff = false; // PSU checked and is on
        } else {
            printerData->isPSUoff = true; // PSU checked and is off, set flag
        }
    } else {
        printerData->isPSUoff = false; // we are not checking PSU state, so assume on
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
