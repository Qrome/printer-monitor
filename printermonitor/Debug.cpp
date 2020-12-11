#include "Debug.h"
#include <SoftwareSerial.h>


Debug::Debug() {
}

void Debug::initialize() {
  Serial.begin(115200);
  delay(10);
  Serial.println();
}

void Debug::print(char *data) {
  Serial.print(data);
}

void Debug::print(String data) {
  Serial.print(data);
}

void Debug::print(int8_t data) {
  Serial.print(data);
}

void Debug::printF(char *data, unsigned int uInt) {
  Serial.printf(data, uInt);
}

void Debug::printLn(char *data) {
  Serial.println(data);
}

void Debug::printLn(int8_t data) {
  Serial.println(data);
}

void Debug::printLn(String data) {
  Serial.println(data);
}

void Debug::printLn(long int data) {
  Serial.println(data);
}


/********************************************

void serialDebugSetup() {
#ifndef USE_NEXTION_DISPLAY
  
#endif
}

#define NOP __asm__ __volatile__ ("nop\n\t")

#ifndef USE_NEXTION_DISPLAY
#define serialDebugPrint(data) 
#define serialDebugPrintF(data, args) Serial.printf(data, args)
#define serialDebugPrintLn(data) Serial.println(data)
#else
#define serialDebugPrint(data) NOP
#define serialDebugPrintLn(data) NOP
#endif
*/
