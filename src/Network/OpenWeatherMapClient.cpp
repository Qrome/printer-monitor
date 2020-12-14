#include "OpenWeatherMapClient.h"

OpenWeatherMapClient::OpenWeatherMapClient(String ApiKey, int CityID, int cityCount, boolean isMetric, String language, DebugController *debugController, JsonRequestClient *jsonRequestClient) {
    this->debugController = debugController;
    this->jsonRequestClient = jsonRequestClient;
    this->updateCityId(CityID);
    this->updateLanguage(language);
    this->myApiKey = ApiKey;
    this->isMetric = false;
    this->setMetric(isMetric);
}

void OpenWeatherMapClient::updateWeatherApiKey(String ApiKey) {
    myApiKey = ApiKey;
}

void OpenWeatherMapClient::updateLanguage(String language) {
    lang = language;
    if (lang == "") {
        lang = "en";
    }
}

void OpenWeatherMapClient::updateWeather() {
    
    String apiGetData = "/data/2.5/group?id=" + myCityIDs + "&units=" + units + "&cnt=1&APPID=" + myApiKey + "&lang=" + lang + " HTTP/1.1";
    this->debugController->printLn("Getting Weather Data");
    this->debugController->printLn(apiGetData);
    const size_t bufferSize = 1024;
    weathers[0].cached = false;
    weathers[0].error = "";

    DynamicJsonDocument *jsonBuffer = this->jsonRequestClient->requestJson(
        PRINTER_REQUEST_GET,
        String(servername),
        80,
        "",
        apiGetData,
        "",
        bufferSize,
        true
    );
    if ((jsonBuffer == NULL) || (this->jsonRequestClient->getLastError() != "")) {
        this->debugController->printLn("Weather Data Parsing failed!");
        this->debugController->printLn(this->jsonRequestClient->getLastError());
        weathers[0].error = this->jsonRequestClient->getLastError();
        return;
    }

    int count = (*jsonBuffer)["cnt"];

    for (int inx = 0; inx < count; inx++) {
        weathers[inx].lon = (float)(*jsonBuffer)["list"][inx]["coord"]["lon"];
        weathers[inx].lat = (float)(*jsonBuffer)["list"][inx]["coord"]["lat"];
        weathers[inx].dt = (long)(*jsonBuffer)["list"][inx]["dt"];
        weathers[inx].city = (const char*)(*jsonBuffer)["list"][inx]["name"];
        weathers[inx].country = (const char*)(*jsonBuffer)["list"][inx]["sys"]["country"];
        weathers[inx].temp = (float)(*jsonBuffer)["list"][inx]["main"]["temp"];
        weathers[inx].humidity = (int)(*jsonBuffer)["list"][inx]["main"]["humidity"];
        weathers[inx].condition = (const char*)(*jsonBuffer)["list"][inx]["weather"][0]["main"];
        weathers[inx].wind = (float)(*jsonBuffer)["list"][inx]["wind"]["speed"];
        weathers[inx].weatherId = (int)(*jsonBuffer)["list"][inx]["weather"][0]["id"];
        weathers[inx].description = (const char*)(*jsonBuffer)["list"][inx]["weather"][0]["description"];
        weathers[inx].icon = (const char*)(*jsonBuffer)["list"][inx]["weather"][0]["icon"];

        this->debugController->printLn("lat: " + weathers[inx].lat);
        this->debugController->printLn("lon: " + weathers[inx].lon);
        this->debugController->printLn("dt: " + weathers[inx].dt);
        this->debugController->printLn("city: " + weathers[inx].city);
        this->debugController->printLn("country: " + weathers[inx].country);
        this->debugController->printLn("temp: " + weathers[inx].temp);
        this->debugController->printLn("humidity: " + weathers[inx].humidity);
        this->debugController->printLn("condition: " + weathers[inx].condition);
        this->debugController->printLn("wind: " + weathers[inx].wind);
        this->debugController->printLn("weatherId: " + weathers[inx].weatherId);
        this->debugController->printLn("description: " + weathers[inx].description);
        this->debugController->printLn("icon: " + weathers[inx].icon);
        this->debugController->printLn("");
    }
}

String OpenWeatherMapClient::roundValue(String value) {
    float f = value.toFloat();
    int rounded = (int)(f+0.5f);
    return String(rounded);
}

void OpenWeatherMapClient::updateCityId(int CityID) {
    int CityIDs[1];
    CityIDs[0] = CityID;
    this->updateCityIdList(CityIDs, 1);
}

void OpenWeatherMapClient::updateCityIdList(int CityIDs[], int cityCount) {
    myCityIDs = "";
    for (int inx = 0; inx < cityCount; inx++) {
        if (CityIDs[inx] > 0) {
            if (myCityIDs != "") {
                myCityIDs = myCityIDs + ",";
            }
            myCityIDs = myCityIDs + String(CityIDs[inx]); 
        }
    }
}

