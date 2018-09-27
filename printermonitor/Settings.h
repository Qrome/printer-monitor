/** The MIT License (MIT)

Copyright (c) 2018 David Payne

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

/******************************************************************************
 * Printer Monitor is designed for the Wemos D1 ESP8266
 * Wemos D1 Mini:  https://amzn.to/2qLyKJd
 * 0.96" OLED I2C 128x64 Display (12864) SSD1306
 * OLED Display:  https://amzn.to/2JDEAUF
 ******************************************************************************/
/******************************************************************************
 * NOTE: The settings here are the default settings for the first loading.  
 * After loading you will manage changes to the settings via the Web Interface.  
 * If you want to change settings again in the settings.h, you will need to 
 * erase the file system on the Wemos or use the “Reset Settings” option in 
 * the Web Interface.
 ******************************************************************************/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "TimeClient.h"
#include "OctoPrintClient.h"
#include "OpenWeatherMapClient.h"
#include "WeatherStationFonts.h"
#include "FS.h"
#include "SH1106Wire.h"
#include "SSD1306Wire.h"
#include "OLEDDisplayUi.h"

//******************************
// Start Settings
//******************************

// OctoPrint Monitoring -- Monitor your 3D printer OctoPrint Server
String OctoPrintApiKey = "";   // ApiKey from your User Account on OctoPrint
String OctoPrintHostName = "octopi";// Default 'octopi' -- or hostname if different (optional if your IP changes)
String OctoPrintServer = "";   // IP or Address of your OctoPrint Server (DO NOT include http://)
int OctoPrintPort = 80;        // the port you are running your OctoPrint server on (usually 80);
String OctoAuthUser = "";      // only used if you have haproxy or basic athentintication turned on (not default)
String OctoAuthPass = "";      // only used with haproxy or basic auth (only needed if you must authenticate)

// Weather Configuration
boolean DISPLAYWEATHER = true; // true = show weather when not printing / false = no weather
String WeatherApiKey = ""; // Your API Key from http://openweathermap.org/
// Default City Location (use http://openweathermap.org/find to find city ID)
int CityIDs[] = { 5304391 }; //Only USE ONE for weather marquee
boolean IS_METRIC = false; // false = Imperial and true = Metric

const int WEBSERVER_PORT = 80; // The port you can access this device on over HTTP
const boolean WEBSERVER_ENABLED = true;  // Device will provide a web interface via http://[ip]:[port]/
char* www_username = "admin";  // User account for the Web Interface
char* www_password = "password";  // Password for the Web Interface
float UtcOffset = -7; // Hour offset from GMT for your timezone
boolean IS_24HOUR = false;     // 23:00 millitary 24 hour clock
int minutesBetweenDataRefresh = 15;
boolean DISPLAYCLOCK = true;   // true = Show Clock when not printing / false = turn off display when not printing

// Display Settings
const int I2C_DISPLAY_ADDRESS = 0x3c; // I2C Address of your Display (usually 0x3c or 0x3d)
const int SDA_PIN = D2;
const int SCL_PIN = D5;
boolean INVERT_DISPLAY = false; // true = pins at top | false = pins at the bottom
//#define DISPLAY_SH1106       // Uncomment this line to use the SH1106 display -- SSD1306 is used by default and is most common

boolean ENABLE_OTA = true;     // this will allow you to load firmware to the device over WiFi (see OTA for ESP8266)
String OTA_Password = "";      // Set an OTA password here -- leave blank if you don't want to be prompted for password
//******************************
// End Settings
//******************************

String themeColor = "light-green"; // this can be changed later in the web interface.
