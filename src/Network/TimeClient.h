#pragma once
#include <ESP8266WiFi.h>
#include "../Global/DebugController.h"

#define NTP_PACKET_SIZE 48

class TimeClient {
private:
    float myUtcOffset = 0;
    long localEpoc = 0;
    long localMillisAtUpdate;
    const char* ntpServerName = "www.google.com";
    const int httpPort = 80;    
    byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
    DebugController * debugController;
    
    long lastEpoch = 0;
    long firstEpoch = 0;

public:
    TimeClient(float utcOffset, DebugController * debugController);
    void updateTime();
    bool handleSync(int snycDelayMinutes);
    int getMinutesFromLast(long lastEpochToUse);
    int getSecondsFromLast(long lastEpochToUse);
    void resetLastEpoch();
    long getLastEpoch();

    
    
    void setUtcOffset(float utcOffset);
    String getHours();
    String getAmPmHours();
    String getAmPm();
    int getMinutesNumber();
    String getMinutes();
    int getSecondsNumber();
    String getSeconds();
    String getFormattedTime();
    String getAmPmFormattedTime();
    long getCurrentEpoch();
    long getCurrentEpochWithUtcOffset();
};