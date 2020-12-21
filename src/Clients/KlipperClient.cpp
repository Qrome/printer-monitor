// Moonraker API: https://github.com/Arksine/moonraker/blob/master/docs/web_api.md
// ArduinoJSON Assistant: https://arduinojson.org/v6/assistant/

#include "KlipperClient.h"

/**
 * @brief Construct a new Klipper Client:: Klipper Client object
 * 
 * @param globalDataController  Handle to global data controller
 * @param debugController       Handle to debug controller
 * @param jsonRequestClient     Handle to json request instance
 */
KlipperClient::KlipperClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient)
: BasePrinterClientImpl("Klipper", globalDataController, debugController, jsonRequestClient) {
}

/**
 * @brief Refresh job and state data for printer
 * @param printerData       Handle to printer struct
 */
void KlipperClient::getPrinterJobResults(PrinterDataStruct *printerData) {
    const size_t bufferSize = 1536; // according to ArduinoJson assistant
    DynamicJsonDocument *jsonDoc;
    if (!this->isValidConfig(printerData)) {
        return;
    }

    // Req 1
    this->debugController->printLn("Get Klipper Data: " + String(printerData->remoteAddress) + ":" + String(printerData->remotePort));
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        String(printerData->remoteAddress),
        printerData->remotePort,
        String(printerData->encAuth),
        "/printer/objects/query?print_stats",
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

    printerData->state = KlipperClient::translateState((const char*)(*jsonDoc)["result"]["status"]["print_stats"]["state"]);
    //printerData.filamentLength = (const char*)(*jsonDoc)["result"]["status"]["job"]["print_stats"]["filament_used"];
    //printerData.progressPrintTime = (const char*)(*jsonDoc)["result"]["status"]["print_stats"]["print_duration"];
    //printerData.fileName = (const char*)(*jsonDoc)["result"]["status"]["print_stats"]["filename"];

    if (this->isOperational(printerData)) {
        this->debugController->printLn("Status: " + this->globalDataController->getPrinterStateAsText(printerData));
    } else {
        this->debugController->printLn("Printer Not Operational");
    }

    if (printerData->state == PRINTER_STATE_PRINTING) {
        printerData->isPrinting = true;
    } else {
        // We dont printing, so abort function here
        printerData->isPrinting = false;
        return;
    }

    // Req 2
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        String(printerData->remoteAddress),
        printerData->remotePort,
        String(printerData->encAuth),
        "/printer/objects/query?heater_bed&extruder&display_status&toolhead&virtual_sdcard",
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

    //printerData.progressCompletion = (int)(*jsonDoc)["result"]["status"]["display_status"]["progress"];
    printerData->toolTemp = (int)(*jsonDoc)["result"]["status"]["extruder"]["temperature"];
    printerData->toolTargetTemp = (int)(*jsonDoc)["result"]["status"]["extruder"]["target"];
    printerData->bedTemp = (int)(*jsonDoc)["result"]["status"]["heater_bed"]["temperature"];
    printerData->bedTargetTemp = (int)(*jsonDoc)["result"]["status"]["heater_bed"]["target"];
    float fileProgress = (float)(*jsonDoc)["result"]["status"]["virtual_sdcard"]["progress"];
    //printerData.progressFilepos = (const char*)(*jsonDoc)["result"]["status"]["virtual_sdcard"]["file_position"];
    printerData->estimatedPrintTime = (float)(*jsonDoc)["result"]["status"]["toolhead"]["estimated_print_time"];

    /*
    printerData.progressPrintTimeLeft : No metadata is available, print duration and progress can be used to calculate the ETA:
    */
    //float totalPrintTime = printerData.progressPrintTime.toFloat() / fileProgress;
    //printerData.progressPrintTimeLeft = String(totalPrintTime - printerData.progressPrintTime.toFloat());
    
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
void KlipperClient::getPrinterPsuState(PrinterDataStruct *printerData)  {
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
