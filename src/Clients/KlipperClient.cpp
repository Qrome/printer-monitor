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




void KlipperClient::getPrinterJobResults() {
    const size_t bufferSize = JSON_ARRAY_SIZE(4) + JSON_OBJECT_SIZE(1) + 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(9) + 426;
    DynamicJsonDocument *jsonDoc;
    if (!validate()) {
        return;
    }

    // Req 1
    this->debugController->printLn("Get Klipper Data: " + this->getInstanceServerTarget() + ":" + String(this->getInstanceServerPort()));
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        this->getInstanceServerTarget(),
        this->getInstanceServerPort(),
        this->encodedAuth,
        "/printer/objects/query?print_stats",
        "",
        bufferSize,
        true
    );
    printerData.state = (const char*)(*jsonDoc)["result"]["status"]["print_stats"]["state"];
    printerData.filamentLength = (const char*)(*jsonDoc)["result"]["status"]["job"]["print_stats"]["filament_used"];
    printerData.progressPrintTime = (const char*)(*jsonDoc)["result"]["status"]["print_stats"]["print_duration"];
    printerData.fileName = (const char*)(*jsonDoc)["result"]["status"]["print_stats"]["filename"];

    if (printerData.state == "printing") {
        printerData.isPrinting = true;
    } else {
        printerData.isPrinting = false;
    }
    if (BasePrinterClientImpl::isOperational()) {
        this->debugController->printLn("Status: " + printerData.state);
    } else {
        this->debugController->printLn("Printer Not Operational");
    }

    // Req 2
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        this->getInstanceServerTarget(),
        this->getInstanceServerPort(),
        this->encodedAuth,
        "/printer/objects/query?heater_bed&extruder&display_status",
        "",
        bufferSize,
        true
    );
    if (this->jsonRequestClient->getLastError() != "") {
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

    printerData.progressCompletion = (int)(*jsonDoc)["result"]["status"]["display_status"]["progress"];
    printerData.toolTemp = (int)(*jsonDoc)["result"]["status"]["extruder"]["temperature"];
    printerData.toolTargetTemp = (int)(*jsonDoc)["result"]["status"]["extruder"]["target"];
    printerData.bedTemp = (int)(*jsonDoc)["result"]["status"]["heater_bed"]["temperature"];
    printerData.bedTargetTemp = (int)(*jsonDoc)["result"]["status"]["heater_bed"]["target"];
 
    // Req 3
    jsonDoc = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        this->getInstanceServerTarget(),
        this->getInstanceServerPort(),
        this->encodedAuth,
        "/printer/objects/query?toolhead&virtual_sdcard",
        "",
        bufferSize,
        true
    );
    if (this->jsonRequestClient->getLastError() != "") {
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

    float fileProgress = (float)(*jsonDoc)["result"]["status"]["virtual_sdcard"]["progress"];
    printerData.progressFilepos = (const char*)(*jsonDoc)["result"]["status"]["virtual_sdcard"]["file_position"];
    printerData.estimatedPrintTime = (float)(*jsonDoc)["result"]["status"]["toolhead"]["estimated_print_time"];

    /*
    printerData.progressPrintTimeLeft : No metadata is available, print duration and progress can be used to calculate the ETA:
    */
    float totalPrintTime = printerData.progressPrintTime.toFloat() / fileProgress;
    printerData.progressPrintTimeLeft = String(totalPrintTime - printerData.progressPrintTime.toFloat());
    




//     // &webhooks&virtual_sdcard&print_stats
    

    /*
    
    printerData.averagePrintTime = (int)jsonBuffer["result"]["status"]["toolhead"]["averagePrintTime"];
    // printerData.fileSize = (const char*)jsonBuffer["job"]["file"]["size"];
    printerData.lastPrintTime = (const char*)jsonBuffer["job"]["lastPrintTime"];
    ;
    
    
    */
/**
printerData.progressPrintTimeLeft : 
If no metadata is available, print duration and progress can be used to calculate the ETA:

// assume "result" is the response from the status query
let vsd = result.status.virtual_sdcard;
let pstats = result.status.print_stats;
let total_time = pstats.print_duration / vsd.progress;
let eta = total_time - pstats.print_duration; */


    /*

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
    
    
    printerData.fileSize = (long)jsonBuffer2["result"]["size"];

    */

    if (BasePrinterClientImpl::isOperational()) {
        this->debugController->printLn("Status: " + printerData.state + " " + printerData.fileName + "(" + printerData.progressCompletion + "%)");
    }
}

void KlipperClient::getPrinterPsuState() {
    /*// get the PSU state (if enabled and printer operational)
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
    } */
}
