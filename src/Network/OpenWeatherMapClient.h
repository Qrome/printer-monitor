#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <base64.h>
#include "../Global/DebugController.h"

class OpenWeatherMapClient {
private:
    String myCityIDs = "";
    String myApiKey = "";
    String units = "";
    String lang = "";
    
    const char* servername = "api.openweathermap.org";  // remote server we will connect to
    String result;

    typedef struct {
        String lat;
        String lon;
        String dt;
        String city;
        String country;
        String temp;
        String humidity;
        String condition;
        String wind;
        String weatherId;
        String description;
        String icon;
        boolean cached;
        String error;
    } weather;

    weather weathers[5];

    String roundValue(String value);
    DebugController *debugController;

public:
    OpenWeatherMapClient(String ApiKey, int CityID, int cityCount, boolean isMetric, String language, DebugController *debugController);
    void updateWeather();
    void updateWeatherApiKey(String ApiKey);
    void updateCityIdList(int CityIDs[], int cityCount);
    void updateLanguage(String language);
    void setMetric(boolean isMetric);

    String getWeatherResults();

    String getLat(int index);
    String getLon(int index);
    String getDt(int index);
    String getCity(int index);
    String getCountry(int index);
    String getTemp(int index);
    String getTempRounded(int index);
    String getHumidity(int index);
    String getHumidityRounded(int index);
    String getCondition(int index);
    String getWind(int index);
    String getWindRounded(int index);
    String getWeatherId(int index);
    String getDescription(int index);
    String getIcon(int index);
    boolean getCached();
    String getMyCityIDs();
    String getWeatherIcon(int index);
    String getError();
};