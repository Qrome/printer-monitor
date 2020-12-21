#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "../Global/GlobalDataController.h"

/**
 * Webpage form items for reuse
 */
static const char FORM_ITEM_CHECKBOX[] PROGMEM = "<div class='bx--row'>"
                    "<div class='bx--col bx--col--auto bx--form-item'>"
                        "<input class='bx--toggle-input bx--toggle-input--small' id='%FORMID%' type='checkbox' name='%FORMID%' %CHECKED% %ONCHANGE%>"
                        "<label class='bx--toggle-input__label' for='%FORMID%'>"
                            "<span class='bx--toggle__switch'>"
                                "<svg class='bx--toggle__check' width='6px' height='5px' viewBox='0 0 6 5'>"
                                    "<path d='M2.2 2.7L5 0 6 1 2.2 5 0 2.7 1 1.5z' />"
                                "</svg>"
                                "<span class='bx--toggle__text--off' aria-hidden='true'>%LABELOFF%</span>"
                                "<span class='bx--toggle__text--on' aria-hidden='true'>%LABELON%</span>"
                            "</span>"
                        "</label>"
                    "</div>"
                "</div>";
static const char FORM_ITEM_CHECKBOX_ON[] PROGMEM = " activated";
static const char FORM_ITEM_CHECKBOX_OFF[] PROGMEM = " deactivated";

/**
 * Webpage side menu right for main items
 */
