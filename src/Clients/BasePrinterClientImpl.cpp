#include "BasePrinterClientImpl.h"

/**
 * @brief Construct a new Base Printer Client Impl:: Base Printer Client Impl object
 * 
 * @param clientType            Client type name
 * @param globalDataController  Handle to global data controller
 * @param debugController       Handle to debug controller
 * @param jsonRequestClient     Handle to json request instance
 */
BasePrinterClientImpl::BasePrinterClientImpl(
    String clientType,
    GlobalDataController *globalDataController,
    DebugController *debugController,
    JsonRequestClient *jsonRequestClient
) {
    this->globalDataController = globalDataController;
    this->debugController = debugController;
    this->clientType = clientType;
    this->jsonRequestClient = jsonRequestClient;
}

/**
 * @brief Get the type of client for this instance
 * @return String 
 */
String BasePrinterClientImpl::getClientType() {
    return this->clientType;
}

/**
 * @brief Update data for client, like AUTH
 * @param printerData       Handle to printer struct
 */
void BasePrinterClientImpl::updatePrintClient(PrinterDataStruct *printerData) {
    if (printerData->basicAuthNeeded) {
        String encodedAuth = "";
        String userpass = String(printerData->basicAuthUsername) + ":" + String(printerData->basicAuthPassword);
        base64 b64;
        MemoryHelper::stringToChar(b64.encode(userpass, true), printerData->encAuth, 120);
    }
}

/**
 * @brief Validate configuration for printer
 * @param printerData       Handle to printer struct
 * @return boolean 
 */
boolean BasePrinterClientImpl::isValidConfig(PrinterDataStruct *printerData) {
    boolean rtnValue = true;
    if ((String(printerData->remoteAddress) == "") || (String(printerData->remotePort) == "")) {
        MemoryHelper::stringToChar("Server address or host name is required", printerData->error, 120);
        rtnValue = false;
    }
    if (String(printerData->error) != "") {
        printerData->state = PRINTER_STATE_ERROR;
        rtnValue = false;
    }
    return rtnValue;
}

/**
 * @brief Check if printer is pperational
 * @param printerData       Handle to printer struct
 * @return boolean 
 */
boolean BasePrinterClientImpl::isOperational(PrinterDataStruct *printerData) {
    boolean operational = false;
    if ((printerData->state != PRINTER_STATE_OFFLINE) || printerData->isPrinting) {
        operational = true;
    }
    return operational;
}