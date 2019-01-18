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

#include "OctoPrintClient.h"

OctoPrintClient::OctoPrintClient(String ApiKey, String server, int port, String user, String pass, boolean psu) {
  updateOctoPrintClient(ApiKey, server, port, user, pass, psu);
}

void OctoPrintClient::updateOctoPrintClient(String ApiKey, String server, int port, String user, String pass, boolean psu) {
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

boolean OctoPrintClient::validate() {
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

WiFiClient OctoPrintClient::getSubmitRequest(String apiGetData) {
  WiFiClient printClient;
  printClient.setTimeout(5000);

  Serial.println("Getting Octoprint Data via GET");
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
    Serial.println("Connection to OctoPrint failed: " + String(myServer) + ":" + String(myPort)); //error message if no client connect
    Serial.println();
    resetPrintData();
    printerData.error = "Connection to OctoPrint failed: " + String(myServer) + ":" + String(myPort);
    return printClient;
  }

  // Check HTTP status
  char status[32] = {0};
  printClient.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0 && strcmp(status, "HTTP/1.1 409 CONFLICT") != 0) {
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

  return printClient;
}

WiFiClient OctoPrintClient::getPostRequest(String apiPostData, String apiPostBody) {
  WiFiClient printClient;
  printClient.setTimeout(5000);

  Serial.println("Getting Octoprint Data via POST");
  Serial.println(apiPostData + " | " + apiPostBody);
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
      Serial.println("Connection to " + String(myServer) + ":" + String(myPort) + " failed.");
      Serial.println();
      resetPrintData();
      printerData.error = "Connection to " + String(myServer) + ":" + String(myPort) + " failed.";
      return printClient;
    }
  } 
  else {
    Serial.println("Connection to OctoPrint failed: " + String(myServer) + ":" + String(myPort)); //error message if no client connect
    Serial.println();
    resetPrintData();
    printerData.error = "Connection to OctoPrint failed: " + String(myServer) + ":" + String(myPort);
    return printClient;
  }

  // Check HTTP status
  char status[32] = {0};
  printClient.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0 && strcmp(status, "HTTP/1.1 409 CONFLICT") != 0) {
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

  return printClient;
}

void OctoPrintClient::getPrinterJobResults() {
  if (!validate()) {
    return;
  }
  //**** get the Printer Job status
  String apiGetData = "GET /api/job HTTP/1.1";
  WiFiClient printClient = getSubmitRequest(apiGetData);
  if (printerData.error != "") {
    return;
  }
  const size_t bufferSize = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 2*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + 710;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(printClient);
  if (!root.success()) {
    Serial.println("OctoPrint Data Parsing failed: " + String(myServer) + ":" + String(myPort));
    printerData.error = "OctoPrint Data Parsing failed: " + String(myServer) + ":" + String(myPort);
    printerData.state = "";
    return;
  }
  
  printerData.averagePrintTime = (const char*)root["job"]["averagePrintTime"];
  printerData.estimatedPrintTime = (const char*)root["job"]["estimatedPrintTime"];
  printerData.fileName = (const char*)root["job"]["file"]["name"];
  printerData.fileSize = (const char*)root["job"]["file"]["size"];
  printerData.lastPrintTime = (const char*)root["job"]["lastPrintTime"];
  printerData.progressCompletion = (const char*)root["progress"]["completion"];
  printerData.progressFilepos = (const char*)root["progress"]["filepos"];
  printerData.progressPrintTime = (const char*)root["progress"]["printTime"];
  printerData.progressPrintTimeLeft = (const char*)root["progress"]["printTimeLeft"];
  printerData.filamentLength = (const char*)root["job"]["filament"]["tool0"]["length"];
  printerData.state = (const char*)root["state"];

  if (isOperational()) {
    Serial.println("Status: " + printerData.state);
  } else {
    Serial.println("Printer Not Operational");
  }

  //**** get the Printer Temps and Stat
  apiGetData = "GET /api/printer?exclude=sd,history HTTP/1.1";
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
    printerData.bedTargetTemp = (const char*)root2["temperature"]["bed"]["target"];
    return;
  }

  String printing = (const char*)root2["state"]["flags"]["printing"];
  if (printing == "true") {
    printerData.isPrinting = true;
  }
  printerData.toolTemp = (const char*)root2["temperature"]["tool0"]["actual"];
  printerData.toolTargetTemp = (const char*)root2["temperature"]["tool0"]["target"];
  printerData.bedTemp = (const char*)root2["temperature"]["bed"]["actual"];
  printerData.bedTargetTemp = (const char*)root2["temperature"]["bed"]["target"];

  if (isPrinting()) {
    Serial.println("Status: " + printerData.state + " " + printerData.fileName + "(" + printerData.progressCompletion + "%)");
  }
}

void OctoPrintClient::getPrinterPsuState() {
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
void OctoPrintClient::resetPrintData() {
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

String OctoPrintClient::getAveragePrintTime(){
  return printerData.averagePrintTime;
}

String OctoPrintClient::getEstimatedPrintTime() {
  return printerData.estimatedPrintTime;
}

String OctoPrintClient::getFileName() {
  return printerData.fileName;
}

String OctoPrintClient::getFileSize() {
  return printerData.fileSize;
}

String OctoPrintClient::getLastPrintTime(){
  return printerData.lastPrintTime;
}

String OctoPrintClient::getProgressCompletion() {
  return String(printerData.progressCompletion.toInt());
}

String OctoPrintClient::getProgressFilepos() {
  return printerData.progressFilepos;  
}

String OctoPrintClient::getProgressPrintTime() {
  return printerData.progressPrintTime;
}

String OctoPrintClient::getProgressPrintTimeLeft() {
  String rtnValue = printerData.progressPrintTimeLeft;
  if (getProgressCompletion() == "100") {
    rtnValue = "0"; // Print is done so this should be 0 this is a fix for OctoPrint
  }
  return rtnValue;
}

String OctoPrintClient::getState() {
  return printerData.state;
}

boolean OctoPrintClient::isPrinting() {
  return printerData.isPrinting;
}

boolean OctoPrintClient::isPSUoff() {
  return printerData.isPSUoff;
}

boolean OctoPrintClient::isOperational() {
  boolean operational = false;
  if (printerData.state == "Operational" || isPrinting()) {
    operational = true;
  }
  return operational;
}

String OctoPrintClient::getTempBedActual() {
  return printerData.bedTemp;
}

String OctoPrintClient::getTempBedTarget() {
  return printerData.bedTargetTemp;
}

String OctoPrintClient::getTempToolActual() {
  return printerData.toolTemp;
}

String OctoPrintClient::getTempToolTarget() {
  return printerData.toolTargetTemp;
}

String OctoPrintClient::getFilamentLength() {
  return printerData.filamentLength;
}

String OctoPrintClient::getError() {
  return printerData.error;
}

String OctoPrintClient::getValueRounded(String value) {
  float f = value.toFloat();
  int rounded = (int)(f+0.5f);
  return String(rounded);
}
