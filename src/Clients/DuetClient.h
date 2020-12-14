#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <base64.h>
#include "Debug.h"
#include "BasePrinterClientImpl.h"

class DuetClient : public BasePrinterClientImpl {
private:
    String myApiKey = "";
    String encodedAuth = "";
    boolean pollPsu;

    boolean validate();
    WiFiClient getSubmitRequest(String apiGetData);
    WiFiClient getPostRequest(String apiPostData, String apiPostBody);
    
    String result;
  
public:
    DuetClient(GlobalDataController *globalDataController, DebugController *debugController);

    void getPrinterJobResults() override;
    void getPrinterPsuState() override;
    void updatePrintClient() override;
};