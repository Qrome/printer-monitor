#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>

class BaseDisplayClient {
public:
  virtual void preSetup();
  virtual void postSetup();
  virtual void handleUpdate();
  virtual void flipDisplayUpdate();
  virtual void showBootScreen();
  virtual void showApAccessScreen(String apSsid, String apIp);
  virtual void showWebserverSplashScreen(bool isEnabled);
};
