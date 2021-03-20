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
/* 07 April 2019 : Jon Smith : Redesigned this class for Repetier Server */

#include "RepetierClient.h"

RepetierClient::RepetierClient(String ApiKey, String server, int port, String user, String pass, boolean psu) {
  updatePrintClient(ApiKey, server, port, user, pass, psu);
}

void RepetierClient::updatePrintClient(String ApiKey, String server, int port, String user, String pass, boolean psu) {
  server.toCharArray(myServer, 100);
  myApiKey = ApiKey;
  myPort = port;
  encodedAuth = "";
  if (user != "") {
    String userpass = user + ":" + pass;
    base64 b64;
    encodedAuth = b64.encode(userpass, true);
  }
  pollPsu = psu;
}

boolean RepetierClient::validate() {
  boolean rtnValue = false;
  printerData.error = "";
  if (String(myServer) == "") {
    printerData.error += "Server address is required; ";
  }
  if (myApiKey == "") {
    printerData.error += "ApiKey is required; ";
  }
  if (printerData.error == "") {
    rtnValue = true;
  }
  return rtnValue;
}

WiFiClient RepetierClient::getSubmitRequest(String apiGetData) {
  WiFiClient printClient;
  printClient.setTimeout(5000);

  Serial.println("Getting Repetier Data via GET");
  Serial.println(apiGetData);
  result = "";
  if (printClient.connect(myServer, myPort)) {  //starts client connection, checks for connection
    printClient.println(apiGetData);
    printClient.println("Host: " + String(myServer) + ":" + String(myPort));
    printClient.println("X-Api-Key: " + myApiKey);
    if (encodedAuth != "") {
      printClient.print("Authorization: ");
      printClient.println("Basic " + encodedAuth);
    }
    printClient.println("User-Agent: ArduinoWiFi/1.1");
    printClient.println("Connection: close");
    if (printClient.println() == 0) {
      Serial.println("Connection to " + String(myServer) + ":" + String(myPort) + " failed.");
      Serial.println();
      resetPrintData();
      printerData.error = "Connection to " + String(myServer) + ":" + String(myPort) + " failed.";
      return printClient;
    }
  } 
  else {
    Serial.println("Connection to Repetier failed: " + String(myServer) + ":" + String(myPort)); //error message if no client connect
    Serial.println();
    resetPrintData();
    printerData.error = "Connection to Repetier failed: " + String(myServer) + ":" + String(myPort);
    return printClient;
  }
/*
  // Check HTTP status
  char status[32] = {0};
  printClient.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    printerData.state = "";
    printerData.error = "Response: " + String(status);
    return printClient;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!printClient.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    printerData.error = "Invalid response from " + String(myServer) + ":" + String(myPort);
    printerData.state = "";
  }
*/
  return printClient;
}


void RepetierClient::getPrinterJobResults() {
  if (!validate()) {
    return;
  }
  //**** get the Printer Job status
  String apiGetData = "GET /printer/api/?a=listPrinter&apikey=" + myApiKey;
  WiFiClient printClient = getSubmitRequest(apiGetData);
  if (printerData.error != "") {
    return;
  }
  const size_t bufferSize = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + 710;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Parse JSON object
  JsonArray& root = jsonBuffer.parseArray(printClient);
    
  if (!root.success()) {
    printerData.error = "Repetier Data Parsing failed: " + String(myServer) + ":" + String(myPort);
    Serial.println(printerData.error);
    printerData.state = "";
    return;
  }

  int inx = 0;
  int count = root.size();
  Serial.println("Size of root: " + String(count));
  for (int i = 0; i < count; i++) {
    Serial.println("Printer: " + String((const char*)root[i]["slug"]));
    if (String((const char*)root[i]["slug"]) == printerData.printerName) {
      inx = i;
      break;
    }
  }
  
  JsonObject& pr = root[inx];
  
  //printerData.averagePrintTime = (const char*)pr[""];
  printerData.estimatedPrintTime = (const char*)pr["printTime"];
  printerData.fileName = (const char*) pr["job"];
  printerData.fileSize = (const char*) pr["totalLines"];
  //printerData.filamentLength = (const char*) pr[""];
  printerData.state = (const char*) pr["online"];
  //printerData.lastPrintTime = (const char*) pr[""];
  printerData.progressCompletion = (const char*) pr["done"];
  printerData.progressFilepos = (const char*) pr["linesSend"];
  printerData.progressPrintTime = (const char*) pr["printedTimeComp"];

//Figure out Time Left
  long timeTot=0;
  long timeElap=0;
  long timeLeft=0;
  if (printerData.estimatedPrintTime != "" ) {
    timeTot = atol(pr["printTime"]);
  }
  if (printerData.progressPrintTime != "") {
    timeElap= atol(pr["printedTimeComp"]);
  }
  timeLeft = timeTot-timeElap; 
  printerData.progressPrintTimeLeft = String(timeLeft);

  if (printerData.fileName != "none") {
    printerData.isPrinting = true;
  } else {
    printerData.isPrinting = false;
  }

  if (printerData.isPrinting) {  
    Serial.println("Printing: " + printerData.fileName);
  }
  
  if (isOperational()) {
    Serial.println("Status: " + printerData.state);
  } else {
    Serial.println("Printer Not Operational");
  }

  //**** get the Printer Temps and Stat
  apiGetData = "GET /printer/api/?a=stateList&apikey=" + myApiKey;
  printClient = getSubmitRequest(apiGetData);
  if (printerData.error != "") {
    return;
  }
  const size_t bufferSize2 = 3*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(9) + 300;
  DynamicJsonBuffer jsonBuffer2(bufferSize2);

  //Parse JSON object
  JsonObject& root2 = jsonBuffer2.parseObject(printClient);

  //Select printer
  JsonObject& pr2 = root2[printerData.printerName];

  if (!root2.success()) {
    printerData.isPrinting = false;
    printerData.toolTemp = "";
    printerData.toolTargetTemp = "";
    printerData.bedTemp = "";
    printerData.bedTargetTemp = "";
    return;
  }

  printerData.toolTemp = (const char*) pr2["extruder"][0]["tempRead"];
  printerData.toolTargetTemp = (const char*) pr2["extruder"][0]["tempSet"];
  printerData.bedTemp = (const char*) pr2["heatedBeds"][0]["tempRead"];
  printerData.bedTargetTemp = (const char*) pr2["heatedBeds"][0]["tempSet"];

  // Layer & Endtime
  apiGetData = "GET /plugin/DisplayLayerProgress/values HTTP/1.1";
  printClient = getSubmitRequest(apiGetData);
  if (printerData.error != "") {
    return;
  }
  const size_t bufferSize3 = JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(10) + 550;
  DynamicJsonBuffer jsonBuffer3(bufferSize3);

  // Parse JSON object
  JsonObject& root3 = jsonBuffer3.parseObject(printClient);
  if (!root3.success()) {
    printerData.estimatedEndTime = "";
    printerData.currentLayer = "";
    return;
  }

  printerData.estimatedEndTime = (const char*)root3["print"]["estimatedEndTime"];
  printerData.currentLayer = (const char*)root3["layer"]["current"];
  printerData.totalLayers = (const char*)root3["layer"]["total"];

  if (printerData.isPrinting) {
    Serial.println("Status: " + printerData.state + " " + printerData.fileName + "(" + printerData.progressCompletion + "%)");
  }
}

