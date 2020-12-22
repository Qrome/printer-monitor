#include "GlobalDataController.h"

/**
 * @brief Initialize class for all needed data
 */
GlobalDataController::GlobalDataController(TimeClient *timeClient, OpenWeatherMapClient *weatherClient, DebugController *debugController) {
     this->timeClient = timeClient;
     this->weatherClient = weatherClient;
     this->debugController = debugController;
     this->printers = (PrinterDataStruct *)malloc(1 * sizeof(PrinterDataStruct));
     this->basePrinterClients = (BasePrinterClient**)malloc(1 * sizeof(int));
     this->initDefaultConfig();
}

/**
 * @brief Setup global controller
 */
void GlobalDataController::setup() {
    this->listSettingFiles();
    this->readSettings();
}

/**
 * @brief List files in eeprom for debug
 */
void GlobalDataController::listSettingFiles() {
    if(this->debugController->isEnabled()) {
        this->debugController->printLn("========= FileSystem Files =================");
        Dir dir = LittleFS.openDir("/");
        while (dir.next())
        {
            this->debugController->printLn(dir.fileName());
        }
    }
}

/**
 * @brief Read all setting from eeprom
 */
void GlobalDataController::readSettings() {
    if ((LittleFS.exists(CONFIG) == false) || (LittleFS.exists(PRINTERCONFIG) == false)) {
        this->debugController->printLn("Settings File does not yet exists.");
        this->writeSettings();
        return;
    }

    // Read basic settings
    File fr = LittleFS.open(CONFIG, "r");
    String line;
    while(fr.available()) {
        line = fr.readStringUntil('\n');
        this->readSettingsForInt(line, "printerCnt", &this->printersCnt);
        this->readSettingsForBool(line, "systemInvertDisplay", &this->systemData.invertDisplay);
        this->readSettingsForBool(line, "systemHasBasicAuth", &this->systemData.hasBasicAuth);
        this->readSettingsForString(line, "systemWebserverUsername", &this->systemData.webserverUsername);
        this->readSettingsForString(line, "systemWebserverPassword", &this->systemData.webserverPassword);
        this->readSettingsForInt(line, "systemWebserverPort", &this->systemData.webserverPort);
        this->readSettingsForBool(line, "systemUseLedFlash", &this->systemData.useLedFlash);
        this->readSettingsForInt(line, "systemResyncMinutes", &this->systemData.clockWeatherResyncMinutes);
        this->readSettingsForInt(line, "clockUtcOffset", &this->clockData.utcOffset);
        this->readSettingsForBool(line, "clockShow", &this->clockData.show);
        this->readSettingsForBool(line, "clockIs24h", &this->clockData.is24h);
        this->readSettingsForBool(line, "weatherShow", &this->weatherData.show);
        this->readSettingsForString(line, "weatherApiKey", &this->weatherData.apiKey);
        this->readSettingsForBool(line, "weatherIsMetric", &this->weatherData.isMetric);
        this->readSettingsForInt(line, "weatherCityId", &this->weatherData.cityId);
        this->readSettingsForString(line, "weatherLang", &this->weatherData.lang);
    }
    fr.close();

    // Read printer settings
    free(this->printers);
    int mallocSize = this->printersCnt;
    if (mallocSize == 0) {
        mallocSize = 0;
    }
    this->printers = (PrinterDataStruct *)malloc(mallocSize * sizeof(PrinterDataStruct));
    fr = LittleFS.open(PRINTERCONFIG, "r");
    String searchName = "";
    while(fr.available()) {
        line = fr.readStringUntil('\n');
        for(int i=0; i<this->printersCnt; i++) {
            searchName = "printer" + String(i) + "_";
            this->readSettingsForChar(line, searchName + "Name", this->printers[i].customName, 20);
            this->readSettingsForInt(line, searchName + "ApiType", &this->printers[i].apiType);
            this->readSettingsForChar(line, searchName + "ApiKey", this->printers[i].apiKey, 60);
            this->readSettingsForChar(line, searchName + "RemAddr", this->printers[i].remoteAddress, 60);
            this->readSettingsForInt(line, searchName + "RemPort", &this->printers[i].remotePort);
            this->readSettingsForBool(line, searchName + "baNeed", &this->printers[i].basicAuthNeeded);
            this->readSettingsForChar(line, searchName + "baUser", this->printers[i].basicAuthUsername, 30);
            this->readSettingsForChar(line, searchName + "baPass", this->printers[i].basicAuthPassword, 60);
            this->readSettingsForBool(line, searchName + "hasPsu", &this->printers[i].hasPsuControl);
        }
    }
    fr.close();

    // Reset printer data
    for(int i=0; i<this->printersCnt; i++) {
        BasePrinterClient::resetPrinterData(&this->printers[i]);
    }

    // Trigger updates!
    //this->getPrinterClient()->updatePrintClient();
    this->weatherClient->updateWeatherApiKey(this->weatherData.apiKey);
    this->weatherClient->updateLanguage(this->weatherData.lang);
    this->weatherClient->setMetric(this->weatherData.isMetric);
    this->weatherClient->updateCityId(this->weatherData.cityId);
    this->timeClient->setUtcOffset(this->clockData.utcOffset);
}

