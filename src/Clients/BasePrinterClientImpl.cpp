#include "BasePrinterClientImpl.h"

BasePrinterClientImpl::BasePrinterClientImpl(
    String printerType,
    GlobalDataController *globalDataController,
    DebugController *debugController,
    JsonRequestClient *jsonRequestClient
) {
    this->globalDataController = globalDataController;
    this->debugController = debugController;
    this->printerType = printerType;
    this->jsonRequestClient = jsonRequestClient;
}

// Reset all PrinterData
void BasePrinterClientImpl::resetPrintData() {
    this->printerData.averagePrintTime = "";
    this->printerData.estimatedPrintTime = "";
    this->printerData.fileName = "";
    this->printerData.fileSize = "";
    this->printerData.lastPrintTime = "";
    this->printerData.progressCompletion = "";
    this->printerData.progressFilepos = "";
    this->printerData.progressPrintTime = "";
    this->printerData.progressPrintTimeLeft = "";
    this->printerData.state = "";
    this->printerData.toolTemp = "";
    this->printerData.toolTargetTemp = "";
    this->printerData.filamentLength = "";
    this->printerData.bedTemp = "";
    this->printerData.bedTargetTemp = "";
    this->printerData.isPrinting = false;
    this->printerData.isPSUoff = false;
    this->printerData.error = "";
}

String BasePrinterClientImpl::getAveragePrintTime(){
    return printerData.averagePrintTime;
}

String BasePrinterClientImpl::getEstimatedPrintTime() {
    return printerData.estimatedPrintTime;
}

String BasePrinterClientImpl::getFileName() {
    return printerData.fileName;
}

String BasePrinterClientImpl::getFileSize() {
    return printerData.fileSize;
}

String BasePrinterClientImpl::getLastPrintTime(){
    return printerData.lastPrintTime;
}

String BasePrinterClientImpl::getProgressCompletion() {
    return String(printerData.progressCompletion.toInt());
}

String BasePrinterClientImpl::getProgressFilepos() {
    return printerData.progressFilepos;  
}

String BasePrinterClientImpl::getProgressPrintTime() {
    return printerData.progressPrintTime;
}

String BasePrinterClientImpl::getProgressPrintTimeLeft() {
    String rtnValue = printerData.progressPrintTimeLeft;
    if (getProgressCompletion() == "100") {
        rtnValue = "0"; // Print is done so this should be 0 this is a fix for Duet
    }
    return rtnValue;
}

String BasePrinterClientImpl::getState() {
    return printerData.state;
}

boolean BasePrinterClientImpl::isPrinting() {
    return printerData.isPrinting;
}

boolean BasePrinterClientImpl::isPSUoff() {
    return printerData.isPSUoff;
}

boolean BasePrinterClientImpl::isOperational() {
    boolean operational = false;
    if (printerData.state == "I" || isPrinting()) {
        operational = true;
    }
    return operational;
}

String BasePrinterClientImpl::getTempBedActual() {
    return printerData.bedTemp;
}

String BasePrinterClientImpl::getTempBedTarget() {
    return printerData.bedTargetTemp;
}

String BasePrinterClientImpl::getTempToolActual() {
    return printerData.toolTemp;
}

String BasePrinterClientImpl::getTempToolTarget() {
    return printerData.toolTargetTemp;
}

String BasePrinterClientImpl::getFilamentLength() {
    return printerData.filamentLength;
}

String BasePrinterClientImpl::getError() {
    return printerData.error;
}

String BasePrinterClientImpl::getValueRounded(String value) {
    float f = value.toFloat();
    int rounded = (int)(f+0.5f);
    return String(rounded);
}

String BasePrinterClientImpl::getPrinterType() {
    return this->printerType;
}

int BasePrinterClientImpl::getPrinterPort() {
    return this->globalDataController->getPrinterPort();
}

String BasePrinterClientImpl::getPrinterName() {
    return printerData.printerName;
}

void BasePrinterClientImpl::setPrinterName(String printer) {
    printerData.printerName = printer;
}

String BasePrinterClientImpl::getInstanceServerTarget() {
    String targetServer = this->globalDataController->getPrinterServer();
    if (this->globalDataController->getPrinterServer() == "") {
        targetServer = this->globalDataController->getPrinterHostName();
    }
    return targetServer;
}

int BasePrinterClientImpl::getInstanceServerPort() {
    return this->globalDataController->getPrinterPort();
}