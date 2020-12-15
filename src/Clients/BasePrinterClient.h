#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <base64.h>
#include "Debug.h"
#include "../Network/JsonRequestClient.h"

#define PRINTER_STATE_OFFLINE       (int)-2
#define PRINTER_STATE_ERROR         (int)-1
#define PRINTER_STATE_STANDBY       (int)0
#define PRINTER_STATE_PRINTING      (int)1
#define PRINTER_STATE_PAUSED        (int)2
#define PRINTER_STATE_COMPLETED     (int)3

class BasePrinterClient {
public:
  virtual void getPrinterJobResults();
  virtual void getPrinterPsuState();
  virtual void updatePrintClient();

  virtual String getAveragePrintTime() = 0;
  virtual String getEstimatedPrintTime() = 0;
  virtual String getFileName() = 0;
  virtual String getFileSize() = 0;
  virtual String getLastPrintTime() = 0;
  virtual String getProgressCompletion() = 0;
  virtual String getProgressFilepos() = 0;
  virtual String getProgressPrintTime() = 0;
  virtual String getProgressPrintTimeLeft() = 0;
  virtual int getState() = 0;
  virtual String getStateAsText() = 0;
  virtual boolean isPrinting() = 0;
  virtual boolean isOperational() = 0;
  virtual boolean isPSUoff() = 0;
  virtual String getTempBedActual() = 0;
  virtual String getTempBedTarget() = 0;
  virtual String getTempToolActual() = 0;
  virtual String getTempToolTarget() = 0;
  virtual String getFilamentLength() = 0;
  virtual String getValueRounded(String value) = 0;
  virtual String getError() = 0;
  virtual String getPrinterType() = 0;
  virtual int getPrinterPort() = 0;
  virtual String getPrinterName() = 0;
  virtual void setPrinterName(String printer) = 0;
};