/**
 * @brief Set read setting line to char
 * 
 * @param line          Readed line from config
 * @param expSearch     Search expansion name
 * @param targetChar    Target char array
 * @param maxLen        Max size 
 * @return bool         true = found | false = line not match
 */
bool GlobalDataController::readSettingsForChar(String line, String expSearch, char *targetChar, size_t maxLen) {
    String searchName = expSearch +"=";
    if (line.indexOf(searchName) >= 0) {   
        String readData = line.substring(line.lastIndexOf(searchName) + searchName.length());
        readData.trim();
        MemoryHelper::stringToChar(readData, targetChar, maxLen);
        this->debugController->printLn(searchName + String(targetChar));
        return true;
    }
    return false;
}

/**
 * @brief Set read setting line to String
 * 
 * @param line          Readed line from config
 * @param expSearch     Search expansion name
 * @param targetString  Target String
 * @return bool         true = found | false = line not match
 */
bool GlobalDataController::readSettingsForString(String line, String expSearch, String *targetString) {
    String searchName = expSearch +"=";
    if (line.indexOf(searchName) >= 0) {   
        *targetString = line.substring(line.lastIndexOf(searchName) + searchName.length());
        targetString->trim();
        this->debugController->printLn(searchName + *targetString);
        return true;
    }
    return false;
}

/**
 * @brief Set read setting line to int
 * 
 * @param line          Readed line from config
 * @param expSearch     Search expansion name
 * @param targetInt     Target integer
 * @return bool         true = found | false = line not match
 */
bool GlobalDataController::readSettingsForInt(String line, String expSearch, int *targetInt) {
    String searchName = expSearch +"=";
    if (line.indexOf(searchName) >= 0) {   
        *targetInt = line.substring(line.lastIndexOf(searchName) + searchName.length()).toInt();
        this->debugController->printLn(searchName + String(*targetInt));
        return true;
    }
    return false;
}

/**
 * @brief Set read setting line to bool
 * 
 * @param line          Readed line from config
 * @param expSearch     Search expansion name
 * @param targetInt     Target bool
 * @return bool         true = found | false = line not match
 */
bool GlobalDataController::readSettingsForBool(String line, String expSearch, bool *targetBool) {
    int temp = 0;
    if(this->readSettingsForInt(line, expSearch, &temp)) {
        *targetBool = temp;
        return true;
    }
    return false;
}

/**
 * @brief Write all setting to eeprom
 */
