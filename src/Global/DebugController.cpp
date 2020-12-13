#include "DebugController.h"


void DebugController::setup() {
    Serial.begin(115200);
    delay(10);
    Serial.println("Debugger started");
}

void DebugController::print(const char *data) {
    Serial.print(data);
}

void DebugController::print(String data) {
    Serial.print(data);
}

void DebugController::print(int8_t data) {
    Serial.print(data);
}

void DebugController::printF(const char *data, unsigned int uInt) {
    Serial.printf(data, uInt);
}

void DebugController::printLn(const char *data) {
    Serial.println(data);
}

void DebugController::printLn(String data) {
    Serial.println(data);
}

void DebugController::printLn(long int data) {
    Serial.println(data);
}

void DebugController::printLn(int8_t data) {
    Serial.println(data);
}