/**
 * PrinterMonitor Configuration file
 */

#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Clients/Clients.h"

/**
 * Basic software settings
 */
#define VERSION "4.0"
#define HOSTNAME "PrintMon-" 
#define CONFIG "/conf.txt"

//===========================================================================
//========================= MCU & Display config ============================
//===========================================================================

/**
 * MCU Selector
 */
#define USE_ESP8266_NODEMCU

// Display Settings
//#define USE_NEXTION_DISPLAY
#ifdef USE_NEXTION_DISPLAY
    #ifdef USE_ESP8266_NODEMCU
        #define DISPLAY_TX_PIN                  5
        #define DISPLAY_RX_PIN                  4
    #else
        #define DISPLAY_TX_PIN                  D1
        #define DISPLAY_RX_PIN                  D2
    #endif
#else
    // I2C Address of your Display (usually 0x3c or 0x3d)
    #define DISPLAY_I2C_DISPLAY_ADDRESS         0x3c
    #ifdef USE_ESP8266_NODEMCU
        #define DISPLAY_SDA_PIN                 4
        #define DISPLAY_SCL_PIN                 5
    #else
        #define DISPLAY_SDA_PIN                 D1
        #define DISPLAY_SCL_PIN                 D2
    #endif
  
    // true = pins at top | false = pins at the bottom
    #define DISPLAY_INVERT_DISPLAY              true
    // true = to use the SH1106 display | false = SSD1306 is used by default and is most common
    #define DISPLAY_SH1106                      true
#endif

/**
 * LED Settings
 */

// LED will always flash on bootup or Wifi Errors
#define EXTERNAL_LED            LED_BUILTIN
// true = System LED will Flash on Service Calls; false = disabled LED flashing
#define USE_FLASH               true

//===========================================================================
//========================== Client for requests ============================
//===========================================================================

/**
 * Printer client selection
 * Use one of following (defined in Clients/Clients.h):
 *          DUET_CLIENT
 *          KLIPPER_CLIENT
 *          REPETIER_CLIENT
 *          OCTOPRINT_CLIENT
 */
#define PRINTERCLIENT   KLIPPER_CLIENT

// Default: ApiKey from your User Account on client endpoint
#define PRINTERCLIENT_APIKEY        ""
// Default: HostName from your client endpoint
#define PRINTERCLIENT_HOSTNAME      ""
// Default: server ip (withot http://) for client endpoint
#define PRINTERCLIENT_SERVER        ""
// Default: The port you are running your client server on (usually 80)
#define PRINTERCLIENT_PORT          80
// Default: Only used if you have haproxy or basic athentintication turned on (not default)
#define PRINTERCLIENT_AUTHUSER      ""
// Default: Only used with haproxy or basic auth (only needed if you must authenticate)
#define PRINTERCLIENT_AUTHPASS      ""
// Default: Set to true if https://github.com/kantlivelong/OctoPrint-PSUControl/ in use
#define PRINTERCLIENT_HASPSU        false

//===========================================================================
//=========================== Webserver config ==============================
//===========================================================================

// Device will provide a web interface via http://[ip]:[port]/
#define WEBSERVER_ENABLED           true
// The port you can access this device on over HTTP
#define WEBSERVER_PORT              80
// true = require athentication to change configuration settings / false = no auth
#define WEBSERVER_IS_BASIC_AUTH     true
// User account for the Web Interface
#define WEBSERVER_USERNAME          "admin"
// Password for the Web Interface
#define WEBSERVER_PASSWORD          "password"
// this can be changed later in the web interface.
#define WEBSERVER_THEMECOLOR        "light-green"

//===========================================================================
//========================== Time service config ============================
//===========================================================================

// true = Show Clock when not printing / false = turn off display when not printing
#define DISPLAYCLOCK                true
// Hour offset from GMT for your timezone
#define TIME_UTCOFFSET              -6
// 23:00 millitary 24 hour clock
#define TIME_IS_24HOUR              false
// Minutes between resync with time server
#define TIME_RESYNC_MINUTES_DELAY   15

//===========================================================================
//============================ Weather config ===============================
//===========================================================================

// true = show weather when not printing / false = no weather
#define DISPLAYWEATHER              true
// Your API Key from http://openweathermap.org/
#define WEATHER_APIKEY              ""
// Default City Location (use http://openweathermap.org/find to find city ID)
#define WEATHER_CITYID              4923482
// false = Imperial and true = Metric
#define WEATHER_METRIC              false
// Languages: ar, bg, ca, cz, de, el, en, fa, fi, fr, gl, hr, hu, it, ja, kr, la, lt, mk, nl, pl, pt, ro, ru, se, sk, sl, es, tr, ua, vi, zh_cn, zh_tw
#define WEATHER_LANGUAGE            "en"

//===========================================================================
//============================= OTA config ==================================
//===========================================================================

// this will allow you to load firmware to the device over WiFi (see OTA for ESP8266)
#define ENABLE_OTA          true
// Set an OTA password here -- leave blank if you don't want to be prompted for password
#define OTA_Password        ""