void GlobalDataController::writeSettings() {
    // Store basic settings
    File f = LittleFS.open(CONFIG, "w");
    if (!f) {
        this->debugController->printLn("File open failed!");
    } else {
        this->debugController->printLn("Saving default settings now...");
        f.println("printerCnt=" + String(this->printersCnt));
        f.println("systemInvertDisplay=" + String(this->systemData.invertDisplay));
        f.println("systemHasBasicAuth=" + String(this->systemData.hasBasicAuth));
        f.println("systemWebserverUsername=" + this->systemData.webserverUsername);
        f.println("systemWebserverPassword=" + this->systemData.webserverPassword);
        f.println("systemWebserverPort=" + String(this->systemData.webserverPort));
        f.println("systemUseLedFlash=" + String(this->systemData.useLedFlash));
        f.println("systemResyncMinutes=" + String(this->systemData.clockWeatherResyncMinutes));
        f.println("clockUtcOffset=" + String(this->clockData.utcOffset));
        f.println("clockShow=" + String(this->clockData.show));
        f.println("clockIs24h=" + String(this->clockData.is24h));
        f.println("weatherShow=" + String(this->weatherData.show));
        f.println("weatherApiKey=" + this->weatherData.apiKey);
        f.println("weatherCityId=" + String(this->weatherData.cityId));
        f.println("weatherIsMetric=" + String(this->weatherData.isMetric));
        f.println("weatherLang=" + this->weatherData.lang);        
    }
    f.close();

    // Store printer settings
    f = LittleFS.open(PRINTERCONFIG, "w");
    if (!f) {
        this->debugController->printLn("File open failed!");
    } else {
        this->debugController->printLn("Saving printer settings now...");
        for(int i=0; i<this->printersCnt; i++) {
            f.println("printer" + String(i) + "_Name=" + String(this->printers[i].customName));
            f.println("printer" + String(i) + "_ApiType=" + String(this->printers[i].apiType));
            f.println("printer" + String(i) + "_ApiKey=" + String(this->printers[i].apiKey));
            f.println("printer" + String(i) + "_RemAddr=" + String(this->printers[i].remoteAddress));
            f.println("printer" + String(i) + "_RemPort=" + String(this->printers[i].remotePort));
            f.println("printer" + String(i) + "_baNeed=" + String(this->printers[i].basicAuthNeeded));
            f.println("printer" + String(i) + "_baUser=" + String(this->printers[i].basicAuthUsername));
            f.println("printer" + String(i) + "_baPass=" + String(this->printers[i].basicAuthPassword));
            f.println("printer" + String(i) + "_hasPsu=" + String(this->printers[i].hasPsuControl));
        }
    }
    f.close();

    // Success, let us read all again to match
    readSettings();
}

/**
 * @brief Intialize all internal data with parameters from <see ref="Configuration.h">
 */
void GlobalDataController::initDefaultConfig() {
    this->systemData.clockWeatherResyncMinutes = TIME_RESYNC_MINUTES_DELAY;
    this->systemData.hasBasicAuth = WEBSERVER_IS_BASIC_AUTH;
#ifdef DISPLAY_INVERT_DISPLAY
    this->systemData.invertDisplay = DISPLAY_INVERT_DISPLAY;
#else
    this->systemData.invertDisplay = false;
#endif
    this->systemData.useLedFlash = USE_FLASH;
    this->systemData.lastError = "";
    this->systemData.version = VERSION;
    this->systemData.webserverPassword = WEBSERVER_PASSWORD;
    this->systemData.webserverPort = WEBSERVER_PORT;
    this->systemData.webserverUsername = WEBSERVER_USERNAME;

    this->clockData.is24h = TIME_IS_24HOUR;
    this->clockData.show = DISPLAYCLOCK;
    this->clockData.utcOffset = TIME_UTCOFFSET;

    this->weatherData.apiKey = WEATHER_APIKEY;
    this->weatherData.cityId = WEATHER_CITYID;
    this->weatherData.isMetric = WEATHER_METRIC;
    this->weatherData.lang = WEATHER_LANGUAGE;
    this->weatherData.show = DISPLAYWEATHER;
}

