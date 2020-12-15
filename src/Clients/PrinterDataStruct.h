#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <base64.h>

#define PRINTER_STATE_OFFLINE       (int)-2
#define PRINTER_STATE_ERROR         (int)-1
#define PRINTER_STATE_STANDBY       (int)0
#define PRINTER_STATE_PRINTING      (int)1
#define PRINTER_STATE_PAUSED        (int)2
#define PRINTER_STATE_COMPLETED     (int)3

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
    int state;
    String toolTemp;
    String toolTargetTemp;
    String filamentLength;
    String bedTemp;
    String bedTargetTemp;
    boolean isPrinting;
    boolean isPSUoff;
    String error;
    String printerName;
} PrinterDataStruct;