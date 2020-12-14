// Moonraker API: https://github.com/Arksine/moonraker/blob/master/docs/web_api.md
// ArduinoJSON Assistant: https://arduinojson.org/v6/assistant/

#include "KlipperClient.h"

KlipperClient::KlipperClient(GlobalDataController *globalDataController, DebugController *debugController) {
    this->globalDataController = globalDataController;
    this->debugController = debugController;
    this->updatePrintClient();
}

void KlipperClient::updatePrintClient() {
    this->globalDataController->getPrinterHostName().toCharArray(this->myServer, 100);
    this->globalDataController->getPrinterServer().toCharArray(this->myServerIp, 30);
    this->myPort = this->globalDataController->getPrinterPort();
    encodedAuth = "";
    if (this->globalDataController->getPrinterAuthUser() != "") {
        String userpass = this->globalDataController->getPrinterAuthUser() + ":" + this->globalDataController->getPrinterAuthPass();
        base64 b64;
        encodedAuth = b64.encode(userpass, true);
    }
    pollPsu = this->globalDataController->hasPrinterPsu();
}

boolean KlipperClient::validate() {
    boolean rtnValue = false;
    printerData.error = "";
    if ((String(this->myServer) == "") && (String(this->myServerIp) == "")) {
        printerData.error += "Server address or host name is required; ";
    }
    if (printerData.error == "") {
        rtnValue = true;
    }
    return rtnValue;
}

WiFiClient KlipperClient::getSubmitRequest(String apiGetData) {
    WiFiClient printClient;
    printClient.setTimeout(5000);
    const char *targetServer = this->myServerIp;
    if (String(this->myServerIp) == "") {
        targetServer = this->myServer;
    }

    this->debugController->printLn("Getting Klipper Data via GET");
    this->debugController->printLn(apiGetData);
    result = "";
    if (printClient.connect(targetServer, myPort)) {  //starts client connection, checks for connection
        printClient.println(apiGetData);
        printClient.println("Host: " + String(targetServer) + ":" + String(myPort));
        if (encodedAuth != "") {
        printClient.print("Authorization: ");
        printClient.println("Basic " + encodedAuth);
        }
        printClient.println("User-Agent: ArduinoWiFi/1.1");
        printClient.println("Connection: close");
        if (printClient.println() == 0) {
        this->debugController->printLn("Connection to " + String(targetServer) + ":" + String(myPort) + " failed.");
        this->debugController->printLn("");
        resetPrintData();
        printerData.error = "Connection to " + String(targetServer) + ":" + String(myPort) + " failed.";
        return printClient;
        }
    } 
    else {
        this->debugController->printLn("Connection to Klipper failed: " + String(targetServer) + ":" + String(myPort)); //error message if no client connect
        this->debugController->printLn("");
        resetPrintData();
        printerData.error = "Connection to Klipper failed: " + String(targetServer) + ":" + String(myPort);
        return printClient;
    }

    // Check HTTP status
    char status[32] = {0};
    printClient.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0 && strcmp(status, "HTTP/1.1 409 CONFLICT") != 0) {
        this->debugController->printLn("Unexpected response: ");
        this->debugController->printLn(status);
        printerData.state = "";
        printerData.error = "Response: " + String(status);
        return printClient;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!printClient.find(endOfHeaders)) {
        this->debugController->printLn("Invalid response");
        printerData.error = "Invalid response from " + String(targetServer) + ":" + String(myPort);
        printerData.state = "";
    }

    return printClient;
}

WiFiClient KlipperClient::getPostRequest(String apiPostData, String apiPostBody) {
    WiFiClient printClient;
    printClient.setTimeout(5000);
    const char *targetServer = this->myServerIp;
    if (String(this->myServerIp) == "") {
        targetServer = this->myServer;
    }

    this->debugController->printLn("Getting Klipper Data via POST");
    this->debugController->printLn(apiPostData + " | " + apiPostBody);
    result = "";
    if (printClient.connect(targetServer, myPort)) {  //starts client connection, checks for connection
        printClient.println(apiPostData);
        printClient.println("Host: " + String(targetServer) + ":" + String(myPort));
        printClient.println("Connection: close");
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
            this->debugController->printLn("Connection to " + String(targetServer) + ":" + String(myPort) + " failed.");
            this->debugController->printLn("");
            resetPrintData();
            printerData.error = "Connection to " + String(targetServer) + ":" + String(myPort) + " failed.";
            return printClient;
        }
    } 
    else {
        this->debugController->printLn("Connection to Klipper failed: " + String(targetServer) + ":" + String(myPort)); //error message if no client connect
        this->debugController->printLn("");
        resetPrintData();
        printerData.error = "Connection to Klipper failed: " + String(targetServer) + ":" + String(myPort);
        return printClient;
    }

    // Check HTTP status
    char status[32] = {0};
    printClient.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0 && strcmp(status, "HTTP/1.1 409 CONFLICT") != 0) {
        this->debugController->print("Unexpected response: ");
        this->debugController->printLn(status);
        printerData.state = "";
        printerData.error = "Response: " + String(status);
        return printClient;
    }

    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!printClient.find(endOfHeaders)) {
        this->debugController->printLn("Invalid response");
        printerData.error = "Invalid response from " + String(targetServer) + ":" + String(myPort);
        printerData.state = "";
    }

    return printClient;
}

