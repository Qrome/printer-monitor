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
/* 18 Feb 2022 : Robert von Könemann @vknmnn : Add Compatibility for Moonraker */


#include "MoonrakerClient.h"

MoonrakerClient::MoonrakerClient(String ApiKey, String server, int port, String user, String pass, boolean psu) {
  updatePrintClient(ApiKey, server, port, user, pass, psu);
}

void MoonrakerClient::updatePrintClient(String ApiKey, String server, int port, String user, String pass, boolean psu) {
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

boolean MoonrakerClient::validate() {
  printerData.error = "";
  if (String(myServer) == "") {
    printerData.error += "Server address is required; ";
    return false;
  }
  return true;
}

String MoonrakerClient::jsonToString(JsonVariant value) {
  if (!value.success()) {
    return "";
  }
  if (value.is<const char*>()) {
    const char* text = value.as<const char*>();
    return text ? String(text) : "";
  }
  return String((double)value);
}

String MoonrakerClient::toolLabelFromName(String extruderName) {
  if (extruderName == "extruder") {
    return "T0";
  }
  if (extruderName.startsWith("extruder")) {
    return "T" + extruderName.substring(8);
  }
  return extruderName;
}

WiFiClient MoonrakerClient::getSubmitRequest(String apiGetData) {
  WiFiClient printClient;
  printClient.setTimeout(5000);

  Serial.println("Getting Moonraker Data via GET");
  Serial.println(apiGetData);
  result = "";
  if (printClient.connect(myServer, myPort)) {  //starts client connection, checks for connection
    printClient.println(apiGetData);
    printClient.println("Host: " + String(myServer) + ":" + String(myPort));
    if (myApiKey != "") {
      printClient.println("X-Api-Key: " + myApiKey);
    }
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
    Serial.println("Connection to Moonraker failed: " + String(myServer) + ":" + String(myPort));
    Serial.println();
    resetPrintData();
    printerData.error = "Connection to Moonraker failed: " + String(myServer) + ":" + String(myPort);
    return printClient;
  }

  // Check HTTP status
  char status[32] = {0};
  printClient.readBytesUntil('\r', status, sizeof(status));
  if (strncmp(status, "HTTP/1.1 200", 12) != 0 && strncmp(status, "HTTP/1.0 200", 12) != 0 && strcmp(status, "HTTP/1.1 409 CONFLICT") != 0) {
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

WiFiClient MoonrakerClient::getPostRequest(String apiPostData, String apiPostBody) {
  WiFiClient printClient;
  printClient.setTimeout(5000);

  Serial.println("Getting Octoprint Data via POST");
  Serial.println(apiPostData + " | " + apiPostBody);
  result = "";
  if (printClient.connect(myServer, myPort)) {  //starts client connection, checks for connection
    printClient.println(apiPostData);
    printClient.println("Host: " + String(myServer) + ":" + String(myPort));
    printClient.println("Connection: close");
    if (myApiKey != "") {
      printClient.println("X-Api-Key: " + myApiKey);
    }
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
    Serial.println("Connection to Moonraker failed: " + String(myServer) + ":" + String(myPort));
    Serial.println();
    resetPrintData();
    printerData.error = "Connection to Moonraker failed: " + String(myServer) + ":" + String(myPort);
    return printClient;
  }

  // Check HTTP status
  char status[32] = {0};
  printClient.readBytesUntil('\r', status, sizeof(status));
  if (strncmp(status, "HTTP/1.1 200", 12) != 0 && strncmp(status, "HTTP/1.0 200", 12) != 0 && strcmp(status, "HTTP/1.1 409 CONFLICT") != 0) {
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

void MoonrakerClient::getPrinterJobResults() {
  if (!validate()) {
    return;
  }

  String apiGetData = "GET /printer/objects/query?print_stats=filename,state,print_duration,total_duration,filament_used&virtual_sdcard=progress,file_position,file_size&display_status=progress&heater_bed=temperature,target&toolhead=extruder&extruder=temperature,target&extruder1=temperature,target&extruder2=temperature,target&extruder3=temperature,target HTTP/1.1";
  WiFiClient printClient = getSubmitRequest(apiGetData);
  if (printerData.error != "") {
    return;
  }

  const size_t bufferSize = 2048;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(printClient);
  printClient.stop();
  if (!root.success()) {
    Serial.println("Moonraker Data Parsing failed: " + String(myServer) + ":" + String(myPort));
    printerData.error = "Moonraker Data Parsing failed: " + String(myServer) + ":" + String(myPort);
    printerData.state = "";
    return;
  }

  printerData.fileName = jsonToString(root["result"]["status"]["print_stats"]["filename"]);
  printerData.state = jsonToString(root["result"]["status"]["print_stats"]["state"]);
  printerData.averagePrintTime = jsonToString(root["result"]["status"]["print_stats"]["print_duration"]);
  printerData.progressPrintTime = jsonToString(root["result"]["status"]["print_stats"]["total_duration"]);
  printerData.filamentLength = jsonToString(root["result"]["status"]["print_stats"]["filament_used"]);
  printerData.fileSize = jsonToString(root["result"]["status"]["virtual_sdcard"]["file_size"]);
  printerData.progressFilepos = jsonToString(root["result"]["status"]["virtual_sdcard"]["file_position"]);

  String displayProgress = jsonToString(root["result"]["status"]["display_status"]["progress"]);
  String sdProgress = jsonToString(root["result"]["status"]["virtual_sdcard"]["progress"]);
  printerData.progressCompletion = (displayProgress != "") ? displayProgress : sdProgress;

  String activeExtruder = jsonToString(root["result"]["status"]["toolhead"]["extruder"]);
  if (activeExtruder == "" || activeExtruder == "null") {
    activeExtruder = "extruder";
  }
  printerData.activeTool = toolLabelFromName(activeExtruder);
  printerData.toolTemp = jsonToString(root["result"]["status"][activeExtruder.c_str()]["temperature"]);
  printerData.toolTargetTemp = jsonToString(root["result"]["status"][activeExtruder.c_str()]["target"]);
  printerData.bedTemp = jsonToString(root["result"]["status"]["heater_bed"]["temperature"]);
  printerData.bedTargetTemp = jsonToString(root["result"]["status"]["heater_bed"]["target"]);

  printerData.isPrinting = (printerData.state == "printing" || printerData.state == "paused");
  printerData.estimatedPrintTime = "";
  printerData.progressPrintTimeLeft = "";
  printerData.lastPrintTime = "";

  if (printerData.fileName != "" && printerData.fileName != "null") {
    String apiGetFileData = "GET /server/files/metadata?filename=" + urlencode(printerData.fileName) + " HTTP/1.1";
    WiFiClient printClientFile = getSubmitRequest(apiGetFileData);
    if (printerData.error == "") {
      DynamicJsonBuffer jsonBufferFile(1024);
      JsonObject& rootfile = jsonBufferFile.parseObject(printClientFile);
      printClientFile.stop();
      if (rootfile.success()) {
        printerData.estimatedPrintTime = jsonToString(rootfile["result"]["estimated_time"]);
        float estimated = printerData.estimatedPrintTime.toFloat();
        float progress = printerData.progressCompletion.toFloat();
        if (estimated > 0 && progress >= 0 && progress < 1) {
          printerData.progressPrintTimeLeft = String(estimated * (1.0 - progress));
        }
      }
    } else {
      printerData.error = "";
    }
  }

  if (isOperational()) {
    Serial.println("Status: " + printerData.state);
  } else {
    Serial.println("Printer Not Operational: " + printerData.state);
  }

  if (isPrinting()) {
    Serial.println("Status: " + printerData.state + " " + printerData.fileName + "(" + getProgressCompletion() + "%)");
  }
}

void MoonrakerClient::getPrinterPsuState() {
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
void MoonrakerClient::resetPrintData() {
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
  printerData.activeTool = "";
  printerData.isPrinting = false;
  printerData.isPSUoff = false;
  printerData.error = "";
}

String MoonrakerClient::getAveragePrintTime(){
  return printerData.averagePrintTime;
}

String MoonrakerClient::getEstimatedPrintTime() {
  return printerData.estimatedPrintTime;
}

String MoonrakerClient::getFileName() {
  return printerData.fileName;
}

String MoonrakerClient::getFileSize() {
  return printerData.fileSize;
}

String MoonrakerClient::getLastPrintTime(){
  return printerData.lastPrintTime;
}

String MoonrakerClient::getProgressCompletion() {
  return String(printerData.progressCompletion.toFloat() * 100);
}

String MoonrakerClient::getProgressFilepos() {
  return printerData.progressFilepos;
}

String MoonrakerClient::getProgressPrintTime() {
  return printerData.progressPrintTime;
}

String MoonrakerClient::getProgressPrintTimeLeft() {
  return String(printerData.estimatedPrintTime.toInt() - printerData.progressCompletion.toFloat() * printerData.estimatedPrintTime.toInt());
}

String MoonrakerClient::getState() {
  return printerData.state;
}

boolean MoonrakerClient::isPrinting() {
  return printerData.isPrinting;
}

boolean MoonrakerClient::isPSUoff() {
  return printerData.isPSUoff;
}

boolean MoonrakerClient::isOperational() {
  return printerData.state == "standby" ||
         printerData.state == "printing" ||
         printerData.state == "paused" ||
         printerData.state == "complete" ||
         printerData.state == "cancelled" ||
         printerData.state == "error" ||
         printerData.state == "Operational";
}

String MoonrakerClient::getTempBedActual() {
  return printerData.bedTemp;
}

String MoonrakerClient::getTempBedTarget() {
  return printerData.bedTargetTemp;
}

String MoonrakerClient::getTempToolActual() {
  return printerData.toolTemp;
}

String MoonrakerClient::getTempToolTarget() {
  return printerData.toolTargetTemp;
}

String MoonrakerClient::getActiveTool() {
  return printerData.activeTool;
}

String MoonrakerClient::getFilamentLength() {
  return printerData.filamentLength;
}

String MoonrakerClient::getError() {
  return printerData.error;
}

String MoonrakerClient::getValueRounded(String value) {
  float f = value.toFloat();
  int rounded = (int)(f+0.5f);
  return String(rounded);
}

String MoonrakerClient::getPrinterType() {
  return printerType;
}

int MoonrakerClient::getPrinterPort() {
  return myPort;
}

String MoonrakerClient::getPrinterName() {
  return printerData.printerName;
}

void MoonrakerClient::setPrinterName(String printer) {
  printerData.printerName = printer;
}

void MoonrakerClient::hexchar(unsigned char c, unsigned char &hex1, unsigned char &hex2) {
  hex1 = c / 16;
  hex2 = c % 16;
  hex1 += hex1 <= 9 ? '0' : 'a' - 10;
  hex2 += hex2 <= 9 ? '0' : 'a' - 10;
}

String MoonrakerClient::urlencode(String s) {
#define URLENCODE_BUF_LEN 50
  static char urlencode_buffer[URLENCODE_BUF_LEN];
  static char * buf_max = urlencode_buffer + URLENCODE_BUF_LEN - 1;
  char * buf = urlencode_buffer;
  const char *str = s.c_str();

  for (size_t i = 0, l = s.length(); i < l; i++) {

    char c = str[i];

    if ((c >= '0' && c <= '9') ||
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '-' || c == '_' || c == '.' || c == '!' || c == '~' ||
        c == '*' || c == '\'' || c == '(' || c == ')')
    {
      if (buf < buf_max) {
        *buf++ = c;
      }
    }
    else if (c == ' ') {
      if (buf < buf_max) {
        *buf++ = '+';
      }
    } else {
      if (buf < buf_max - 2) {
        *buf++ = '%';
      }
      unsigned char d1, d2;
      hexchar(c, d1, d2);
      *buf++ = d1;
      *buf++ = d2;
    }
  }
  *buf = 0;
  return String((const char*) urlencode_buffer);
}