static const char MENUE_ITEMS[] PROGMEM = 
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link menitem' href='/'>"
        "Home"
        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='16' height='16' viewBox='0 0 32 32' aria-hidden='true'><path d='M16.6123,2.2138a1.01,1.01,0,0,0-1.2427,0L1,13.4194l1.2427,1.5717L4,13.6209V26a2.0041,2.0041,0,0,0,2,2H26a2.0037,2.0037,0,0,0,2-2V13.63L29.7573,15,31,13.4282ZM18,26H14V18h4Zm2,0V18a2.0023,2.0023,0,0,0-2-2H14a2.002,2.002,0,0,0-2,2v8H6V12.0615l10-7.79,10,7.8005V26Z'></path></svg>"
    "</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link menitem' href='/configureprinter/show'>"
        "Configure Printers"
        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='16' height='16' viewBox='0 0 32 32' aria-hidden='true'><path d='M28,9H25V3H7V9H4a2,2,0,0,0-2,2V21a2,2,0,0,0,2,2H7v6H25V23h3a2,2,0,0,0,2-2V11A2,2,0,0,0,28,9ZM9,5H23V9H9ZM23,27H9V17H23Zm5-6H25V15H7v6H4V11H28Z'></path></svg>"
    "</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link menitem' href='/configurestation/show'>"
        "Configure Station"
        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='16' height='16' viewBox='0 0 32 32' aria-hidden='true'><path d='M27,16.76c0-.25,0-.5,0-.76s0-.51,0-.77l1.92-1.68A2,2,0,0,0,29.3,11L26.94,7a2,2,0,0,0-1.73-1,2,2,0,0,0-.64.1l-2.43.82a11.35,11.35,0,0,0-1.31-.75l-.51-2.52a2,2,0,0,0-2-1.61H13.64a2,2,0,0,0-2,1.61l-.51,2.52a11.48,11.48,0,0,0-1.32.75L7.43,6.06A2,2,0,0,0,6.79,6,2,2,0,0,0,5.06,7L2.7,11a2,2,0,0,0,.41,2.51L5,15.24c0,.25,0,.5,0,.76s0,.51,0,.77L3.11,18.45A2,2,0,0,0,2.7,21L5.06,25a2,2,0,0,0,1.73,1,2,2,0,0,0,.64-.1l2.43-.82a11.35,11.35,0,0,0,1.31.75l.51,2.52a2,2,0,0,0,2,1.61h4.72a2,2,0,0,0,2-1.61l.51-2.52a11.48,11.48,0,0,0,1.32-.75l2.42.82a2,2,0,0,0,.64.1,2,2,0,0,0,1.73-1L29.3,21a2,2,0,0,0-.41-2.51ZM25.21,24l-3.43-1.16a8.86,8.86,0,0,1-2.71,1.57L18.36,28H13.64l-.71-3.55a9.36,9.36,0,0,1-2.7-1.57L6.79,24,4.43,20l2.72-2.4a8.9,8.9,0,0,1,0-3.13L4.43,12,6.79,8l3.43,1.16a8.86,8.86,0,0,1,2.71-1.57L13.64,4h4.72l.71,3.55a9.36,9.36,0,0,1,2.7,1.57L25.21,8,27.57,12l-2.72,2.4a8.9,8.9,0,0,1,0,3.13L27.57,20Z'></path><path d='M16,22a6,6,0,1,1,6-6A5.94,5.94,0,0,1,16,22Zm0-10a3.91,3.91,0,0,0-4,4,3.91,3.91,0,0,0,4,4,3.91,3.91,0,0,0,4-4A3.91,3.91,0,0,0,16,12Z'></path></svg>"
    "</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link menitem' href='/configureweather/show'>"
        "Configure Weather"
        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='16' height='16' viewBox='0 0 32 32' aria-hidden='true'><path d='M24.8008,11.1382a8.9938,8.9938,0,0,0-17.6006,0A6.533,6.533,0,0,0,2,17.5H2V19a1,1,0,0,0,1,1H15a1,1,0,0,0,0-2H4v-.4966H4a4.5176,4.5176,0,0,1,4.144-4.4819l.8155-.064.0991-.812a6.9936,6.9936,0,0,1,13.8838,0l.0986.812.8154.064A4.4962,4.4962,0,0,1,23.5,22H7a1,1,0,0,0,0,2H23.5a6.4963,6.4963,0,0,0,1.3008-12.8618Z'></path><rect width='18' height='2' x='2' y='26' rx='1'></rect></svg>"
    "</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link menitem' href='/configuresensor/show'>"
        "Configure Sensor"
        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='16' height='16' viewBox='0 0 32 32' aria-hidden='true'><path d='M30,19H26V15H24v9H8V8l9-.0009V6H13V2H11V6H8A2.002,2.002,0,0,0,6,8v3H2v2H6v6H2v2H6v3a2.0023,2.0023,0,0,0,2,2h3v4h2V26h6v4h2V26h3a2.0027,2.0027,0,0,0,2-2V21h4Z'></path><path d='M26,2a4.0042,4.0042,0,0,0-4,4,3.9556,3.9556,0,0,0,.5668,2.0192L19.5859,11H11V21H21V12.4141l2.9808-2.9808A3.9553,3.9553,0,0,0,26,10a4,4,0,0,0,0-8ZM19,19H13V13h6ZM26,8a2,2,0,1,1,2-2A2.0023,2.0023,0,0,1,26,8Z'></path></svg>"
    "</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link menitem' href='/systemreset' onclick='return confirm(\"Do you want to reset to default settings?\")'>"
        "Reset Settings"
        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='16' height='16' viewBox='0 0 32 32' aria-hidden='true'><path d='M18,28A12,12,0,1,0,6,16v6.2L2.4,18.6,1,20l6,6,6-6-1.4-1.4L8,22.2V16H8A10,10,0,1,1,18,26Z'></path></svg>"
    "</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link menitem' href='/forgetwifi' onclick='return confirm(\"Do you want to forget to WiFi connection?\")'>"
        "Forget WiFi"
        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='16' height='16' viewBox='0 0 32 32' aria-hidden='true'><circle cx='16' cy='25' r='2'></circle><path d='M30 3.4141L28.5859 2 2 28.5859 3.4141 30 14.0962 19.3179a5.9359 5.9359 0 016.01 1.3193L21.52 19.2236a7.9669 7.9669 0 00-5.125-2.2041l3.3875-3.3877a11.9908 11.9908 0 014.5647 2.7647L25.76 14.9829A13.975 13.975 0 0021.334 12.08L24.3308 9.083a17.9364 17.9364 0 014.2546 3.0747L30 10.7432v-.002a20.02 20.02 0 00-4.1895-3.1377zM14.68 13.0776l2.0415-2.0415C16.481 11.0234 16.2437 11 16 11a13.9447 13.9447 0 00-9.771 3.9927l1.4136 1.4136A11.97 11.97 0 0114.68 13.0776zM16 7a17.87 17.87 0 014.2324.5254L21.875 5.8828A19.9537 19.9537 0 002 10.7412v.0225L3.4043 12.168A17.9193 17.9193 0 0116 7z'></path></svg>"
    "</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link menitem' href='/update'>"
        "Firmware Update"
        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='16' height='16' viewBox='0 0 32 32' aria-hidden='true'><path d='M28,12H20V4h8Zm-6-2h4V6H22Z'></path><path d='M17,15V9H9V23H23V15Zm-6-4h4v4H11Zm4,10H11V17h4Zm6,0H17V17h4Z'></path><path d='M26,28H6a2.0023,2.0023,0,0,1-2-2V6A2.0023,2.0023,0,0,1,6,4H16V6H6V26H26V16h2V26A2.0023,2.0023,0,0,1,26,28Z'></path></svg>"
    "</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link menitem' href='https://github.com/Qrome' target='_blank'>"
        "About"
        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='16' height='16' viewBox='0 0 32 32' aria-hidden='true'><path d='M16,2A14,14,0,1,0,30,16,14,14,0,0,0,16,2Zm0,26A12,12,0,1,1,28,16,12,12,0,0,1,16,28Z'></path><circle cx='16' cy='23.5' r='1.5'></circle><path d='M17,8H15.5A4.49,4.49,0,0,0,11,12.5V13h2v-.5A2.5,2.5,0,0,1,15.5,10H17a2.5,2.5,0,0,1,0,5H15v4.5h2V17a4.5,4.5,0,0,0,0-9Z'></path></svg>"
    "</a></li>";

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
        "<script>function showhide(a,b) {var e=$(\"[data-sh='\"+b+\"']\");if(a.checked||a.prop('checked')){e.removeClass('hidden')}else{e.addClass('hidden')}}</script>"
        "<style>.hidden{display:none} .bx--form-item{margin-bottom:20px} .bx--table-column-menu{width: 3.25rem} .menitem{padding:6px 1rem;font-size:.875rem;font-weight:600;line-height:1.29;letter-spacing:.16px;display:flex;justify-content:space-between;text-decoration:none;color:#c6c6c6}</style>"
        "<script>function openModal(refelementId){document.body.classList.add(\"bx--body--with-modal-open\");document.getElementById(refelementId).classList.add(\"is-visible\")} function closeModal(refelementId){document.getElementById(refelementId).classList.remove(\"is-visible\");document.body.classList.remove(\"bx--body--with-modal-open\")}</script>"
        "</head><body>"
        "<header class='cv-header bx--header'>"
        "<a href='/' class='cv-header-name bx--header__name'>";

