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

#pragma once
#include <ESP8266WiFi.h>
#include "libs/ArduinoJson/ArduinoJson.h"

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
  
public:
  OpenWeatherMapClient(String ApiKey, int CityIDs[], int cityCount, boolean isMetric, String language);
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
