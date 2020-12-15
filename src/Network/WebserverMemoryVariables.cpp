#include "WebserverMemoryVariables.h"

/**
 * @brief Send out header for webpage
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendHeader(ESP8266WebServer *server, GlobalDataController *globalDataController, String pageLabel, String pageTitle) {
    WebserverMemoryVariables::sendHeader(server, globalDataController, pageLabel, pageTitle, false);
}

/**
 * @brief Send out header for webpage
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 * @param pageLabel                 Title label
 * @param pageTitle                 Title
 * @param refresh                   if true, auto refresh in header is set
 */
void WebserverMemoryVariables::sendHeader(
    ESP8266WebServer *server,
    GlobalDataController *globalDataController,
    String pageLabel,
    String pageTitle,
    boolean refresh
) {
    globalDataController->ledOnOff(true);
    int8_t rssi = globalDataController->getWifiQuality();

    server->sendHeader("Cache-Control", "no-cache, no-store");
    server->sendHeader("Pragma", "no-cache");
    server->sendHeader("Expires", "-1");
    server->setContentLength(CONTENT_LENGTH_UNKNOWN);
    server->send(200, "text/html", "");

    server->sendContent("<!DOCTYPE HTML>"
        "<html><head><title>PrintBuddy</title><link rel='icon' href='data:;base64,='>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    );
    if (refresh) {
        server->sendContent("<meta http-equiv=\"refresh\" content=\"30\">");
    }
    server->sendContent("<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>"
        "<link rel='stylesheet' href='https://unpkg.com/carbon-components/css/carbon-components.min.css'></style>"
        "<link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.15.1/css/all.css'>"
        "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js'></script>"
        "<style>.hidden{display:none}</style>"
        "</head><body>"
        "<header class='cv-header bx--header'>"
        "<a href='/' class='cv-header-name bx--header__name'>"
    );
    server->sendContent("<span class='bx--header__name--prefix'>PrintBuddy&nbsp;</span>V" + String(VERSION));
    server->sendContent("</a>"
        "<nav class='cv-header-nav bx--header__nav'></nav>"
        "<div class='bx--header__global'>"
            "<button type='button' class='cv-header-global-action bx--header__action' onclick='openWifiInfo()'>"
                "<i class='fas fa-wifi' style='color: white; font-size: 20px;'></i>"
            "</button>"
            "<button type='button' class='cv-header-global-action bx--header__action' onclick='openSidebar()'>"
                "<svg focusable='false' preserveAspectRatio='xMidYMid meet' xmlns='http://www.w3.org/2000/svg' fill='currentColor' width='20' height='20' viewBox='0 0 32 32' aria-hidden='true'>"
                    "<path d='M14 4H18V8H14zM4 4H8V8H4zM24 4H28V8H24zM14 14H18V18H14zM4 14H8V18H4zM24 14H28V18H24zM14 24H18V28H14zM4 24H8V28H4zM24 24H28V28H24z'></path>"
                "</svg>"
            "</button>"
        "</div>"
        "<div id='sidebar' class='cv-header-panel bx--header-panel'>"
            "<ul class='cv-switcher bx--switcher__item'>"
    );
    server->sendContent(String(FPSTR(MENUE_ITEMS)));
    server->sendContent("</ul>"
        "</div>"
        "<div class='bx--toast-notification bx--toast-notification--info hidden' style='position: absolute; right: -16px; top: 40px;' id='wifiinfo'>"
            "<div class='bx--toast-notification__details'>"
                "<h3 class='bx--toast-notification__title'>WiFi Signal Strength</h3>"
                    "<div class='bx--toast-notification__subtitle'><span>"
    );
    server->sendContent(String(rssi) + "%");
    server->sendContent("</span></div>"
                "</div>"
            "</div>"
        "</header>"
        "<script>function openSidebar(){document.getElementById('sidebar').classList.toggle('bx--header-panel--expanded');document.getElementById('wifiinfo').classList.add('hidden');};function openWifiInfo(){document.getElementById('sidebar').classList.remove('bx--header-panel--expanded');document.getElementById('wifiinfo').classList.toggle('hidden');}</script>"
        "<br><div class='bx--grid bx--grid--full-width' style='margin-top:60px'>"
            "<div class='page-header' style='margin-bottom:20px'><h4 class='page-header__label'>");
    server->sendContent(pageLabel);
    server->sendContent("</h4><h1 id='page-title' class='page-header__title'>");
    server->sendContent(pageTitle);
    server->sendContent("</h1></div><div class='bx--row'>");
}

/**
 * @brief send out footer content for webpage
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendFooter(ESP8266WebServer *server, GlobalDataController *globalDataController) {
    server->sendContent("<br><br><br></div></div>");
    server->sendContent("<script src='https://unpkg.com/carbon-components/scripts/carbon-components.min.js'></script></body></html>");
    server->sendContent("");
    server->client().stop();
    globalDataController->ledOnOff(false);
}

/**
 * @brief Send out upload form for updates
 * @param server                    Send out instancce
 * @param globalDataController      Access to global data
 */
void WebserverMemoryVariables::sendUpdateForm(ESP8266WebServer *server, GlobalDataController *globalDataController) {
    server->sendContent(FPSTR(UPDATE_FORM));
}