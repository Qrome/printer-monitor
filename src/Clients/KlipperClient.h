#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <base64.h>
#include "Debug.h"
#include "BasePrinterClientImpl.h"
#include "../Global/GlobalDataController.h"

class KlipperClient : public BasePrinterClientImpl {
private:
    String encodedAuth = "";
    boolean pollPsu;

    boolean validate();
    WiFiClient getSubmitRequest(String apiGetData);
    WiFiClient getPostRequest(String apiPostData, String apiPostBody);
    
    String result;
  
public:
    KlipperClient(GlobalDataController *globalDataController, DebugController *debugController);

    void getPrinterJobResults() override;
    void getPrinterPsuState() override;
    void updatePrintClient() override;
};