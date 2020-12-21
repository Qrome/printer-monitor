// DUET API: https://reprap.org/wiki/RepRap_Firmware_Status_responses
// ArduinoJSON Assistant: https://arduinojson.org/v6/assistant/

#include "DuetClient.h"

/**
 * @brief Construct a new Duet Client:: Duet Client object
 * 
 * @param globalDataController  Handle to global data controller
 * @param debugController       Handle to debug controller
 * @param jsonRequestClient     Handle to json request instance
 */
DuetClient::DuetClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient)
    : BasePrinterClientImpl("Duet", globalDataController, debugController, jsonRequestClient) {
}

/**
 * @brief Refresh job and state data for printer
 * @param printerData       Handle to printer struct
 */
void DuetClient::getPrinterJobResults(PrinterDataStruct *printerData) {
    const size_t bufferSize = 2048; // according to ArduinoJson assistant
    DynamicJsonDocument *jsonDoc;
    if (!this->isValidConfig(printerData)) {
        return;
    }

    // Req 1
    this->debugController->printLn("Get Duet Data: " + String(printerData->remoteAddress) + ":" + String(printerData->remotePort));
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        String(printerData->remoteAddress),
        printerData->remotePort,
        String(printerData->encAuth),
        "/rr_status?type=1",
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

    printerData->state = DuetClient::translateState((const char*)(*jsonDoc)["status"]);

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
        "/rr_status?type=3",
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

    //printerData.filamentLength = (const char*)(*jsonDoc)["result"]["status"]["job"]["print_stats"]["filament_used"];
    //printerData.progressPrintTime = (const char*)(*jsonDoc)["printDuration"];
    //printerData.fileName = (const char*)(*jsonDoc)["result"]["status"]["print_stats"]["filename"];
    //printerData.progressCompletion = (int)(*jsonDoc)["fractionPrinted"];
    printerData->toolTemp = (int)(*jsonDoc)["temps"]["current"][1];
    printerData->toolTargetTemp = (int)(*jsonDoc)["temps"]["tools"]["active"][0][0];
    printerData->bedTemp = (int)(*jsonDoc)["temps"]["bed"]["current"];
    printerData->bedTargetTemp = (int)(*jsonDoc)["temps"]["bed"]["active"];
    float fileProgress = (float)(*jsonDoc)["fractionPrinted"];
    //printerData.progressFilepos = (const char*)(*jsonDoc)["filePosition"];
    printerData->estimatedPrintTime = (float)(*jsonDoc)["file"];

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
void DuetClient::getPrinterPsuState(PrinterDataStruct *printerData)  {
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