static const char HEADER_BLOCK3[] PROGMEM = "</a>"
        "<nav class='cv-header-nav bx--header__nav'></nav>"
        "<div class='bx--header__global'>"
            "<button type='button' class='cv-header-global-action bx--header__action' onclick='openChipInfo()'>"
                "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='20' height='20' viewBox='0 0 32 32' aria-hidden='true'>"
                    "<path d='M11,11V21H21V11Zm8,8H13V13h6Z'></path>"
                    "<path d='M30,13V11H26V8a2,2,0,0,0-2-2H21V2H19V6H13V2H11V6H8A2,2,0,0,0,6,8v3H2v2H6v6H2v2H6v3a2,2,0,0,0,2,2h3v4h2V26h6v4h2V26h3a2,2,0,0,0,2-2V21h4V19H26V13ZM24,24H8V8H24Z'></path>"
                "</svg>"
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

static const char HEADER_BLOCK_ERROR[] PROGMEM = "<div class='bx--inline-notification bx--inline-notification--error' role='alert' style='max-width:100%'>"
            "<div class='bx--inline-notification__details'>"
                "<svg focusable='false' preserveAspectRatio='xMidYMid meet' style='will-change: transform;' xmlns='http://www.w3.org/2000/svg' class='bx--inline-notification__icon' width='20' height='20' viewBox='0 0 20 20' aria-hidden='true'><path d='M10,1c-5,0-9,4-9,9s4,9,9,9s9-4,9-9S15,1,10,1z M13.5,14.5l-8-8l1-1l8,8L13.5,14.5z'></path><path d='M13.5,14.5l-8-8l1-1l8,8L13.5,14.5z' data-icon-path='inner-path' opacity='0'></path></svg>"
                "<div class='bx--inline-notification__text-wrapper'>"
                    "<p class='bx--inline-notification__title'>%ERRORMSG%</p>"
                "</div>"
            "</div>"
            "<button data-notification-btn class='bx--inline-notification__close-button' type='button' aria-label='close'>"
                "<svg focusable='false' preserveAspectRatio='xMidYMid meet' style='will-change: transform;' xmlns='http://www.w3.org/2000/svg' class='bx--inline-notification__close-icon' width='20' height='20' viewBox='0 0 32 32' aria-hidden='true'><path d='M24 9.4L22.6 8 16 14.6 9.4 8 8 9.4 14.6 16 8 22.6 9.4 24 16 17.4 22.6 24 24 22.6 17.4 16 24 9.4z'></path></svg>"
            "</button>"
        "</div>";

