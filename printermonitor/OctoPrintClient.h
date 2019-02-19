/** The MIT License (MIT)

Copyright (c) 2018 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Additional Contributions:
/* 15 Jan 2019 : Owen Carter : Add psucontrol query via POST api call */

#pragma once
#include <ESP8266WiFi.h>
#include "libs/ArduinoJson/ArduinoJson.h"
#include <base64.h>

class OctoPrintClient {

private:
  char myServer[100];
  int myPort = 80;
  String myApiKey = "";
  String encodedAuth = "";
  boolean pollPsu;

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
  } PrinterStruct;

  PrinterStruct printerData;

  
public:
  OctoPrintClient(String ApiKey, String server, int port, String user, String pass, boolean psu);
  void getPrinterJobResults();
  void getPrinterPsuState();
  void updateOctoPrintClient(String ApiKey, String server, int port, String user, String pass, boolean psu);

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
};
