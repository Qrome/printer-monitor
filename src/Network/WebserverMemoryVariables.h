#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "../Global/GlobalDataController.h"

/**
 * Webpage side menu right for main items
 */
static const char MENUE_ITEMS[] PROGMEM = 
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/'><i class='fa fa-home'></i> Home</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/configureprinter'><i class='fas fa-print'></i></i> Configure Printers</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/configurestation'><i class='fa fa-cog'></i> Configure Station</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/configureweather'><i class='fa fa-cloud'></i> Weather</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/systemreset' onclick='return confirm(\"Do you want to reset to default settings?\")'><i class='fa fa-undo'></i> Reset Settings</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/forgetwifi' onclick='return confirm(\"Do you want to forget to WiFi connection?\")'><i class='fa fa-wifi'></i> Forget WiFi</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/update'><i class='fa fa-wrench'></i> Firmware Update</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='https://github.com/Qrome' target='_blank'><i class='fa fa-question-circle'></i> About</a></li>";

/**
 * Basic header/footer blocks
 */
static const char HEADER_BLOCK1[] PROGMEM = "<!DOCTYPE HTML>"
        "<html><head><title>PrintBuddy</title><link rel='icon' href='data:;base64,='>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>";

static const char HEADER_BLOCK2[] PROGMEM = "<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>"
        "<link rel='stylesheet' href='https://unpkg.com/carbon-components/css/carbon-components.min.css'></style>"
        "<link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.15.1/css/all.css'>"
        "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js'></script>"
        "<style>.hidden{display:none} .bx--form-item{margin-bottom:20px} .bx--table-column-menu{width: 3.25rem}</style>"
        "</head><body>"
        "<header class='cv-header bx--header'>"
        "<a href='/' class='cv-header-name bx--header__name'>";

static const char HEADER_BLOCK3[] PROGMEM = "</a>"
        "<nav class='cv-header-nav bx--header__nav'></nav>"
        "<div class='bx--header__global'>"
            "<button type='button' class='cv-header-global-action bx--header__action' onclick='openChipInfo()'>"
                "<i class='fas fa-microchip' style='color: white; font-size: 20px;'></i>"
            "</button>"
            "<button type='button' class='cv-header-global-action bx--header__action' onclick='openSidebar()'>"
                "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='20' height='20' viewBox='0 0 32 32' aria-hidden='true'>"
                    "<path d='M14 4H18V8H14zM4 4H8V8H4zM24 4H28V8H24zM14 14H18V18H14zM4 14H8V18H4zM24 14H28V18H24zM14 24H18V28H14zM4 24H8V28H4zM24 24H28V28H24z'></path>"
                "</svg>"
            "</button>"
        "</div>"
        "<div id='sidebar' class='cv-header-panel bx--header-panel'>"
            "<ul class='cv-switcher bx--switcher__item'>";

static const char HEADER_BLOCK4[] PROGMEM = "</ul>"
        "</div>"
        "<div class='bx--toast-notification bx--toast-notification--info hidden' style='position: absolute; right: -16px; top: 40px;' id='chipinfo'>"
            "<div class='bx--toast-notification__details'>"
                "<h3 class='bx--toast-notification__title'>ESP Details</h3>"
                    "<div class='bx--toast-notification__subtitle'>";

static const char HEADER_BLOCK5[] PROGMEM = "</div>"
                "</div>"
            "</div>"
        "</header>"
        "<script>function openSidebar(){document.getElementById('sidebar').classList.toggle('bx--header-panel--expanded');document.getElementById('chipinfo').classList.add('hidden');};function openChipInfo(){document.getElementById('sidebar').classList.remove('bx--header-panel--expanded');document.getElementById('chipinfo').classList.toggle('hidden');}</script>"
        "<br><div class='bx--grid bx--grid--full-width' style='margin-top:60px'>"
            "<div class='page-header' style='margin-bottom:20px'><h4 class='page-header__label'>";

static const char FOOTER_BLOCK[] PROGMEM = "<br><br><br></div>"
        "<script src='https://unpkg.com/carbon-components/scripts/carbon-components.min.js'></script>"
    "</body>"
"</html>";

/**
 * Controls for update firmware/filesystem
 */
static const char UPDATE_FORM[] PROGMEM = "<div class='bx--row'>"
    "<div class='bx--col-md-4'>"
        "<form method='POST' action='' enctype='multipart/form-data'>"
            "<div class='cv-file-uploader cv-form-item bx--form-item'>"
                "<strong class='bx--file--label'>Update Firmware</strong>"
                "<p class='bx--label-description'>Select the firmware you want to upload</p>"
                "<div data-file='' class='bx--file'>"
                    "<label for='firmware' role='button' tabindex='0' class='bx--file-browse-btn'>"
                        "<div data-file-drop-container='' class='bx--file__drop-container'>"
                            "Drag and drop file here or upload"
                            "<input type='file' id='firmware' accept='.bin,.bin.gz' class='bx--file-input' name='firmware' onchange='document.getElementById(\"ffile\").innerHTML = \"\"'>"
                        "</div>"
                    "</label>"
                    "<div data-file-container='' class='bx--file-container' id='ffile'></div>"
                "</div>"
            "</div>"
            "<input type='submit' value='Update Firmware' class='bx--btn bx--btn--danger'>"
        "</form>"
    "</div>"
    "<div class='bx--col-md-4'>"
        "<form method='POST' action='' enctype='multipart/form-data'>"
            "<div class='cv-file-uploader cv-form-item bx--form-item'>"
                "<strong class='bx--file--label'>Update FileSystem</strong>"
                "<p class='bx--label-description'>Select the filesystem you want to upload</p>"
                "<div data-file='' class='bx--file'>"
                    "<label for='filesystem' role='button' tabindex='0' class='bx--file-browse-btn'>"
                        "<div data-file-drop-container='' class='bx--file__drop-container'>"
                            "Drag and drop file here or upload"
                            "<input type='file' id='filesystem' accept='.bin,.bin.gz' class='bx--file-input' name='filesystem' onchange='document.getElementById(\"fsys\").innerHTML = \"\"'>"
                        "</div>"
                    "</label>"
                    "<div data-file-container='' class='bx--file-container' id='fsys'></div>"
                "</div>"
            "</div>"
            "<input type='submit' value='Update FileSystem' class='bx--btn bx--btn--danger'>"
        "</form>"
    "</div>"
"</div>";

/**
 * Controls for weather configuration
 */
static const char WEATHER_FORM1[] PROGMEM = "<form action='/updateweatherconfig' method='get'>"
                    "<div class='bx--row'>"
                        "<div class='bx--form-item bx--col bx--col--auto'>"
                            "<input class='bx--toggle-input bx--toggle-input--small' id='isWeatherEnabled' type='checkbox' name='isWeatherEnabled' %IS_WEATHER_CHECKED%>"
                            "<label class='bx--toggle-input__label' for='isWeatherEnabled'>"
                                "<span class='bx--toggle__switch'>"
                                    "<svg class='bx--toggle__check' width='6px' height='5px' viewBox='0 0 6 5'>"
                                        "<path d='M2.2 2.7L5 0 6 1 2.2 5 0 2.7 1 1.5z' />"
                                    "</svg>"
                                    "<span class='bx--toggle__text--off' aria-hidden='true'>Display Weather when printer is off deactivated</span>"
                                    "<span class='bx--toggle__text--on' aria-hidden='true'>Display Weather when printer is off activated</span>"
                                "</span>"
                            "</label>"
                        "</div>"
                    "</div>";

static const char WEATHER_FORM2[] PROGMEM = "<div class='bx--row'>"
                        "<div class='bx--form-item bx--col bx--col--auto'>"
                            "<input class='bx--toggle-input bx--toggle-input--small' id='metric' type='checkbox' name='metric' %METRIC%>"
                            "<label class='bx--toggle-input__label' for='metric'>"
                                "<span class='bx--toggle__switch'>"
                                    "<svg class='bx--toggle__check' width='6px' height='5px' viewBox='0 0 6 5'>"
                                        "<path d='M2.2 2.7L5 0 6 1 2.2 5 0 2.7 1 1.5z' />"
                                    "</svg>"
                                    "<span class='bx--toggle__text--on' aria-hidden='true'>Show in Celsius</span>"
                                    "<span class='bx--toggle__text--off' aria-hidden='true'>Show in Fahrenheit</span>"
                                "</span>"
                            "</label>"
                        "</div>"
                    "</div>";

static const char WEATHER_FORM3[] PROGMEM = "<div class='bx--row'>"
                        "<div class='bx--form-item bx--col bx--col--auto'>"
                            "<label for='openWeatherMapApiKey' class='bx--label'>OpenWeatherMap API Key (get from <a href='https://openweathermap.org/' target='_BLANK'>here</a>)</label>"
                            "<input id='openWeatherMapApiKey' type='text' class='bx--text-input' name='openWeatherMapApiKey' value='%WEATHERKEY%' maxlength='60'>"
                        "</div>"
                    "</div>";

static const char WEATHER_FORM4[] PROGMEM = "<div class='bx--row'>"
                        "<div class='bx--form-item bx--col bx--col--auto'>"
                            "<label for='city1' class='bx--label'>%CITYNAME1% (<a href='http://openweathermap.org/find' target='_BLANK'><i class='fa fa-search'></i> Search for City ID</a>)</label>"
                            "<input id='city1' type='text' class='bx--text-input' name='city1' value='%CITY1%' onkeypress='return isNumberKey(event)'>"
                        "</div>"
                    "</div>"
                    "<div class='bx--row'>"
                        "<div class='bx--form-item bx--col bx--col--auto bx--select'>"
                            "<label for='language' class='bx--label'>Weather Language</label>"
                            "<div class='bx--select-input__wrapper'>"
                                "<select id='language' class='bx--select-input' name='language'>";

static const char WEATHER_FORM5[] PROGMEM = "</select>"
                                "<svg focusable='false' preserveAspectRatio='xMidYMid meet' style='will-change: transform;' xmlns='http://www.w3.org/2000/svg' class='bx--select__arrow' width='10' height='6' viewBox='0 0 10 6' aria-hidden='true'><path d='M5 6L0 1 0.7 0.3 5 4.6 9.3 0.3 10 1z'></path></svg>"
                            "</div>"
                        "</div>"
                    "</div>"
                    "<div class='bx--row'>"
                        "<div class='bx--form-item bx--col bx--col--auto'>"
                            "<button class='bx--btn bx--btn--primary' type='submit'>Save</button>"
                        "</div>"
                    "</div>"
                "</form>"
                "<script>function isNumberKey(e){var h=e.which?e.which:event.keyCode;return!(h>31&&(h<48||h>57))}</script>";

static const char WEATHER_FORM_OPTIONS[] PROGMEM = "<option class='bx--select-option'>ar</option>"
                                    "<option class='bx--select-option'>bg</option>"
                                    "<option class='bx--select-option'>ca</option>"
                                    "<option class='bx--select-option'>cz</option>"
                                    "<option class='bx--select-option'>de</option>"
                                    "<option class='bx--select-option'>el</option>"
                                    "<option class='bx--select-option'>en</option>"
                                    "<option class='bx--select-option'>fa</option>"
                                    "<option class='bx--select-option'>fi</option>"
                                    "<option class='bx--select-option'>fr</option>"
                                    "<option class='bx--select-option'>gl</option>"
                                    "<option class='bx--select-option'>hr</option>"
                                    "<option class='bx--select-option'>hu</option>"
                                    "<option class='bx--select-option'>it</option>"
                                    "<option class='bx--select-option'>ja</option>"
                                    "<option class='bx--select-option'>kr</option>"
                                    "<option class='bx--select-option'>la</option>"
                                    "<option class='bx--select-option'>lt</option>"
                                    "<option class='bx--select-option'>mk</option>"
                                    "<option class='bx--select-option'>nl</option>"
                                    "<option class='bx--select-option'>pl</option>"
                                    "<option class='bx--select-option'>pt</option>"
                                    "<option class='bx--select-option'>ro</option>"
                                    "<option class='bx--select-option'>ru</option>"
                                    "<option class='bx--select-option'>se</option>"
                                    "<option class='bx--select-option'>sk</option>"
                                    "<option class='bx--select-option'>sl</option>"
                                    "<option class='bx--select-option'>es</option>"
                                    "<option class='bx--select-option'>tr</option>"
                                    "<option class='bx--select-option'>ua</option>"
                                    "<option class='bx--select-option'>vi</option>"
                                    "<option class='bx--select-option'>zh_cn</option>"
                                    "<option class='bx--select-option'>zh_tw</option>";

/**
 * Controls for station configuration
 */
static const char STATION_CONFIG_FORM1[] PROGMEM = "<form action='/updatestationconfig' method='get'>"
                "<div class='bx--row'>"
                    "<div class='bx--col bx--col--auto bx--form-item'>"
                        "<input class='bx--toggle-input bx--toggle-input--small' id='isClockEnabled' type='checkbox' name='isClockEnabled' %IS_CLOCK_CHECKED%>"
                        "<label class='bx--toggle-input__label' for='isClockEnabled'>"
                            "<span class='bx--toggle__switch'>"
                                "<svg class='bx--toggle__check' width='6px' height='5px' viewBox='0 0 6 5'>"
                                    "<path d='M2.2 2.7L5 0 6 1 2.2 5 0 2.7 1 1.5z' />"
                                "</svg>"
                                "<span class='bx--toggle__text--off' aria-hidden='true'>Display Clock when printer is off deactivated</span>"
                                "<span class='bx--toggle__text--on' aria-hidden='true'>Display Clock when printer is off activated</span>"
                            "</span>"
                        "</label>"
                    "</div>"
                "</div>";

static const char STATION_CONFIG_FORM2[] PROGMEM = "<div class='bx--row'>"
                    "<div class='bx--col bx--col--auto bx--form-item'>"
                        "<input class='bx--toggle-input bx--toggle-input--small' id='is24hour' type='checkbox' name='is24hour' %IS_24HOUR_CHECKED%>"
                        "<label class='bx--toggle-input__label' for='is24hour'>"
                            "<span class='bx--toggle__switch'>"
                                "<svg class='bx--toggle__check' width='6px' height='5px' viewBox='0 0 6 5'>"
                                    "<path d='M2.2 2.7L5 0 6 1 2.2 5 0 2.7 1 1.5z' />"
                                "</svg>"
                                "<span class='bx--toggle__text--off' aria-hidden='true'>Use 24 Hour Clock (military time) deactivated</span>"
                                "<span class='bx--toggle__text--on' aria-hidden='true'>Use 24 Hour Clock (military time) activated</span>"
                            "</span>"
                        "</label>"
                    "</div>"
                "</div>";

static const char STATION_CONFIG_FORM3[] PROGMEM = "<div class='bx--row'>"
                    "<div class='bx--col bx--col--auto bx--form-item'>"
                        "<input class='bx--toggle-input bx--toggle-input--small' id='invDisp' type='checkbox' name='invDisp' %IS_INVDISP_CHECKED%>"
                        "<label class='bx--toggle-input__label' for='invDisp'>"
                            "<span class='bx--toggle__switch'>"
                                "<svg class='bx--toggle__check' width='6px' height='5px' viewBox='0 0 6 5'>"
                                    "<path d='M2.2 2.7L5 0 6 1 2.2 5 0 2.7 1 1.5z' />"
                                "</svg>"
                                "<span class='bx--toggle__text--off' aria-hidden='true'>Flip display orientation deactivated</span>"
                                "<span class='bx--toggle__text--on' aria-hidden='true'>Flip display orientation activated</span>"
                            "</span>"
                        "</label>"
                    "</div>"
                "</div>";

static const char STATION_CONFIG_FORM4[] PROGMEM = "<div class='bx--row'>"
                    "<div class='bx--col bx--col--auto bx--form-item'>"
                        "<input class='bx--toggle-input bx--toggle-input--small' id='useFlash' type='checkbox' name='useFlash' %USEFLASH%>"
                        "<label class='bx--toggle-input__label' for='useFlash'>"
                            "<span class='bx--toggle__switch'>"
                                "<svg class='bx--toggle__check' width='6px' height='5px' viewBox='0 0 6 5'>"
                                    "<path d='M2.2 2.7L5 0 6 1 2.2 5 0 2.7 1 1.5z' />"
                                "</svg>"
                                "<span class='bx--toggle__text--off' aria-hidden='true'>Flash System LED on Service Calls deactivated</span>"
                                "<span class='bx--toggle__text--on' aria-hidden='true'>Flash System LED on Service Calls activated</span>"
                            "</span>"
                        "</label>"
                    "</div>"
                "</div>";

static const char STATION_CONFIG_FORM5[] PROGMEM = "<div class='bx--row'>"
                    "<div class='bx--form-item bx--col bx--col--auto bx--select'>"
                        "<label for='refresh' class='bx--label'>Clock Sync / Weather Refresh (minutes)</label>"
                        "<div class='bx--select-input__wrapper'>"
                            "<select id='refresh' class='bx--select-input' name='refresh'>"
                                "%OPTIONS%"
                            "</select>"
                            "<svg focusable='false' preserveAspectRatio='xMidYMid meet' style='will-change: transform;' xmlns='http://www.w3.org/2000/svg' class='bx--select__arrow' width='10' height='6' viewBox='0 0 10 6' aria-hidden='true'><path d='M5 6L0 1 0.7 0.3 5 4.6 9.3 0.3 10 1z'></path></svg>"
                        "</div>"
                    "</div>"
                "</div>";

static const char STATION_CONFIG_FORM5OPT[] PROGMEM = "<option class='bx--select-option'>10</option>"
                                "<option class='bx--select-option'>15</option>"
                                "<option class='bx--select-option'>20</option>"
                                "<option class='bx--select-option'>30</option>"
                                "<option class='bx--select-option'>60</option>";

static const char STATION_CONFIG_FORM6[] PROGMEM = "<div class='bx--row'>"
                    "<div class='bx--form-item bx--col bx--col--auto'>"
                        "<label for='utcoffset' class='bx--label'>UTC Time Offset</label>"
                        "<input id='utcoffset' type='text' class='bx--text-input' name='utcoffset' value='%UTCOFFSET%' maxlength='1'>"
                    "</div>"
                "</div>";

static const char STATION_CONFIG_FORM7[] PROGMEM = "<div class='bx--row'>"
                    "<div class='bx--col bx--col--auto bx--form-item'>"
                        "<input class='bx--toggle-input bx--toggle-input--small' id='isBasicAuth' type='checkbox' name='isBasicAuth' %IS_BASICAUTH_CHECKED%>"
                        "<label class='bx--toggle-input__label' for='isBasicAuth'>"
                            "<span class='bx--toggle__switch'>"
                                "<svg class='bx--toggle__check' width='6px' height='5px' viewBox='0 0 6 5'>"
                                    "<path d='M2.2 2.7L5 0 6 1 2.2 5 0 2.7 1 1.5z' />"
                                "</svg>"
                                "<span class='bx--toggle__text--off' aria-hidden='true'>Use Security Credentials for Configuration Changes deactivated</span>"
                                "<span class='bx--toggle__text--on' aria-hidden='true'>Use Security Credentials for Configuration Changes activated</span>"
                            "</span>"
                        "</label>"
                    "</div>"
                "</div>";

static const char STATION_CONFIG_FORM8[] PROGMEM = "<div class='bx--row'>"
                    "<div class='bx--form-item bx--col bx--col--auto'>"
                        "<label for='userid' class='bx--label'>User ID (for this interface)</label>"
                        "<input id='userid' type='text' class='bx--text-input' name='userid' value='%USERID%' maxlength='20'>"
                    "</div>"
                "</div>"
                "<div class='bx--row'>"
                    "<div class='bx--form-item bx--col bx--col--auto'>"
                        "<label for='stationpassword' class='bx--label'>Password (for this interface)</label>"
                        "<input id='stationpassword' type='password' class='bx--text-input' name='stationpassword' value='%STATIONPASSWORD%'>"
                    "</div>"
                "</div>"
                "<div class='bx--row'>"
                    "<div class='bx--form-item bx--col bx--col--auto'>"
                        "<button class='bx--btn bx--btn--primary' type='submit'>Save</button>"
                    "</div>"
                "</div>"
            "</form>";

/**
 * @brief Class to generate HTML content from Memory
 */
class WebserverMemoryVariables {
public:
    static void sendHeader(ESP8266WebServer *server, GlobalDataController *globalDataController, String pageLabel, String pageTitle);
    static void sendHeader(ESP8266WebServer *server, GlobalDataController *globalDataController, String pageLabel, String pageTitle, boolean refresh);
    static void sendFooter(ESP8266WebServer *server, GlobalDataController *globalDataController);

    static void sendUpdateForm(ESP8266WebServer *server, GlobalDataController *globalDataController);
    static void sendWeatherConfigForm(ESP8266WebServer *server, GlobalDataController *globalDataController);
    static void sendStationConfigForm(ESP8266WebServer *server, GlobalDataController *globalDataController);
};