static const char FOOTER_BLOCK[] PROGMEM = "<br><br><br></div>"
        "<div class='bx--loading-overlay hidden' id='pageloading'>"
            "<div data-loading class='bx--loading'>"
                "<svg class='bx--loading__svg' viewBox='-75 -75 150 150'>"
                    "<title>Loading</title>"
                    "<circle class='bx--loading__stroke' cx='0' cy='0' r='37.5' />"
                "</svg>"
            "</div>"
        "</div>"
        "<script src='https://unpkg.com/carbon-components/scripts/carbon-components.min.js'></script>"
        "<script>$(function(){$('form').on('submit',function(e){$('#pageloading').removeClass('hidden')})})</script>"
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
static const char WEATHER_FORM_START[] PROGMEM = "<form action='/configureweather/update' method='get'>";

static const char WEATHER_FORM1_ID[] PROGMEM = "isWeatherEnabled";
static const char WEATHER_FORM1_LABEL[] PROGMEM = "Display Weather when printer is off";

static const char WEATHER_FORM2_ID[] PROGMEM = "metric";
static const char WEATHER_FORM2_LABEL_ON[] PROGMEM = "Show in Celsius";
static const char WEATHER_FORM2_LABEL_OFF[] PROGMEM = "Show in Fahrenheit";

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
static const char STATION_CONFIG_FORM_START[] PROGMEM = "<form action='/configurestation/update' method='get'>";

static const char STATION_CONFIG_FORM1_ID[] PROGMEM = "isClockEnabled";
static const char STATION_CONFIG_FORM1_LABEL[] PROGMEM = "Display Clock when printer is off";

static const char STATION_CONFIG_FORM2_ID[] PROGMEM = "is24hour";
static const char STATION_CONFIG_FORM2_LABEL[] PROGMEM = "Use 24 Hour Clock (military time)";

static const char STATION_CONFIG_FORM3_ID[] PROGMEM = "invDisp";
static const char STATION_CONFIG_FORM3_LABEL[] PROGMEM = "Flip display orientation";

static const char STATION_CONFIG_FORM4_ID[] PROGMEM = "useFlash";
static const char STATION_CONFIG_FORM4_LABEL[] PROGMEM = "Flash System LED on Service Call";

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

static const char STATION_CONFIG_FORM7_ID[] PROGMEM = "isBasicAuth";
static const char STATION_CONFIG_FORM7_LABEL[] PROGMEM = "Use Security Credentials for Configuration Changes";

static const char STATION_CONFIG_FORM8[] PROGMEM = "<div class='bx--row' data-sh='uspw'>"
                    "<div class='bx--form-item bx--col bx--col--auto'>"
                        "<label for='userid' class='bx--label'>User ID (for this interface)</label>"
                        "<input id='userid' type='text' class='bx--text-input' name='userid' value='%USERID%' maxlength='20'>"
                    "</div>"
                "</div>"
                "<div class='bx--row' data-sh='uspw'>"
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
            "</form><script>showhide($('#isBasicAuth'), 'uspw')</script>";

/**
 * Controls for printer configuration
 */
