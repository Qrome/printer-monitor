#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <base64.h>
#include "Debug.h"
#include "BasePrinterClientImpl.h"
#include "../Global/GlobalDataController.h"

class KlipperClient : public BasePrinterClientImpl {
private:
    boolean pollPsu;

    boolean validate();
 
public:
    KlipperClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient);

    void getPrinterJobResults() override;
    void getPrinterPsuState() override;
    void updatePrintClient() override;
};