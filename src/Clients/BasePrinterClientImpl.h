#pragma once
#include "BasePrinterClient.h"
#include "../Global/GlobalDataController.h"

class BasePrinterClientImpl : public BasePrinterClient {
protected:
    GlobalDataController *globalDataController;
    DebugController *debugController;
    String printerType = "Octoprint";

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
    
public:
    BasePrinterClientImpl(String printerType, GlobalDataController *globalDataController, DebugController *debugController);

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