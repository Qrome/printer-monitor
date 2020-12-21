#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <base64.h>
#include "Debug.h"
#include "BasePrinterClientImpl.h"
#include "../Global/GlobalDataController.h"

/**
 * @brief REPETIER Client implementation
 */
class RepetierClient : public BasePrinterClientImpl {
public:
    RepetierClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient);
    void getPrinterJobResults(PrinterDataStruct *printerData) override;
    void getPrinterPsuState(PrinterDataStruct *printerData) override;
    boolean clientNeedApiKey() override { return true; };

private:    
    static int translateState(String stateText);
};