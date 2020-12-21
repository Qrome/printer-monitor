#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <base64.h>
#include "Debug.h"
#include "../Network/JsonRequestClient.h"
#include "PrinterDataStruct.h"
#include "../../include/MemoryHelper.h"

/**
 * @brief Basic function definitions for an printer client like an interface
 */
class BasePrinterClient {
public:
    virtual void getPrinterJobResults(PrinterDataStruct *printerData) = 0;
    virtual void getPrinterPsuState(PrinterDataStruct *printerData) = 0;
    virtual void updatePrintClient(PrinterDataStruct *printerData) = 0;
    virtual String getClientType() = 0;
    virtual boolean isValidConfig(PrinterDataStruct *printerData) = 0;

    /**
     * @brief Reset all dynamic variables for printer
     * @param printerData       Handle to printer struct
     */
    static void resetPrinterData(PrinterDataStruct *printerData) {
        printerData->state = PRINTER_STATE_OFFLINE;
        printerData->isPrinting = false;
        printerData->isPSUoff = false;
        printerData->averagePrintTime = 0;
        printerData->bedTargetTemp = 0.0f;
        printerData->bedTemp = 0.0f;
        MemoryHelper::stringToChar("", printerData->error, 120);
        MemoryHelper::stringToChar("", printerData->encAuth, 120);
        printerData->estimatedPrintTime = 0;
        printerData->filamentLength = 0.0f;
        MemoryHelper::stringToChar("", printerData->fileName, 60);
        printerData->fileSize = 0;
        printerData->lastPrintTime = 0;
        MemoryHelper::stringToChar("", printerData->progressCompletion, 60);
        printerData->progressFilepos = 0;
        printerData->progressPrintTime = 0;
        printerData->progressPrintTimeLeft = 0;
        printerData->toolTargetTemp = 0.0f;
        printerData->toolTemp = 0.0f;
    }
};