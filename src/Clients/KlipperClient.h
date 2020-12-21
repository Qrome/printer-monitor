#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <base64.h>
#include "Debug.h"
#include "BasePrinterClientImpl.h"
#include "../Global/GlobalDataController.h"

/**
 * @brief KLIPPER Client implementation
 */
class KlipperClient : public BasePrinterClientImpl {
public:
    KlipperClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient);
    void getPrinterJobResults(PrinterDataStruct *printerData) override;
    void getPrinterPsuState(PrinterDataStruct *printerData) override;
    boolean clientNeedApiKey() override { return false; };

private:    
    static int translateState(String stateText);
};