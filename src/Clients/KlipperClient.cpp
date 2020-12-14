// Moonraker API: https://github.com/Arksine/moonraker/blob/master/docs/web_api.md
// ArduinoJSON Assistant: https://arduinojson.org/v6/assistant/

#include "KlipperClient.h"

KlipperClient::KlipperClient(GlobalDataController *globalDataController, DebugController *debugController, JsonRequestClient *jsonRequestClient)
: BasePrinterClientImpl("Klipper", globalDataController, debugController, jsonRequestClient) {
    this->updatePrintClient();
}

void KlipperClient::updatePrintClient() {
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
    if ((this->globalDataController->getPrinterServer() == "") && (this->globalDataController->getPrinterHostName() == "")) {
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
    String targetServer = this->globalDataController->getPrinterServer();
    if (this->globalDataController->getPrinterServer() == "") {
        targetServer = this->globalDataController->getPrinterHostName();
    }

    this->debugController->printLn("Getting Klipper Data via GET");
    this->debugController->printLn(apiGetData);
    result = "";
    if (printClient.connect(targetServer, this->globalDataController->getPrinterPort())) {  //starts client connection, checks for connection
        printClient.println(apiGetData);
        printClient.println("Host: " + String(targetServer) + ":" + this->globalDataController->getPrinterPort());
        if (encodedAuth != "") {
        printClient.print("Authorization: ");
        printClient.println("Basic " + encodedAuth);
        }
        printClient.println("User-Agent: ArduinoWiFi/1.1");
        printClient.println("Connection: close");
        if (printClient.println() == 0) {
        this->debugController->printLn("Connection to " + targetServer + ":" + String(this->globalDataController->getPrinterPort()) + " failed.");
        this->debugController->printLn("");
        resetPrintData();
        printerData.error = "Connection to " + targetServer + ":" + String(this->globalDataController->getPrinterPort()) + " failed.";
        return printClient;
        }
    } 
    else {
        this->debugController->printLn("Connection to Klipper failed: " + targetServer + ":" + String(this->globalDataController->getPrinterPort())); //error message if no client connect
        this->debugController->printLn("");
        resetPrintData();
        printerData.error = "Connection to Klipper failed: " + targetServer + ":" + String(this->globalDataController->getPrinterPort());
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
        printerData.error = "Invalid response from " + targetServer + ":" + String(this->globalDataController->getPrinterPort());
        printerData.state = "";
    }

    return printClient;
}

WiFiClient KlipperClient::getPostRequest(String apiPostData, String apiPostBody) {
    WiFiClient printClient;
    printClient.setTimeout(5000);
    String targetServer = this->globalDataController->getPrinterServer();
    if (this->globalDataController->getPrinterServer() == "") {
        targetServer = this->globalDataController->getPrinterHostName();
    }

    this->debugController->printLn("Getting Klipper Data via POST");
    this->debugController->printLn(apiPostData + " | " + apiPostBody);
    result = "";
    if (printClient.connect(targetServer, this->globalDataController->getPrinterPort())) {  //starts client connection, checks for connection
        printClient.println(apiPostData);
        printClient.println("Host: " + targetServer + ":" + String(this->globalDataController->getPrinterPort()));
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
            this->debugController->printLn("Connection to " + targetServer + ":" + String(this->globalDataController->getPrinterPort()) + " failed.");
            this->debugController->printLn("");
            resetPrintData();
            printerData.error = "Connection to " + targetServer + ":" + String(this->globalDataController->getPrinterPort()) + " failed.";
            return printClient;
        }
    } 
    else {
        this->debugController->printLn("Connection to Klipper failed: " + targetServer + ":" + String(this->globalDataController->getPrinterPort())); //error message if no client connect
        this->debugController->printLn("");
        resetPrintData();
        printerData.error = "Connection to Klipper failed: " + targetServer + ":" + String(this->globalDataController->getPrinterPort());
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
        printerData.error = "Invalid response from " + targetServer + ":" + String(this->globalDataController->getPrinterPort());
        printerData.state = "";
    }

    return printClient;
}











void KlipperClient::getPrinterJobResults() {
    if (!validate()) {
        return;
    }

    const size_t bufferSize = JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(1) + 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(9) + 426;
    DynamicJsonDocument *jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        this->getInstanceServerTarget(),
        this->getInstanceServerPort(),
        this->encodedAuth,
        "/printer/objects/query?heater_bed&extruder&webhooks&virtual_sdcard&print_stats&toolhead&display_status",
        "",
        bufferSize,
        true
    );
    if (this->jsonRequestClient->getLastError() != "") {
        this->debugController->printLn("Get Klipper Data: " + this->getInstanceServerTarget() + ":" + String(this->getInstanceServerPort()));
        this->debugController->printLn(this->jsonRequestClient->getLastError());
        printerData.error = this->jsonRequestClient->getLastError();
        printerData.state = "";
        printerData.isPrinting = false;
        printerData.toolTemp = "";
        printerData.toolTargetTemp = "";
        printerData.bedTemp = "";
        printerData.bedTargetTemp = "";
        return;
    }
    
    

    /*
    String targetServer = this->globalDataController->getPrinterServer();
    if (this->globalDataController->getPrinterServer() == "") {
        targetServer = this->globalDataController->getPrinterHostName();
    }

    // get the Printer Job status
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
        this->debugController->printLn("Klipper Data Parsing failed: " + targetServer + ":" + String(this->globalDataController->getPrinterPort()));
        printerData.error = "Klipper Data Parsing failed: " + targetServer + ":" + String(this->globalDataController->getPrinterPort());
        printerData.state = "";
        printerData.isPrinting = false;
        printerData.toolTemp = "";
        printerData.toolTargetTemp = "";
        printerData.bedTemp = "";
        printerData.bedTargetTemp = (const char*)jsonBuffer["result"]["status"]["heater_bed"]["target"];
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
    printerData.state = (const char*)jsonBuffer["result"]["status"]["print_stats"]["state"];*/
/**
printerData.progressPrintTimeLeft : 
If no metadata is available, print duration and progress can be used to calculate the ETA:

// assume "result" is the response from the status query
let vsd = result.status.virtual_sdcard;
let pstats = result.status.print_stats;
let total_time = pstats.print_duration / vsd.progress;
let eta = total_time - pstats.print_duration; */


    /*if (BasePrinterClientImpl::isOperational()) {
        this->debugController->printLn("Status: " + printerData.state);
    } else {
        this->debugController->printLn("Printer Not Operational");
    }

    // get the fileseize
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

    if (BasePrinterClientImpl::isOperational()) {
        this->debugController->printLn("Status: " + printerData.state + " " + printerData.fileName + "(" + printerData.progressCompletion + "%)");
    }*/
}

void KlipperClient::getPrinterPsuState() {
    //**** get the PSU state (if enabled and printer operational)
    if (pollPsu && BasePrinterClientImpl::isOperational()) {
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