static const char CONFPRINTER_FORM_START[] PROGMEM = "<div class='bx--row'>"
                "<div class='bx--col bx--col--auto bx--data-table-container' data-table>"
                    "<div class='bx--data-table-header'>"
                        "<h4 class='bx--data-table-header__title'>Printers to monitor</h4>"
                        "<p class='bx--data-table-header__description'>Configurationdata</p>"
                    "</div>"
                    "<section class='bx--table-toolbar'>"
                        "<div class='bx--toolbar-content'>"
                            "<button class='bx--btn bx--btn--sm bx--btn--primary' onclick='openModal(\"mae-0\")'>"
                                "Add new"
                                "<svg focusable='false' preserveAspectRatio='xMidYMid meet' style='will-change: transform;' xmlns='http://www.w3.org/2000/svg' class='bx--btn__icon' width='20' height='20' viewBox='0 0 32 32'><path d='M17 15L17 7 15 7 15 15 7 15 7 17 15 17 15 25 17 25 17 17 25 17 25 15 17 15z'></path></svg>"
                            "</button>"
                        "</div>"
                    "</section>"
                    "<table class='bx--data-table bx--data-table--visible-overflow-menu'>"
                        "<thead>"
                            "<tr>"
                                "<th><span class='bx--table-header-label'>Name</span></th>"
                                "<th><span class='bx--table-header-label'>Type</span></th>"
                                "<th><span class='bx--table-header-label'>State</span></th>"
                                "<th class='bx--table-column-menu' style='width: 3.25rem'></th>"
                            "</tr>"
                        "</thead>"
                        "<tbody>";

static const char CONFPRINTER_FORM_ROW_ERROR[] PROGMEM =  "<div class='bx--tag bx--tag--magenta'>"
                                        "%STATUS%"
                                    "</div>";
static const char CONFPRINTER_FORM_ROW_OK[] PROGMEM =  "<div class='bx--tag bx--tag--green'>"
                                        "%STATUS%"
                                    "</div>";

static const char CONFPRINTER_FORM_ROW[] PROGMEM = "<tr>"
                                "<td>%NAME%</td>"
                                "<td>%TYPE%</td>"
                                "<td>%STATE%</td>"
                                "<td class='bx--table-column-menu' style='width: 3.25rem'>"
                                    "<div data-overflow-menu role='menu' tabindex='0' class='bx--overflow-menu'>"
                                        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' style='will-change: transform;' xmlns='http://www.w3.org/2000/svg' class='bx--overflow-menu__icon' width='16' height='16' viewBox='0 0 16 16' aria-hidden='true'><circle cx='8' cy='3' r='1'></circle><circle cx='8' cy='8' r='1'></circle><circle cx='8' cy='13' r='1'></circle></svg>"
                                        "<ul class='bx--overflow-menu-options bx--overflow-menu--flip' data-floating-menu-direction='bottom'>"
                                            "<li class='bx--overflow-menu-options__option bx--table-row--menu-option'>"
                                                "<button class='bx--overflow-menu-options__btn' onclick='openModal(\"mae-%ID%\")'>"
                                                    "<div class='bx--overflow-menu-options__option-content'>"
                                                        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' style='will-change: transform;' xmlns='http://www.w3.org/2000/svg' width='16' height='16' viewBox='0 0 16 16' aria-hidden='true'><path d='M1 13H15V14H1zM12.7 4.5c.4-.4.4-1 0-1.4 0 0 0 0 0 0l-1.8-1.8c-.4-.4-1-.4-1.4 0 0 0 0 0 0 0L2 8.8V12h3.2L12.7 4.5zM10.2 2L12 3.8l-1.5 1.5L8.7 3.5 10.2 2zM3 11V9.2l5-5L9.8 6l-5 5H3z'></path></svg> "
                                                        "Edit"
                                                    "</div>"
                                                "</button>"
                                            "</li>"
                                            "<li class='bx--overflow-menu-options__option bx--table-row--menu-option'>"
                                                "<button class='bx--overflow-menu-options__btn' onclick='openModal('modal-ed454ftfa4q')'>"
                                                    "<div class='bx--overflow-menu-options__option-content'>"
                                                        "<svg focusable='false' preserveAspectRatio='xMidYMid meet' style='will-change: transform;' xmlns='http://www.w3.org/2000/svg' width='16' height='16' viewBox='0 0 16 16' aria-hidden='true'><path d='M6 6H7V12H6zM9 6H10V12H9z'></path><path d='M2 3v1h1v10c0 .6.4 1 1 1h8c.6 0 1-.4 1-1V4h1V3H2zM4 14V4h8v10H4zM6 1H10V2H6z'></path></svg> "
                                                        "Delete"
                                                    "</div>"
                                                "</button>"
                                            "</li>"
                                        "</ul>"
                                    "</div>"
                                "</td>"
                            "</tr>";