/**
 * @brief Delete configurations from eeprom
 * @return bool     true = success | false = error
 */
bool GlobalDataController::resetConfig() {
    return LittleFS.remove(CONFIG) && LittleFS.remove(PRINTERCONFIG);
}

/**
 * @brief Return internal reference to time client
 * @return TimeClient* 
 */
TimeClient *GlobalDataController::getTimeClient() {
    return this->timeClient;
}

/**
 * @brief Return internal reference to weather client
 * @return OpenWeatherMapClient* 
 */
OpenWeatherMapClient *GlobalDataController::getWeatherClient() {
    return this->weatherClient;
}

/**
 * @brief Set LED state
 * @param value     true = enable LED | false = disable LED
 */
void GlobalDataController::ledOnOff(boolean value) {
    if (this->systemData.useLedFlash) {
        if (value) {
            digitalWrite(EXTERNAL_LED, LOW); // LED ON
        } else {
            digitalWrite(EXTERNAL_LED, HIGH);  // LED OFF
        }
    }
}

/**
 * @brief Cyclic LED flash
 * @param number        Number of cycles
 * @param delayTime     Delay between states
 */
void GlobalDataController::flashLED(int number, int delayTime) {
    for (int inx = 0; inx <= number; inx++) {
        delay(delayTime);
        digitalWrite(EXTERNAL_LED, LOW); // ON
        delay(delayTime);
        digitalWrite(EXTERNAL_LED, HIGH); // OFF
        delay(delayTime);
    }
}

/**
 * @brief Set handle to display
 * 
 * @param baseDisplayClient 
 */
void GlobalDataController::setDisplayClient(BaseDisplayClient *baseDisplayClient) {
    this->baseDisplayClient = baseDisplayClient;
}

/**
 * @brief Return handle to current display
 * 
 * @return BaseDisplayClient* 
 */
BaseDisplayClient *GlobalDataController::getDisplayClient() {
    return this->baseDisplayClient;
}

/**
 * @brief Return current configuration for the system
 * 
 * @return SystemDataStruct* 
 */
SystemDataStruct *GlobalDataController::getSystemSettings() {
    return &this->systemData;
}

/**
 * @brief Return current configuration for the clock
 * @return ClockDataStruct* 
 */
ClockDataStruct *GlobalDataController::getClockSettings() {
    return &this->clockData;
}

/**
 * @brief Return current configuration for the weather
 * @return WeatherDataStruct* 
 */
WeatherDataStruct *GlobalDataController::getWeatherSettings() {
    return &this->weatherData;
}

/**
 * @brief Returns number of printers in prntersettings
 * @return int 
 */
int GlobalDataController::getNumPrinters() {
    return this->printersCnt;
}

/**
 * @brief Return current configuration for the printers
 * @return PrinterDataStruct* 
 */
PrinterDataStruct *GlobalDataController::getPrinterSettings() {
    return this->printers;
}

/**
 * @brief Creates an new entry in printer setting table
 * @return PrinterDataStruct*   The new struct for the entry
 */
PrinterDataStruct *GlobalDataController::addPrinterSetting() {
    int mallocSize = this->printersCnt + 1;
    PrinterDataStruct *newStruct = (PrinterDataStruct *)malloc(mallocSize * sizeof(PrinterDataStruct));
    if (this->printersCnt > 0) {
        memcpy(newStruct, this->printers, this->printersCnt * sizeof(PrinterDataStruct));
    }
    free(this->printers);
    this->printers = newStruct;
    PrinterDataStruct *retStruct = &(this->printers[this->printersCnt]);
    memset(retStruct, 0, sizeof(PrinterDataStruct));
    this->printersCnt++;
    return retStruct;
}

/**
 * @brief Creates an new entry in printer setting table
 * @return PrinterDataStruct*   The new struct for the entry
 */