void RepetierClient::getPrinterPsuState() {
  //**** get the PSU state (if enabled and printer operational)
  //Not implemented in Repetier Server AFAIK
}

// Reset all PrinterData
void RepetierClient::resetPrintData() {
  printerData.averagePrintTime = "";
  printerData.estimatedPrintTime = "";
  printerData.fileName = "";
  printerData.fileSize = "";
  printerData.lastPrintTime = "";
  printerData.progressCompletion = "";
  printerData.progressFilepos = "";
  printerData.progressPrintTime = "";
  printerData.progressPrintTimeLeft = "";
  printerData.state = "";
  printerData.toolTemp = "";
  printerData.toolTargetTemp = "";
  printerData.filamentLength = "";
  printerData.bedTemp = "";
  printerData.bedTargetTemp = "";
  printerData.isPrinting = false;
  printerData.isPSUoff = false;
  printerData.currentLayer = "";
  printerData.totalLayers = "";
  printerData.estimatedEndTime = "";
  printerData.error = "";
}

String RepetierClient::getCurrentLayer(){
  return printerData.currentLayer;
}

String RepetierClient::getTotalLayers(){
  return printerData.totalLayers;
}

String RepetierClient::getEstimatedEndTime(){
  return printerData.estimatedEndTime;
}

String RepetierClient::getAveragePrintTime(){
  return printerData.averagePrintTime;
}

String RepetierClient::getEstimatedPrintTime() {
  return printerData.estimatedPrintTime;
}

String RepetierClient::getFileName() {
  return printerData.fileName;
}

String RepetierClient::getFileSize() {
  return printerData.fileSize;
}

String RepetierClient::getLastPrintTime(){
  return printerData.lastPrintTime;
}

String RepetierClient::getProgressCompletion() {
  return String(printerData.progressCompletion.toInt());
}

String RepetierClient::getProgressFilepos() {
  return printerData.progressFilepos;  
}

String RepetierClient::getProgressPrintTime() {
  return printerData.progressPrintTime;
}

String RepetierClient::getProgressPrintTimeLeft() {
  String rtnValue = printerData.progressPrintTimeLeft;
  if (getProgressCompletion() == "100") {
    rtnValue = "0"; // Print is done so this should be 0 this is a fix for OctoPrint
  }
  return rtnValue;
}

String RepetierClient::getState() {
  String rtnValue = "Offline";
  if (printerData.state == "1") {
    rtnValue = "Operational";
  }
  return rtnValue;
}

boolean RepetierClient::isPrinting() {
  return printerData.isPrinting;
}

boolean RepetierClient::isPSUoff() {
  return printerData.isPSUoff;
}

boolean RepetierClient::isOperational() {
  boolean operational = false;
  if (printerData.state == "1" || isPrinting()) {
    operational = true;
  }
  return operational;
}

String RepetierClient::getTempBedActual() {
  String temp = printerData.bedTemp;
  temp.remove(temp.indexOf(".") + 3);
  return temp;
}

String RepetierClient::getTempBedTarget() {
  return printerData.bedTargetTemp;
}

String RepetierClient::getTempToolActual() {
  String temp = printerData.toolTemp;
  temp.remove(temp.indexOf(".") + 3);
  return temp;
}

String RepetierClient::getTempToolTarget() {
  return printerData.toolTargetTemp;
}

String RepetierClient::getFilamentLength() {
  return printerData.filamentLength;
}

String RepetierClient::getError() {
  return printerData.error;
}

String RepetierClient::getValueRounded(String value) {
  float f = value.toFloat();
  int rounded = (int)(f+0.5f);
  return String(rounded);
}

String RepetierClient::getPrinterType() {
  return printerType;
}

int RepetierClient::getPrinterPort() {
  return myPort;
}

String RepetierClient::getPrinterName() {
  return printerData.printerName;
}

void RepetierClient::setPrinterName(String printer) {
  printerData.printerName = printer;
}
