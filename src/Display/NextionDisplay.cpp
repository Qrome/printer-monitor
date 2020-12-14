#include "NextionDisplay.h"

NextionDisplay::NextionDisplay(SoftwareSerial *serialPort, GlobalDataController *globalDataController, DebugController *debugController) {
    this->debugController = debugController;
    this->globalDataController = globalDataController;
    this->serialPort = serialPort;
}

void NextionDisplay::preSetup() {
#ifdef DISPLAY_BAUDRATE
    this->serialPort->begin(DISPLAY_BAUDRATE);
#endif
}

void NextionDisplay::postSetup() {

}

void NextionDisplay::handleUpdate() {

}

void NextionDisplay::flipDisplayUpdate() {
    
}

void NextionDisplay::showBootScreen() {
    String command("version.txt=");
    command += "\"for " + this->globalDataController->getPrinterClient()->getPrinterType() + " V" + this->globalDataController->getVersion() + "\"";
    this->sendCommand("page 0");
    this->sendCommand(command.c_str());
}

void NextionDisplay::showApAccessScreen(String apSsid, String apIp) {
    String commandHostName("WifiHostname.txt=");
    String commandQrCode("WifiScancode.txt=");
    commandHostName += "\"" + apSsid + "\"";
    commandQrCode += "\"WIFI:S:" + apSsid + ";T:WPA;P:;;\"";
    this->sendCommand("page 1");
    this->sendCommand(commandHostName.c_str());
    this->sendCommand(commandQrCode.c_str());
}

void NextionDisplay::showWebserverSplashScreen(bool isEnabled) {

}










void NextionDisplay::sendCommand(String cmd) {
    this->sendCommand(cmd.c_str());
}

void NextionDisplay::sendCommand(const char* cmd) {
  while (this->serialPort->available())
  {
    this->serialPort->read();
  }
    
  this->serialPort->print(cmd);
  this->serialPort->write(0xFF);
  this->serialPort->write(0xFF);
  this->serialPort->write(0xFF);
}
