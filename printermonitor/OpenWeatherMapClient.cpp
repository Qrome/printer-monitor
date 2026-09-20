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

#include "OpenWeatherMapClient.h"
#include <WiFiClientSecure.h>

OpenWeatherMapClient::OpenWeatherMapClient(String ApiKey, int CityIDs[], int cityCount, boolean isMetric, String language) {
  updateCityIdList(CityIDs, cityCount);
  updateLanguage(language);
  myApiKey = ApiKey;
  setMetric(isMetric);
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
  WiFiClientSecure weatherClient;
  weatherClient.setTimeout(15000);
  weatherClient.setInsecure();
#ifdef ESP8266
  weatherClient.setBufferSizes(1024, 512);
#endif

  // The old /data/2.5/group endpoint is no longer available on the free plan
  // (issue #174). Use Current Weather and only the first configured city ID.
  String firstCityID = myCityIDs;
  int comma = firstCityID.indexOf(',');
  if (comma > 0) {
    firstCityID = firstCityID.substring(0, comma);
  }

  String apiGetData = "GET /data/2.5/weather?id=" + firstCityID + "&units=" + units + "&appid=" + myApiKey + "&lang=" + lang + " HTTP/1.1";

  Serial.println("Getting Weather Data");
  Serial.println(apiGetData);
  result = "";
  if (weatherClient.connect(servername, 443)) {
    weatherClient.println(apiGetData);
    weatherClient.println("Host: " + String(servername));
    weatherClient.println("User-Agent: ArduinoWiFi/1.1");
    weatherClient.println("Connection: close");
    weatherClient.println();
  } else {
    Serial.println("connection for weather data failed");
    Serial.println();
    weathers[0].error = "Connection Failed";
    return;
  }

  while (weatherClient.connected() && !weatherClient.available()) delay(1);

  Serial.println("Waiting for data");

  char status[32] = {0};
  weatherClient.readBytesUntil('\r', status, sizeof(status));
  Serial.println("Response Header: " + String(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    weathers[0].error = "HTTP Error: " + String(status);
    return;
  }

  char endOfHeaders[] = "\r\n\r\n";
  if (!weatherClient.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    weathers[0].error = "Invalid HTTP Response";
    return;
  }

  const size_t bufferSize = 2048;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  weathers[0].cached = false;
  weathers[0].error = "";

  JsonObject& root = jsonBuffer.parseObject(weatherClient);
  if (!root.success()) {
    Serial.println(F("Weather Data Parsing failed!"));
    weathers[0].error = "Weather Data Parsing failed!";
    return;
  }

  if (root.containsKey("cod") && String((int)root["cod"]) != "200") {
    weathers[0].error = root.containsKey("message") ? String((const char*)root["message"]) : "OWM API error";
    Serial.println("OpenWeatherMap API Error: " + weathers[0].error);
    return;
  }

  weatherClient.stop();

  weathers[0].lat = String((double)root["coord"]["lat"]);
  weathers[0].lon = String((double)root["coord"]["lon"]);
  weathers[0].dt = String((long)root["dt"]);
  weathers[0].city = root["name"].success() ? String((const char*)root["name"]) : "";
  weathers[0].country = root["sys"]["country"].success() ? String((const char*)root["sys"]["country"]) : "";
  weathers[0].temp = String((double)root["main"]["temp"]);
  weathers[0].humidity = String((int)root["main"]["humidity"]);
  weathers[0].condition = root["weather"][0]["main"].success() ? String((const char*)root["weather"][0]["main"]) : "";
  weathers[0].wind = String((double)root["wind"]["speed"]);
  weathers[0].weatherId = String((int)root["weather"][0]["id"]);
  weathers[0].description = root["weather"][0]["description"].success() ? String((const char*)root["weather"][0]["description"]) : "";
  weathers[0].icon = root["weather"][0]["icon"].success() ? String((const char*)root["weather"][0]["icon"]) : "";

  Serial.println("lat: " + weathers[0].lat);
  Serial.println("lon: " + weathers[0].lon);
  Serial.println("dt: " + weathers[0].dt);
  Serial.println("city: " + weathers[0].city);
  Serial.println("country: " + weathers[0].country);
  Serial.println("temp: " + weathers[0].temp);
  Serial.println("humidity: " + weathers[0].humidity);
  Serial.println("condition: " + weathers[0].condition);
  Serial.println("wind: " + weathers[0].wind);
  Serial.println("weatherId: " + weathers[0].weatherId);
  Serial.println("description: " + weathers[0].description);
  Serial.println("icon: " + weathers[0].icon);
  Serial.println();
}

String OpenWeatherMapClient::roundValue(String value) {
  float f = value.toFloat();
  int rounded = (int)(f+0.5f);
  return String(rounded);
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
