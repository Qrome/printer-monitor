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
/* 12/7/20 : Matthias Grimm : Add Klipper Monitor */

#include "KlipperClient.h"

KlipperClient::KlipperClient(String ApiKey, String server, int port, String user, String pass, boolean psu, Debug *debugHandle) {
  this->debugHandle = debugHandle;
  updatePrintClient(ApiKey, server, port, user, pass, psu);
}

void KlipperClient::updatePrintClient(String ApiKey, String server, int port, String user, String pass, boolean psu) {
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

boolean KlipperClient::validate() {
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

WiFiClient KlipperClient::getSubmitRequest(String apiGetData) {
  WiFiClient printClient;
  printClient.setTimeout(5000);

  this->debugHandle->printLn("Getting Klipper Data via GET");
  this->debugHandle->printLn(apiGetData);
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
      this->debugHandle->printLn("Connection to " + String(myServer) + ":" + String(myPort) + " failed.");
      this->debugHandle->printLn("");
      resetPrintData();
      printerData.error = "Connection to " + String(myServer) + ":" + String(myPort) + " failed.";
      return printClient;
    }
  } 
  else {
    this->debugHandle->printLn("Connection to Klipper failed: " + String(myServer) + ":" + String(myPort)); //error message if no client connect
    this->debugHandle->printLn("");
    resetPrintData();
    printerData.error = "Connection to Klipper failed: " + String(myServer) + ":" + String(myPort);
    return printClient;
  }

  // Check HTTP status
  char status[32] = {0};
  printClient.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0 && strcmp(status, "HTTP/1.1 409 CONFLICT") != 0) {
    this->debugHandle->print("Unexpected response: ");
    this->debugHandle->printLn(status);
    printerData.state = "";
    printerData.error = "Response: " + String(status);
    return printClient;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!printClient.find(endOfHeaders)) {
    this->debugHandle->printLn("Invalid response");
    printerData.error = "Invalid response from " + String(myServer) + ":" + String(myPort);
    printerData.state = "";
  }

  return printClient;
}

WiFiClient KlipperClient::getPostRequest(String apiPostData, String apiPostBody) {
  WiFiClient printClient;
  printClient.setTimeout(5000);

  this->debugHandle->printLn("Getting Klipper Data via POST");
  this->debugHandle->printLn(apiPostData + " | " + apiPostBody);
  result = "";
  if (printClient.connect(myServer, myPort)) {  //starts client connection, checks for connection
    printClient.println(apiPostData);
    printClient.println("Host: " + String(myServer) + ":" + String(myPort));
    printClient.println("Connection: close");
    printClient.println("X-Api-Key: " + myApiKey);
    if (encodedAuth != "") {
      printClient.print("Authorization: ");
      printClient.println("Basic " + encodedAuth);
    }
    printClient.println("User-Agent: ArduinoWiFi/1.1");
    printClient.println("Content-Type: application/json");
    printClient.print("Content-Length: ");
    printClient.println(apiPostBody.length());
    printClient.println();
    printClient.println(apiPostBody);
    if (printClient.println() == 0) {
      this->debugHandle->printLn("Connection to " + String(myServer) + ":" + String(myPort) + " failed.");
      this->debugHandle->printLn("");
      resetPrintData();
      printerData.error = "Connection to " + String(myServer) + ":" + String(myPort) + " failed.";
      return printClient;
    }
  } 
  else {
    this->debugHandle->printLn("Connection to Klipper failed: " + String(myServer) + ":" + String(myPort)); //error message if no client connect
    this->debugHandle->printLn("");
    resetPrintData();
    printerData.error = "Connection to Klipper failed: " + String(myServer) + ":" + String(myPort);
    return printClient;
  }

  // Check HTTP status
  char status[32] = {0};
  printClient.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0 && strcmp(status, "HTTP/1.1 409 CONFLICT") != 0) {
    this->debugHandle->print("Unexpected response: ");
    this->debugHandle->printLn(status);
    printerData.state = "";
    printerData.error = "Response: " + String(status);
    return printClient;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!printClient.find(endOfHeaders)) {
    this->debugHandle->printLn("Invalid response");
    printerData.error = "Invalid response from " + String(myServer) + ":" + String(myPort);
    printerData.state = "";
  }

  return printClient;
}