void KlipperClient::getPrinterJobResults() {
    if (!validate()) {
        return;
    }
    const char *targetServer = this->myServerIp;
    if (String(this->myServerIp) == "") {
        targetServer = this->myServer;
    }

    //**** get the Printer Job status
    String apiGetData = "GET /printer/objects/query?heater_bed&extruder&webhooks&virtual_sdcard&print_stats&toolhead&display_status";
    WiFiClient printClient = getSubmitRequest(apiGetData);
    if (printerData.error != "") {
        return;
    }
    const size_t bufferSize = JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(1) + 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(9) + 426;
    DynamicJsonDocument jsonBuffer(bufferSize);

    // Parse JSON object
    DeserializationError error = deserializeJson(jsonBuffer, printClient);
    if (error) {
        this->debugController->printLn("Klipper Data Parsing failed: " + String(targetServer) + ":" + String(myPort));
        printerData.error = "Klipper Data Parsing failed: " + String(targetServer) + ":" + String(myPort);
        printerData.state = "";
        printerData.isPrinting = false;
        printerData.toolTemp = "";
        printerData.toolTargetTemp = "";
        printerData.bedTemp = "";
        printerData.bedTargetTemp = (const char*)jsonBuffer["result"]["status"]["heater_bed"]["target"];
        return;
        return;
    }
    
    printerData.averagePrintTime = (int)jsonBuffer["result"]["status"]["toolhead"]["averagePrintTime"];
    printerData.estimatedPrintTime = (float)jsonBuffer["result"]["status"]["toolhead"]["estimated_print_time"];
    printerData.fileName = (const char*)jsonBuffer["result"]["status"]["print_stats"]["filename"];
    // printerData.fileSize = (const char*)jsonBuffer["job"]["file"]["size"];
    printerData.lastPrintTime = (const char*)jsonBuffer["job"]["lastPrintTime"];
    printerData.progressCompletion = (int)jsonBuffer["result"]["status"]["display_status"]["progress"];
    printerData.progressFilepos = (const char*)jsonBuffer["result"]["status"]["virtual_sdcard"]["file_position"];
    printerData.progressPrintTime = (const char*)jsonBuffer["result"]["status"]["print_stats"]["print_duration"];
    printerData.progressPrintTimeLeft = (const char*)jsonBuffer["progress"]["printTimeLeft"];
    printerData.filamentLength = (const char*)jsonBuffer["result"]["status"]["job"]["print_stats"]["filament_used"];
    printerData.state = (const char*)jsonBuffer["result"]["status"]["print_stats"]["state"];
/**
printerData.progressPrintTimeLeft : 
If no metadata is available, print duration and progress can be used to calculate the ETA:

// assume "result" is the response from the status query
let vsd = result.status.virtual_sdcard;
let pstats = result.status.print_stats;
let total_time = pstats.print_duration / vsd.progress;
let eta = total_time - pstats.print_duration; */


    if (isOperational()) {
        this->debugController->printLn("Status: " + printerData.state);
    } else {
        this->debugController->printLn("Printer Not Operational");
    }

    //**** get the fileseize
    apiGetData = "GET /server/files/metadata?filename=" + printerData.fileName;
    printClient = getSubmitRequest(apiGetData);
    if (printerData.error != "") {
        return;
    }
    const size_t bufferSize2 = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(8) + 128;
    DynamicJsonDocument jsonBuffer2(bufferSize2);
    
    // Parse JSON object
    DeserializationError error2 = deserializeJson(jsonBuffer2, printClient);
    if (error2) {
        printerData.isPrinting = false;
        printerData.toolTemp = "";
        printerData.toolTargetTemp = "";
        printerData.bedTemp = "";
        printerData.bedTargetTemp = (int)jsonBuffer["result"]["status"]["heater_bed"]["target"];
        return;
    }

    String printing = (const char*)jsonBuffer["result"]["status"]["print_stats"]["state"];
    if (printing == "printing") {
        printerData.isPrinting = true;
    } else {
        printerData.isPrinting = false;
    }
    printerData.toolTemp = (int)jsonBuffer["result"]["status"]["extruder"]["temperature"];
    printerData.toolTargetTemp = (int)jsonBuffer["result"]["status"]["extruder"]["target"];
    printerData.bedTemp = (int)jsonBuffer["result"]["status"]["heater_bed"]["temperature"];
    printerData.bedTargetTemp = (int)jsonBuffer["result"]["status"]["heater_bed"]["target"];
    printerData.fileSize = (long)jsonBuffer2["result"]["size"];

    if (isPrinting()) {
        this->debugController->printLn("Status: " + printerData.state + " " + printerData.fileName + "(" + printerData.progressCompletion + "%)");
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
        DynamicJsonDocument jsonBuffer(bufferSize3);
    
        // Parse JSON object
        DeserializationError error = deserializeJson(jsonBuffer, printClient);
        if (error) {
            printerData.isPSUoff = false; // we do not know PSU state, so assume on
            return;
        }
    
        String psu = (const char*)jsonBuffer["isPSUOn"];
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