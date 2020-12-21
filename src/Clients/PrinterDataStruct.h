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

#define PRINTER_CLIENT_DUET         (int)0
#define PRINTER_CLIENT_KLIPPER      (int)1
#define PRINTER_CLIENT_OCTOPRINT    (int)2
#define PRINTER_CLIENT_REPETIER     (int)3

typedef struct {
    char    customName[20];
    int     apiType;
    char    apiKey[60];
    char    remoteAddress[60];
    int     remotePort;
    bool    basicAuthNeeded;
    char    basicAuthUsername[30];
    char    basicAuthPassword[60];
    bool    hasPsuControl;
    long    lastSyncEpoch;
    char    encAuth[120];
    int     averagePrintTime;
    int     estimatedPrintTime;
    char    fileName[60];
    int     fileSize;
    int     lastPrintTime;
    int     progressCompletion;
    int     progressFilepos;
    float   progressPrintTime;
    float   progressPrintTimeLeft;
    int     state;
    float   toolTemp;
    float   toolTargetTemp;
    float   filamentLength;
    float   bedTemp;
    float   bedTargetTemp;
    bool    isPrinting;
    bool    isPSUoff;
    char    error[120];    
} PrinterDataStruct;