void KlipperClient::getPrinterJobResults() {
  if (!validate()) {
    return;
  }
  //**** get the Printer Job status
  String apiGetData = "GET /printer/objects/query?heater_bed&extruder&print_stats&toolhead&display_status";
  WiFiClient printClient = getSubmitRequest(apiGetData);
  if (printerData.error != "") {
    return;
  }
  const size_t bufferSize = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + 710;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(printClient);
  if (!root.success()) {
    this->debugHandle->printLn("Klipper Data Parsing failed: " + String(myServer) + ":" + String(myPort));
    printerData.error = "Klipper Data Parsing failed: " + String(myServer) + ":" + String(myPort);
    printerData.state = "";
    return;
  }
  
  printerData.averagePrintTime = (const char*)root["result"]["status"]["print_stats"]["averagePrintTime"];
  printerData.estimatedPrintTime = (const char*)root["result"]["status"]["print_stats"]["estimatedPrintTime"];
  printerData.fileName = (const char*)root["result"]["status"]["print_stats"]["filename"];
  printerData.fileSize = (const char*)root["job"]["file"]["size"];
  printerData.lastPrintTime = (const char*)root["job"]["lastPrintTime"];
  printerData.progressCompletion = (const char*)root["result"]["status"]["display_status"]["progress"];
  printerData.progressFilepos = (const char*)root["result"]["status"]["virtual_sdcard"]["file_position"];
  printerData.progressPrintTime = (const char*)root["result"]["status"]["print_stats"]["print_duration"];
  printerData.progressPrintTimeLeft = (const char*)root["progress"]["printTimeLeft"];
  printerData.filamentLength = (const char*)root["result"]["status"]["job"]["print_stats"]["filament_used"];
  printerData.state = (const char*)root["result"]["status"]["print_stats"]["state"];

  if (isOperational()) {
    this->debugHandle->printLn("Status: " + printerData.state);
  } else {
    this->debugHandle->printLn("Printer Not Operational");
  }

  //**** get the Printer Temps and Stat
  apiGetData = "GET /printer/objects/query?heater_bed&extruder&print_stats";
  printClient = getSubmitRequest(apiGetData);
  if (printerData.error != "") {
    return;
  }
  const size_t bufferSize2 = 3*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(9) + 300;
  DynamicJsonBuffer jsonBuffer2(bufferSize2);

  // Parse JSON object
  JsonObject& root2 = jsonBuffer2.parseObject(printClient);
  if (!root2.success()) {
    printerData.isPrinting = false;
    printerData.toolTemp = "";
    printerData.toolTargetTemp = "";
    printerData.bedTemp = "";
    printerData.bedTargetTemp = (const char*)root2["result"]["status"]["heater_bed"]["target"];
    return;
  }

  String printing = (const char*)root2["result"]["status"]["print_stats"]["state"];
  if (printing == "printing") {
    printerData.isPrinting = true;
  } else {
    printerData.isPrinting = false;
  }
  printerData.toolTemp = (const char*)root2["result"]["status"]["extruder"]["temperature"];
  printerData.toolTargetTemp = (const char*)root2["result"]["status"]["extruder"]["target"];
  printerData.bedTemp = (const char*)root2["result"]["status"]["heater_bed"]["temperature"];
  printerData.bedTargetTemp = (const char*)root2["result"]["status"]["heater_bed"]["target"];

  if (isPrinting()) {
    this->debugHandle->printLn("Status: " + printerData.state + " " + printerData.fileName + "(" + printerData.progressCompletion + "%)");
  }
}

