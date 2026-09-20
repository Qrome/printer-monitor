/**The MIT License (MIT)

Copyright (c) 2015 by Daniel Eichhorn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
Modified by David Payne for use in the Scrolling Marquee
*/

#include "TimeClient.h"

TimeClient::TimeClient(float utcOffset) {
  myUtcOffset = utcOffset;
}

void TimeClient::updateTime() {
  WiFiClient client;
  
  if (!client.connect(ntpServerName, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // This will send the request to the server
  client.print(String("GET / HTTP/1.1\r\n") +
               String("Host: www.google.com\r\n") +
               String("Connection: close\r\n\r\n"));
  int repeatCounter = 0;
  while(!client.available() && repeatCounter < 10) {
    delay(1000);
    Serial.println(".");
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
        Serial.println(line.substring(23, 25) + ":" + line.substring(26, 28) + ":" +line.substring(29, 31));
        int parsedHours = line.substring(23, 25).toInt();
        int parsedMinutes = line.substring(26, 28).toInt();
        int parsedSeconds = line.substring(29, 31).toInt();
        Serial.println(String(parsedHours) + ":" + String(parsedMinutes) + ":" + String(parsedSeconds));

        localEpoc = (parsedHours * 60 * 60 + parsedMinutes * 60 + parsedSeconds);
        Serial.println(localEpoc);
        localMillisAtUpdate = millis();

        dstOffsetHours = 0;
        if (observeDst) {
          int comma = line.indexOf(',');
          if (comma > 0) {
            int idx = comma + 1;
            while (idx < (int)line.length() && line.charAt(idx) == ' ') {
              idx++;
            }
            int parsedDay = line.substring(idx).toInt();
            while (idx < (int)line.length() && line.charAt(idx) != ' ') {
              idx++;
            }
            while (idx < (int)line.length() && line.charAt(idx) == ' ') {
              idx++;
            }
            String monthName = line.substring(idx, idx + 3);
            idx += 4;
            int parsedYear = line.substring(idx).toInt();
            int parsedMonth = monthFromName(monthName);
            if (parsedYear > 2000 && parsedMonth > 0 && isUsDaylightSaving(parsedYear, parsedMonth, parsedDay, parsedHours)) {
              dstOffsetHours = 1;
            }
            Serial.println("Date: " + String(parsedYear) + "-" + String(parsedMonth) + "-" + String(parsedDay) + " DST=" + String(dstOffsetHours));
          }
        }
        client.stop();
      }
    }
  }

}

void TimeClient::setUtcOffset(float utcOffset) {
	myUtcOffset = utcOffset;
}

void TimeClient::setObserveDst(boolean observe) {
  observeDst = observe;
}

int TimeClient::monthFromName(String monthName) {
  if (monthName == "JAN") return 1;
  if (monthName == "FEB") return 2;
  if (monthName == "MAR") return 3;
  if (monthName == "APR") return 4;
  if (monthName == "MAY") return 5;
  if (monthName == "JUN") return 6;
  if (monthName == "JUL") return 7;
  if (monthName == "AUG") return 8;
  if (monthName == "SEP") return 9;
  if (monthName == "OCT") return 10;
  if (monthName == "NOV") return 11;
  if (monthName == "DEC") return 12;
  return 0;
}

int TimeClient::dayOfWeek(int year, int month, int day) {
  static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  int y = year;
  if (month < 3) {
    y -= 1;
  }
  return (y + y / 4 - y / 100 + y / 400 + t[month - 1] + day) % 7;
}

int TimeClient::nthSunday(int year, int month, int n) {
  int dow1 = dayOfWeek(year, month, 1);
  int firstSunday = (dow1 == 0) ? 1 : (8 - dow1);
  return firstSunday + ((n - 1) * 7);
}

boolean TimeClient::isUsDaylightSaving(int year, int month, int day, int utcHour) {
  if (month < 3 || month > 11) {
    return false;
  }
  if (month > 3 && month < 11) {
    return true;
  }

  int startDay = nthSunday(year, 3, 2);
  int endDay = nthSunday(year, 11, 1);
  int startUtcHour = (int)round(2.0 - myUtcOffset);
  int endUtcHour = (int)round(2.0 - (myUtcOffset + 1));

  if (month == 3) {
    if (day < startDay) return false;
    if (day > startDay) return true;
    return utcHour >= startUtcHour;
  }

  if (day < endDay) return true;
  if (day > endDay) return false;
  return utcHour < endUtcHour;
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
  return (long)round(getCurrentEpoch() + 3600 * (myUtcOffset + dstOffsetHours) + 86400L) % 86400L;
}
