#include "DebugController.h"

DebugController::DebugController(bool enabled) {
    this->enabled = enabled;
}

bool DebugController::isEnabled() {
    return this->enabled;
}

void DebugController::setup() {
    if (!this->isEnabled()) {
        return;
    }
    Serial.begin(115200);
    delay(10);
    Serial.println("Debugger started");
}

void DebugController::print(const char *data) {
    if (!this->isEnabled()) {
        return;
    }
    Serial.print(data);
}

void DebugController::print(String data) {
    if (!this->isEnabled()) {
        return;
    }
    Serial.print(data);
}

void DebugController::print(int8_t data) {
    if (!this->isEnabled()) {
        return;
    }
    Serial.print(data);
}

void DebugController::printF(const char *data, unsigned int uInt) {
    if (!this->isEnabled()) {
        return;
    }
    Serial.printf(data, uInt);
}

void DebugController::printLn(const char *data) {
    if (!this->isEnabled()) {
        return;
    }
    Serial.println(data);
}

void DebugController::printLn(String data) {
    if (!this->isEnabled()) {
        return;
    }
    Serial.println(data);
}

void DebugController::printLn(long int data) {
    if (!this->isEnabled()) {
        return;
    }
    Serial.println(data);
}

void DebugController::printLn(int8_t data) {
    if (!this->isEnabled()) {
        return;
    }
    Serial.println(data);
}