void KlipperClient::getPrinterPsuState() {
  //**** get the PSU state (if enabled and printer operational)
  if (pollPsu && isOperational()) {
    if (!validate()) {
      printerData.isPSUoff = false; // we do not know PSU state, so assume on.
      return;
    }
    String apiPostData = "POST /api/plugin/psucontrol HTTP/1.1";
    String apiPostBody = "{\"command\":\"getPSUState\"}";
    WiFiClient printClient = getPostRequest(apiPostData,apiPostBody);
    if (printerData.error != "") {
      printerData.isPSUoff = false; // we do not know PSU state, so assume on.
      return;
    }
    const size_t bufferSize3 = JSON_OBJECT_SIZE(2) + 300;
    DynamicJsonBuffer jsonBuffer3(bufferSize3);
  
    // Parse JSON object
    JsonObject& root3 = jsonBuffer3.parseObject(printClient);
    if (!root3.success()) {
      printerData.isPSUoff = false; // we do not know PSU state, so assume on
      return;
    }
  
    String psu = (const char*)root3["isPSUOn"];
    if (psu == "true") {
      printerData.isPSUoff = false; // PSU checked and is on
    } else {
      printerData.isPSUoff = true; // PSU checked and is off, set flag
    }
    printClient.stop(); //stop client
  } else {
    printerData.isPSUoff = false; // we are not checking PSU state, so assume on
  }
}

// Reset all PrinterData
void KlipperClient::resetPrintData() {
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

String KlipperClient::getAveragePrintTime(){
  return printerData.averagePrintTime;
}

String KlipperClient::getEstimatedPrintTime() {
  return printerData.estimatedPrintTime;
}

String KlipperClient::getFileName() {
  return printerData.fileName;
}

String KlipperClient::getFileSize() {
  return printerData.fileSize;
}

String KlipperClient::getLastPrintTime(){
  return printerData.lastPrintTime;
}

String KlipperClient::getProgressCompletion() {
  return String(printerData.progressCompletion.toInt());
}

String KlipperClient::getProgressFilepos() {
  return printerData.progressFilepos;  
}

String KlipperClient::getProgressPrintTime() {
  return printerData.progressPrintTime;
}

String KlipperClient::getProgressPrintTimeLeft() {
  String rtnValue = printerData.progressPrintTimeLeft;
  if (getProgressCompletion() == "100") {
    rtnValue = "0"; // Print is done so this should be 0 this is a fix for Klipper
  }
  return rtnValue;
}

String KlipperClient::getState() {
  return printerData.state;
}

boolean KlipperClient::isPrinting() {
  return printerData.isPrinting;
}

boolean KlipperClient::isPSUoff() {
  return printerData.isPSUoff;
}

boolean KlipperClient::isOperational() {
  boolean operational = false;
  if (printerData.state == "standby" || isPrinting()) {
    operational = true;
  }
  return operational;
}

String KlipperClient::getTempBedActual() {
  return printerData.bedTemp;
}

String KlipperClient::getTempBedTarget() {
  return printerData.bedTargetTemp;
}

String KlipperClient::getTempToolActual() {
  return printerData.toolTemp;
}

String KlipperClient::getTempToolTarget() {
  return printerData.toolTargetTemp;
}

String KlipperClient::getFilamentLength() {
  return printerData.filamentLength;
}

String KlipperClient::getError() {
  return printerData.error;
}

String KlipperClient::getValueRounded(String value) {
  float f = value.toFloat();
  int rounded = (int)(f+0.5f);
  return String(rounded);
}

String KlipperClient::getPrinterType() {
  return printerType;
}

int KlipperClient::getPrinterPort() {
  return myPort;
}

String KlipperClient::getPrinterName() {
  return printerData.printerName;
}

void KlipperClient::setPrinterName(String printer) {
  printerData.printerName = printer;
}