static const char CONFPRINTER_FORM_END[] PROGMEM = "</tbody>"
                    "</table>"
                "</div>"
            "</div>";


static const char CONFPRINTER_FORM_ADDEDIT_TA[] PROGMEM = "Create new printer";
static const char CONFPRINTER_FORM_ADDEDIT_TE[] PROGMEM = "Edit data for printer";

static const char CONFPRINTER_FORM_ADDEDIT1[] PROGMEM = "<div data-modal id='mae-%ID%' class='bx--modal' role='dialog' aria-modal='true' aria-labelledby='mae-%ID%-label' aria-describedby='mae-%ID%-heading' tabindex='-1'>"
                "<div class='bx--modal-container'>"
                    "<form method='GET' action='/configureprinter/edit'>"
                        "<input type='hidden' name='id' value='%ID%'>"
                        "<div class='bx--modal-header'>"
                            "<p class='bx--modal-header__label bx--type-delta' id='mae-%ID%-label'>Printer Configuration</p>"
                            "<p class='bx--modal-header__heading bx--type-beta' id='mae-%ID%-heading'>%TITLE%</p>"
                            "<button class='bx--modal-close' type='button' onclick='closeModal(\"mae-%ID%\")'>"
                                "<svg focusable='false' preserveAspectRatio='xMidYMid meet' style='will-change: transform;' xmlns='http://www.w3.org/2000/svg' class='bx--modal-close__icon' width='16' height='16' viewBox='0 0 16 16' aria-hidden='true'><path d='M12 4.7L11.3 4 8 7.3 4.7 4 4 4.7 7.3 8 4 11.3 4.7 12 8 8.7 11.3 12 12 11.3 8.7 8z'></path></svg>"
                            "</button>"
                        "</div>"
                        "<div class='bx--modal-content bx--modal-content--with-form'>"
                            "<div class='bx--form-item'>"
                                "<label for='e-tname-%ID%' class='bx--label'>Printer Name</label>"
                                "<input id='e-tname-%ID%' name='e-tname' type='text' class='bx--text-input' placeholder='Custom name' data-modal-primary-focus maxlength='20' value='%NAME%'>"
                            "</div>"
                            "<div class='bx--form-item bx--select'>"
                                "<label for='e-tapi-%ID%' class='bx--label'>API Type</label>"
                                "<div class='bx--select-input__wrapper'>"
                                    "<select id='e-tapi-%ID%' class='bx--select-input' name='e-tapi'>"
                                        "<option class='bx--select-option' value='0'>Duet</option>"
                                        "<option class='bx--select-option' value='1'>Klipper</option>"
                                        "<option class='bx--select-option' selected value='2'>Octoprint</option>"
                                        "<option class='bx--select-option' value='3'>Repetier</option>"
                                    "</select>"
                                    "<svg focusable='false' preserveAspectRatio='xMidYMid meet' style='will-change: transform;' xmlns='http://www.w3.org/2000/svg' class='bx--select__arrow' width='10' height='6' viewBox='0 0 10 6' aria-hidden='true'><path d='M5 6L0 1 0.7 0.3 5 4.6 9.3 0.3 10 1z'></path></svg>"
                                "</div>"
                            "</div>"
                            "<div class='bx--form-item'>"
                                "<label for='e-taddr-%ID%' class='bx--label'>Hostname or IP Address (do not include http://)</label>"
                                "<input id='e-taddr-%ID%' name='e-taddr' type='text' class='bx--text-input' placeholder='Target Address' maxlength='60' value='%TARGETADDR%'>"
                            "</div>"
                            "<div class='bx--form-item'>"
                                "<label for='e-tport-%ID%' class='bx--label'>Port</label>"
                                "<input id='e-tport-%ID%' name='e-tport' type='text' class='bx--text-input' placeholder='Target port' maxlength='5' value='%TARGETPORT%'>"
                            "</div>"
                            "<div class='bx--form-item'>"
                                "<input class='bx--toggle-input bx--toggle-input--small' id='e-tpsu-%ID%' type='checkbox' name='e-tpsu'>"
                                "<label class='bx--toggle-input__label' for='e-tpsu-%ID%'>"
                                "<span class='bx--toggle__switch'>"
                                    "<svg class='bx--toggle__check' width='6px' height='5px' viewBox='0 0 6 5'>"
                                        "<path d='M2.2 2.7L5 0 6 1 2.2 5 0 2.7 1 1.5z' />"
                                    "</svg>"
                                    "<span class='bx--toggle__text--off' aria-hidden='true'>PSU control deactivated</span>"
                                    "<span class='bx--toggle__text--on' aria-hidden='true'>PSU control activated</span>"
                                "</span>"
                                "</label>"
                            "</div>"
                            "<div class='bx--form-item'>"
                                "<input class='bx--toggle-input bx--toggle-input--small' id='e-tapipw-%ID%' type='checkbox' name='e-tapipw' onchange='showhide(this, \"apac-%ID%\")' checked='checked'>"
                                "<label class='bx--toggle-input__label' for='e-tapipw-%ID%'>"
                                "<span class='bx--toggle__switch'>"
                                    "<svg class='bx--toggle__check' width='6px' height='5px' viewBox='0 0 6 5'>"
                                        "<path d='M2.2 2.7L5 0 6 1 2.2 5 0 2.7 1 1.5z' />"
                                    "</svg>"
                                    "<span class='bx--toggle__text--off' aria-hidden='true'>Haproxy or basic auth deactivated</span>"
                                    "<span class='bx--toggle__text--on' aria-hidden='true'>Haproxy or basic auth activated</span>"
                                "</span>"
                                "</label>"
                            "</div>"
                            "<div class='bx--form-item' data-sh='apac-%ID%'>"
                                "<label for='e-tapiuser-%ID%' class='bx--label'>User ID (for this interface)</label>"
                                "<input id='e-tapiuser-%ID%' type='text' class='bx--text-input' name='e-tapiuser' value='admin' maxlength='30'>"
                            "</div>"
                            "<div class='bx--form-item' data-sh='apac-%ID%'>"
                                "<label for='e-tapipass-%ID%' class='bx--label'>Password (for this interface)</label>"
                                "<input id='e-tapipass-%ID%' type='password' class='bx--text-input' name='e-tapipass' value='admin'>"
                            "</div><br><br>"
                        "</div>"
                        "<div class='bx--modal-content--overflow-indicator'></div>"
                        "<div class='bx--modal-footer'>"
                            "<button class='bx--btn bx--btn--secondary' type='reset' onclick='closeModal(\"mae-%ID%\")'>Abort</button>"
                            "<button class='bx--btn bx--btn--primary' type='submit' onclick='closeModal(\"mae-%ID%\")'>Save</button>"
                        "</div>"
                    "</form>"
                "</div>"
                "<span tabindex='0'></span>"
            "</div>"
        "</div>";

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
    static void sendPrinterConfigForm(ESP8266WebServer *server, GlobalDataController *globalDataController);

private:
    static void sendFormCheckbox(ESP8266WebServer *server, String formId, bool isChecked, String label);
    static void sendFormCheckbox(ESP8266WebServer *server, String formId, bool isChecked, String labelOn, String labelOff);
    static void sendFormCheckboxEvent(ESP8266WebServer *server, String formId, bool isChecked, String label, String onChange);
    static void sendFormCheckboxEvent(ESP8266WebServer *server, String formId, bool isChecked, String labelOn, String labelOff, String onChange);

    static void sendPrinterConfigFormAEModal(ESP8266WebServer *server, int id, PrinterDataStruct *forPrinter);
};
