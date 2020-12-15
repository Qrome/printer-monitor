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
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/configure'><i class='fa fa-cog'></i> Configure</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/configureweather'><i class='fa fa-cloud'></i> Weather</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/systemreset' onclick='return confirm(\"Do you want to reset to default settings?\")'><i class='fa fa-undo'></i> Reset Settings</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/forgetwifi' onclick='return confirm(\"Do you want to forget to WiFi connection?\")'><i class='fa fa-wifi'></i> Forget WiFi</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='/update'><i class='fa fa-wrench'></i> Firmware Update</a></li>"
    "<li class='cv-switcher-item bx--switcher__item'><a class='cv-switcher-item-link bx--switcher__item-link' href='https://github.com/Qrome' target='_blank'><i class='fa fa-question-circle'></i> About</a></li>";

/**
 * Controls for update firmware/filesystem
 */
static const char UPDATE_FORM[] PROGMEM = "<div class='bx--col-md-4'>"
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
};
