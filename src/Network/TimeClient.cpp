#include "TimeClient.h"

TimeClient::TimeClient(float utcOffset, DebugController * debugController) {
    this->myUtcOffset = utcOffset;
    this->debugController = debugController;
}

bool TimeClient::handleSync(int snycDelayMinutes) {
    //Get Time Update
    if((this->getMinutesFromLast(this->lastEpoch) >= snycDelayMinutes) || this->lastEpoch == 0) {
        this->debugController->printLn("Updating Time...");
        this->updateTime();
        this->lastEpoch = this->getCurrentEpoch();
        this->debugController->printLn("Local time: " + this->getAmPmFormattedTime());
        return true;
    }
    return false;
}

int TimeClient::getMinutesFromLast(long lastEpochToUse) {
    return this->getSecondsFromLast(lastEpochToUse) / 60;
}

int TimeClient::getSecondsFromLast(long lastEpochToUse) {
    int seconds = (this->getCurrentEpoch() - lastEpochToUse);
    return seconds;
}

void TimeClient::resetLastEpoch() {
    this->lastEpoch = 0;
}

long TimeClient::getLastEpoch() {
    return this->lastEpoch;
}

void TimeClient::updateTime() {
    WiFiClient client;
    
    if (!client.connect(ntpServerName, httpPort)) {
        this->debugController->printLn("connection failed");
        return;
    }

    // This will send the request to the server
    client.print(String("GET / HTTP/1.1\r\n") +
                 String("Host: www.google.com\r\n") +
                 String("Connection: close\r\n\r\n"));
    int repeatCounter = 0;
    while(!client.available() && repeatCounter < 10) {
        delay(1000);
        this->debugController->printLn(".");
        repeatCounter++;
    }

    String line;

    int size = 0;
    client.setNoDelay(false);
    while(client.connected()) {
        while((size = client.available()) > 0) {
            line = client.readStringUntil('\n');
            line.toUpperCase();
            // example:
            // date: Thu, 19 Nov 2015 20:25:40 GMT
            if (line.startsWith("DATE: ")) {
                this->debugController->printLn(line.substring(23, 25) + ":" + line.substring(26, 28) + ":" +line.substring(29, 31));
                int parsedHours = line.substring(23, 25).toInt();
                int parsedMinutes = line.substring(26, 28).toInt();
                int parsedSeconds = line.substring(29, 31).toInt();
                this->debugController->printLn(String(parsedHours) + ":" + String(parsedMinutes) + ":" + String(parsedSeconds));

                localEpoc = (parsedHours * 60 * 60 + parsedMinutes * 60 + parsedSeconds);
                this->debugController->printLn(localEpoc);
                localMillisAtUpdate = millis();
                client.stop();
            }
        }
    }
}

void TimeClient::setUtcOffset(float utcOffset) {
	myUtcOffset = utcOffset;
}

String TimeClient::getHours() {
    if (localEpoc == 0) {
      return "--";
    }
    int hours = ((getCurrentEpochWithUtcOffset()  % 86400L) / 3600) % 24;
    if (hours < 10) {
      return "0" + String(hours);
    }
    return String(hours); // print the hour (86400 equals secs per day)

}

int TimeClient::getMinutesNumber() {
    if (localEpoc == 0) {
      return 0;
    }
    return ((getCurrentEpochWithUtcOffset() % 3600) / 60);
}

String TimeClient::getMinutes() {
    if (localEpoc == 0) {
      return "--";
    }
    int minutes = ((getCurrentEpochWithUtcOffset() % 3600) / 60);
    if (minutes < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      return "0" + String(minutes);
    }
    return String(minutes);
}

int TimeClient::getSecondsNumber() {
    if (localEpoc == 0) {
      return 0;
    }
    return getCurrentEpochWithUtcOffset() % 60;
}

String TimeClient::getSeconds() {
    if (localEpoc == 0) {
      return "--";
    }
    int seconds = getCurrentEpochWithUtcOffset() % 60;
    if ( seconds < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      return "0" + String(seconds);
    }
    return String(seconds);
}

String TimeClient::getAmPmHours() {
	int hours = getHours().toInt();
	if (hours >= 13) {
		hours = hours - 12;
	}
	if (hours == 0) {
		hours = 12;
	}
	return String(hours);
}

String TimeClient::getAmPm() {
	int hours = getHours().toInt();
	String ampmValue = "AM";
	if (hours >= 12) {
		ampmValue = "PM";
	}
	return ampmValue;
}

String TimeClient::getFormattedTime() {
    return getHours() + ":" + getMinutes() + ":" + getSeconds();
}

String TimeClient::getAmPmFormattedTime() {
	return getAmPmHours() + ":" + getMinutes() + " " + getAmPm();
}

long TimeClient::getCurrentEpoch() {
    return localEpoc + ((millis() - localMillisAtUpdate) / 1000);
}

long TimeClient::getCurrentEpochWithUtcOffset() {
    return (long)round(getCurrentEpoch() + 3600 * myUtcOffset + 86400L) % 86400L;
}