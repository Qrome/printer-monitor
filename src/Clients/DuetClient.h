#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <base64.h>
#include "Debug.h"
#include "BasePrinterClientImpl.h"
#include "../Global/GlobalDataController.h"

/**
 * @brief DUET Client implementation
 */
class DuetClient : public BasePrinterClientImpl {
public:
    DuetClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient);
    void getPrinterJobResults(PrinterDataStruct *printerData) override;
    void getPrinterPsuState(PrinterDataStruct *printerData) override;
    boolean clientNeedApiKey() override { return false; };

private:    
    static int translateState(String stateText);
};