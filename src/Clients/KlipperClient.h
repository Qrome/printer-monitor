#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <base64.h>
#include "Debug.h"
#include "BasePrinterClient.h"
#include "../Global/GlobalDataController.h"

class KlipperClient : public BasePrinterClient {
private:
    char myServer[100];
    int myPort = 7125;
    String myApiKey = "";
    String encodedAuth = "";
    boolean pollPsu;
    const String printerType = "Klipper";

    void resetPrintData();
    boolean validate();
    WiFiClient getSubmitRequest(String apiGetData);
    WiFiClient getPostRequest(String apiPostData, String apiPostBody);
    
    String result;

    typedef struct {
        String averagePrintTime;
        String estimatedPrintTime;
        String fileName;
        String fileSize;
        String lastPrintTime;
        String progressCompletion;
        String progressFilepos;
        String progressPrintTime;
        String progressPrintTimeLeft;
        String state;
        String toolTemp;
        String toolTargetTemp;
        String filamentLength;
        String bedTemp;
        String bedTargetTemp;
        boolean isPrinting;
        boolean isPSUoff;
        String error;
        String printerName;
    } PrinterStruct;

    PrinterStruct printerData;
    GlobalDataController *globalDataController;
    DebugController *debugController;
  
public:
    KlipperClient(GlobalDataController *globalDataController, DebugController *debugController);
    void getPrinterJobResults();
    void getPrinterPsuState();
    void updatePrintClient();

    String getAveragePrintTime();
    String getEstimatedPrintTime();
    String getFileName();
    String getFileSize();
    String getLastPrintTime();
    String getProgressCompletion();
    String getProgressFilepos();
    String getProgressPrintTime();
    String getProgressPrintTimeLeft();
    String getState();
    boolean isPrinting();
    boolean isOperational();
    boolean isPSUoff();
    String getTempBedActual();
    String getTempBedTarget();
    String getTempToolActual();
    String getTempToolTarget();
    String getFilamentLength();
    String getValueRounded(String value);
    String getError();
    String getPrinterType();
    int getPrinterPort();
    String getPrinterName();
    void setPrinterName(String printer);
};