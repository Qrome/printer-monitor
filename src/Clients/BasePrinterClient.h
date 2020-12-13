/**
 * @file BasePrinterClient.h
 * @author Robert Stein
 * @brief Basic implementation for all implemented printer 
 *        Clients
 * @version 0.1
 * @date 2020-12-12
 * 
 * @copyright Copyright (c) 2020
  */
#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "../Global/GlobalDataController.h"

class BasePrinterClient {
public:
  virtual String getAveragePrintTime() = 0;
  virtual String getEstimatedPrintTime() = 0;
  virtual String getFileName() = 0;
  virtual String getFileSize() = 0;
  virtual String getLastPrintTime() = 0;
  virtual String getProgressCompletion() = 0;
  virtual String getProgressFilepos() = 0;
  virtual String getProgressPrintTime() = 0;
  virtual String getProgressPrintTimeLeft() = 0;
  virtual String getState() = 0;
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