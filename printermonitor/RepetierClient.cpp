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
/* 26 December 2020 : Daniel Glock : Changed connection to Repetier Server */

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

String RepetierClient::getSubmitRequest(String apiGetData) {

  //Your Domain name with URL path or IP address with path
  String serverName = "http://" + String(myServer) + ":" + String(myPort) + String(apiGetData);
  Serial.println("My Server request: " + String(serverName));
  HTTPClient printClient;
    
  // Your IP address with path or Domain name with URL path 
  printClient.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = printClient.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = printClient.getString();
    Serial.println("My Payload: " + String(payload));
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    printerData.error = "Connection to " + String(myServer) + ":" + String(myPort) + " failed.";
  }
  // Free resources
  printClient.end();

  return payload;
}


void RepetierClient::getPrinterJobResults() {
  if (!validate()) {
    return;
  }
  //**** get the Printer Job status
  String apiGetData = "/printer/api/?a=listPrinter&apikey=" + myApiKey;
  String payload = getSubmitRequest(apiGetData);
  if (printerData.error != "") {
    Serial.println("Printer listPrinter not successful");
    return;
  }
  const size_t bufferSize = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + 710;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Parse JSON object
  
  JsonArray& root = jsonBuffer.parseArray(payload);
  int countNew = root.size();
  Serial.println("Size of root new: " + String(countNew));
  root.printTo(Serial);  
  if (!root.success()) {
    printerData.error = "Repetier Data Parsing failed - Printer Job status: " + String(myServer) + ":" + String(myPort);
    Serial.println(printerData.error);
    printerData.state = "";
    return;
  }
  Serial.println("Check JSON file");
  int inx = 0;
  int count = root.size();
  Serial.println("Size of root: " + String(count));
  for (int i = 0; i < count; i++) { // Search Printer choice
    Serial.println("Printer: " + String((const char*)root[i]["slug"]));
    if (String((const char*)root[i]["slug"]) == printerData.printerName) {
      inx = i;
      Serial.println("Printer found successfull in JSON");
      break;
    }
  }
  
  Serial.println("Write printer data to database"); // Serial.println("Printer stateList not successful");
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
    Serial.println("Printer prints");
    printerData.isPrinting = true;
  } else {
    Serial.println("Printer does not print);
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
  // Payload: {"Anycubic_i3_Mega_S":
  // {"activeExtruder":0,"debugLevel":6,"doorOpen":false,"extruder":[{"error":0,"output":47,"tempRead":204.60000610351562,"tempSet":205.00000000000000}],
  // "fans":[{"on":true,"voltage":178}],"filterFan":false,"firmware":"Marlin","firmwareURL":"","flowMultiply":100,"hasXHome":true,"hasYHome":true,"hasZHome":false,
  // "heatedBeds":[{"error":0,"output":33,"tempRead":80.000000000000000,"tempSet":80.000000000000000}],"heatedChambers":[],"layer":35,"lights":0,"numExtruder":1,"powerOn":false,"rec":true,
  // "sdcardMounted":true,"shutdownAfterPrint":false,"speedMultiply":100,"volumetric":false,"x":106.80000305175781,"y":142.85899353027344,"z":6.8000001907348633}}
  // 14:23:12.801 -> E
  apiGetData = "/printer/api/?a=stateList&apikey=" + myApiKey;
  //printClient = getSubmitRequest(apiGetData);
  payload = getSubmitRequest(apiGetData);
  if (printerData.error != "") {
    return;
  }
  const size_t bufferSize2 = 3*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(9) + 300;
  DynamicJsonBuffer jsonBuffer2(bufferSize2);

  //Parse JSON object
  JsonObject& root2 = jsonBuffer2.parseObject(payload);
  //Select printer
  JsonObject& pr2 = root2[printerData.printerName];

  if (!root2.success()) {
    Serial.println("Printer stateList not successful");
    printerData.isPrinting = false;
    printerData.toolTemp = "";
    printerData.toolTargetTemp = "";
    printerData.bedTemp = "";
    printerData.bedTargetTemp = "";
    return;
  }

  Serial.println("Printer data stored in database");
  printerData.toolTemp = (const char*) pr2["extruder"][0]["tempRead"];
  printerData.toolTargetTemp = (const char*) pr2["extruder"][0]["tempSet"];
  printerData.bedTemp = (const char*) pr2["heatedBeds"][0]["tempRead"];
  printerData.bedTargetTemp = (const char*) pr2["heatedBeds"][0]["tempSet"];

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
  printerData.error = "";
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