void OpenWeatherMapClient::setMetric(boolean isMetric) {
    if (isMetric) {
        units = "metric";
    } else {
        units = "imperial";
    }
    this->isMetric = isMetric;
}

String OpenWeatherMapClient::getWeatherResults() {
    return result;
}

String OpenWeatherMapClient::getLat(int index) {
    return weathers[index].lat;
}

String OpenWeatherMapClient::getLon(int index) {
    return weathers[index].lon;
}

String OpenWeatherMapClient::getDt(int index) {
    return weathers[index].dt;
}

String OpenWeatherMapClient::getCity(int index) {
    return weathers[index].city;
}

String OpenWeatherMapClient::getCountry(int index) {
    return weathers[index].country;
}

String OpenWeatherMapClient::getTemp(int index) {
    return weathers[index].temp;
}

String OpenWeatherMapClient::getTempRounded(int index) {
    return roundValue(getTemp(index));
}

String OpenWeatherMapClient::getHumidity(int index) {
    return weathers[index].humidity;
}

String OpenWeatherMapClient::getHumidityRounded(int index) {
    return roundValue(getHumidity(index));
}

String OpenWeatherMapClient::getCondition(int index) {
    return weathers[index].condition;
}

String OpenWeatherMapClient::getWind(int index) {
    return weathers[index].wind;
}

String OpenWeatherMapClient::getWindRounded(int index) {
    return roundValue(getWind(index));
}

String OpenWeatherMapClient::getWeatherId(int index) {
    return weathers[index].weatherId;
}

String OpenWeatherMapClient::getDescription(int index) {
    return weathers[index].description;
}

String OpenWeatherMapClient::getIcon(int index) {
    return weathers[index].icon;
}

boolean OpenWeatherMapClient::getCached() {
    return weathers[0].cached;
}

String OpenWeatherMapClient::getMyCityIDs() {
    return myCityIDs;
}

String OpenWeatherMapClient::getError() {
    return weathers[0].error;
}

String OpenWeatherMapClient::getWeatherIcon(int index)
{
    int id = getWeatherId(index).toInt();
    String W = ")";
    switch(id)
    {
        case 800: W = "B"; break;
        case 801: W = "Y"; break;
        case 802: W = "H"; break;
        case 803: W = "H"; break;
        case 804: W = "Y"; break;
        
        case 200: W = "0"; break;
        case 201: W = "0"; break;
        case 202: W = "0"; break;
        case 210: W = "0"; break;
        case 211: W = "0"; break;
        case 212: W = "0"; break;
        case 221: W = "0"; break;
        case 230: W = "0"; break;
        case 231: W = "0"; break;
        case 232: W = "0"; break;
        
        case 300: W = "R"; break;
        case 301: W = "R"; break;
        case 302: W = "R"; break;
        case 310: W = "R"; break;
        case 311: W = "R"; break;
        case 312: W = "R"; break;
        case 313: W = "R"; break;
        case 314: W = "R"; break;
        case 321: W = "R"; break;
        
        case 500: W = "R"; break;
        case 501: W = "R"; break;
        case 502: W = "R"; break;
        case 503: W = "R"; break;
        case 504: W = "R"; break;
        case 511: W = "R"; break;
        case 520: W = "R"; break;
        case 521: W = "R"; break;
        case 522: W = "R"; break;
        case 531: W = "R"; break;
        
        case 600: W = "W"; break;
        case 601: W = "W"; break;
        case 602: W = "W"; break;
        case 611: W = "W"; break;
        case 612: W = "W"; break;
        case 615: W = "W"; break;
        case 616: W = "W"; break;
        case 620: W = "W"; break;
        case 621: W = "W"; break;
        case 622: W = "W"; break;
        
        case 701: W = "M"; break;
        case 711: W = "M"; break;
        case 721: W = "M"; break;
        case 731: W = "M"; break;
        case 741: W = "M"; break;
        case 751: W = "M"; break;
        case 761: W = "M"; break;
        case 762: W = "M"; break;
        case 771: W = "M"; break;
        case 781: W = "M"; break;
        
        default:break; 
    }
    return W;
}

String OpenWeatherMapClient::getTempSymbol() {
  return this->getTempSymbol(false);
}

String OpenWeatherMapClient::getTempSymbol(boolean forHTML) {
    String rtnValue = "F";
    if (this->isMetric) {
        rtnValue = "C";
    }
    if (forHTML) {
        rtnValue = "&#176;" + rtnValue;
    } else {
        rtnValue = "°" + rtnValue;
    }
    return rtnValue;
}

String OpenWeatherMapClient::getSpeedSymbol() {
    String rtnValue = "mph";
    if (this->isMetric) {
        rtnValue = "kph";
    }
    return rtnValue;
}