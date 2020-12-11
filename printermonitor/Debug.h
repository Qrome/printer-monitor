#pragma once
#include <ESP8266WiFi.h>

class Debug {
public:
  Debug();
  void initialize();
  void print(char *data);
  void print(String data);
  void print(int8_t data);
  
  void printF(char *data, unsigned int uInt);
  
  void printLn(char *data);
  void printLn(String data);
  void printLn(long int data);
  void printLn(int8_t data);
};