bool GlobalDataController::removePrinterSettingByIdx(int idx) {
    if (this->printersCnt <= idx) {
        return false;
    }
    int tSize = this->printersCnt - 1;
    int mallocSize = this->printersCnt - 1;
    if (mallocSize <= 0) {
        mallocSize = 1;
    }
    PrinterDataStruct *newStruct = (PrinterDataStruct *)malloc(mallocSize * sizeof(PrinterDataStruct));
    memset(newStruct, 0, mallocSize * sizeof(PrinterDataStruct));
    if (tSize > 0) {
        int eCnt = 0;
        for(int i=0; i<this->printersCnt; i++) {
            if (i == idx) {
                continue;
            }
            memcpy(&newStruct[eCnt], &this->printers[i], sizeof(PrinterDataStruct));
            eCnt++;
        }
        this->printersCnt = eCnt;
        free(this->printers);
        this->printers = newStruct;
    }
    return true;
}

/**
 * @brief Return a printer state as readable text
 * @param printerHandle     Handle to printer data
 * @return String 
 */
String GlobalDataController::getPrinterStateAsText(PrinterDataStruct *printerHandle) {
    switch (printerHandle->state)
    {
    case PRINTER_STATE_ERROR:
        return "Error";
    case PRINTER_STATE_STANDBY:
        return "Standby";
    case PRINTER_STATE_PRINTING:
        return "Printing";
    case PRINTER_STATE_PAUSED:
        return "Paused";
    case PRINTER_STATE_COMPLETED:
        return "Completed";
    default:
        return "Offline";
    }
}

/**
 * @brief Register an new printer client vor internal handling
 * 
 * @param id                    Type ID
 * @param basePrinterClient     Client
 */
void GlobalDataController::registerPrinterClient(int id, BasePrinterClient *basePrinterClient) {
    if (this->basePrinterCount < (id + 1)) {
        BasePrinterClient** newSet = (BasePrinterClient**)malloc((id + 1) * sizeof(BasePrinterClient*));
        memset(newSet, 0, (id + 1) * sizeof(BasePrinterClient*));
        for (int i=0; i<this->basePrinterCount; i++) {
            newSet[i] = this->basePrinterClients[i];
        }
        free(this->basePrinterClients);
        this->basePrinterClients = newSet;
        this->basePrinterCount = id + 1;
    }
    this->basePrinterClients[id] = basePrinterClient;
}

/**
 * @brief Get all registred printer clients
 * 
 * @return BasePrinterClient** 
 */
BasePrinterClient** GlobalDataController::getRegisteredPrinterClients() {
    return this->basePrinterClients;
}

/**
 * @brief Get number of registred printer clients
 * 
 * @return int 
 */
int GlobalDataController::getRegisteredPrinterClientsNum() {
    return this->basePrinterCount;
}

/**
 * @brief Get viewable type of client for printer
 * @param printerHandle     Handle to printer data
 * @return String 
 */
String GlobalDataController::getPrinterClientType(PrinterDataStruct *printerHandle) {
    for (int i=0; i<this->basePrinterCount; i++) {
        if((i == printerHandle->apiType) && (this->basePrinterClients[i] != NULL)) {
            return this->basePrinterClients[i]->getClientType();
        }
    }
    return "Unknown";
}

/**
 * @brief Sync printer with client
 * @param printerHandle     Handle to printer data
 */
void GlobalDataController::syncPrinter(PrinterDataStruct *printerHandle) {
    for (int i=0; i<this->basePrinterCount; i++) {
        if((i == printerHandle->apiType) && (this->basePrinterClients[i] != NULL)) {
            if (this->basePrinterClients[i]->isValidConfig(printerHandle)) {
                this->debugController->printLn("syncPrinter: " + String(printerHandle->lastSyncEpoch) + " | " + String(printerHandle->customName));
                this->basePrinterClients[i]->getPrinterJobResults(printerHandle);
                this->basePrinterClients[i]->getPrinterPsuState(printerHandle);
            }
        }
    }
}