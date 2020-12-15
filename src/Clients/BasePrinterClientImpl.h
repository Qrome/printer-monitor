#pragma once
#include "BasePrinterClient.h"
#include "../Global/GlobalDataController.h"


class BasePrinterClientImpl : public BasePrinterClient {
protected:
    GlobalDataController *globalDataController;
    DebugController *debugController;
    JsonRequestClient *jsonRequestClient;
    String printerType = "Octoprint";

    PrinterDataStruct printerData;
    String result;
    String encodedAuth = "";
    
public:
    BasePrinterClientImpl(String printerType, GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient);

    void getPrinterJobResults() {};
    void getPrinterPsuState() {};
    void updatePrintClient() {};

    void resetPrintData();
    String getAveragePrintTime();
    String getEstimatedPrintTime();
    String getFileName();
    String getFileSize();
    String getLastPrintTime();
    String getProgressCompletion();
    String getProgressFilepos();
    String getProgressPrintTime();
    String getProgressPrintTimeLeft();
    int getState();
    String getStateAsText();
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

    
protected:
    String getInstanceServerTarget();
    int getInstanceServerPort();
};