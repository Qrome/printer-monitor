#pragma once
#include "BasePrinterClient.h"
#include "../Global/GlobalDataController.h"

/**
 * @brief Basic implementations for an printer client with needed data
 */
class BasePrinterClientImpl : public BasePrinterClient {
protected:
    GlobalDataController *globalDataController;
    DebugController *debugController;
    JsonRequestClient *jsonRequestClient;
    String clientType = "Octoprint";
    
public:
    BasePrinterClientImpl(String clientType, GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient);
    void getPrinterJobResults(PrinterDataStruct *printerData) {};
    void getPrinterPsuState(PrinterDataStruct *printerData) {};
    void updatePrintClient(PrinterDataStruct *printerData);
    String getClientType();
    boolean isOperational(PrinterDataStruct *printerData);
    boolean isValidConfig(PrinterDataStruct *printerData);
};