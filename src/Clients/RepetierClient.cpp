// 
// ArduinoJSON Assistant: https://arduinojson.org/v6/assistant/

#include "RepetierClient.h"

/**
 * @brief Construct a new Repetier Client:: Repetier Client object
 * 
 * @param globalDataController  Handle to global data controller
 * @param debugController       Handle to debug controller
 * @param jsonRequestClient     Handle to json request instance
 */
RepetierClient::RepetierClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient)
    : BasePrinterClientImpl("Repetier", globalDataController, debugController, jsonRequestClient) {
}

/**
 * @brief Refresh job and state data for printer
 * @param printerData       Handle to printer struct
 */
void RepetierClient::getPrinterJobResults(PrinterDataStruct *printerData) {
}

/**
 * @brief Refresh psu state data for printer
 * @param printerData       Handle to printer struct
 */
void RepetierClient::getPrinterPsuState(PrinterDataStruct *printerData)  {
}

/**
 * We translate the avail states 
 */
int RepetierClient::translateState(String stateText) {
    return PRINTER_STATE_OFFLINE;
}
