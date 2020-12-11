#include "NextionDisplayClient.h"

NextionDisplayClient::NextionDisplayClient(byte rxPin, byte txPin, PrinterClientInterface *printerClient) {
  this->printerClient = printerClient;
  this->softwareSerialPort = new SoftwareSerial(rxPin, txPin);
  this->softwareSerialPort->begin(9600);
}

void NextionDisplayClient::postSetup() {
}

void NextionDisplayClient::showPageInit(String printerTypeName, String softwareVersion) {
  String command("version.txt=");
  command += "\"for " + printerTypeName + " V" + softwareVersion + "\"";
  this->sendCommand("page 0");
  this->sendCommand(command.c_str());
}

void NextionDisplayClient::showPageApMode(String protalSsid) {
  String commandHostName("WifiHostname.txt=");
  String commandQrCode("WifiScancode.txt=");
  commandHostName += "\"" + protalSsid + "\"";
  commandQrCode += "\"WIFI:S:" + protalSsid + ";T:WPA;P:;;\"";
  this->sendCommand("page 1");
  this->sendCommand(commandHostName.c_str());
  this->sendCommand(commandQrCode.c_str());
}

void NextionDisplayClient::showPageWeather() {
  this->sendCommand("page 2");
}

void NextionDisplayClient::showPagePrinter() {
  this->sendCommand("page 3");
}




void NextionDisplayClient::sendCommand(const char* cmd) {
  while (this->softwareSerialPort->available())
  {
    this->softwareSerialPort->read();
  }
    
  this->softwareSerialPort->print(cmd);
  this->softwareSerialPort->write(0xFF);
  this->softwareSerialPort->write(0xFF);
  this->softwareSerialPort->write(0xFF);
}
