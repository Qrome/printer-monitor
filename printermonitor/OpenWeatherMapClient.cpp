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

OpenWeatherMapClient::OpenWeatherMapClient(String ApiKey, int CityIDs[], int cityCount, boolean isMetric, String language, Debug *debugHandle) {
  this->debugHandle = debugHandle;
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
  WiFiClient weatherClient;
  String apiGetData = "GET /data/2.5/group?id=" + myCityIDs + "&units=" + units + "&cnt=1&APPID=" + myApiKey + "&lang=" + lang + " HTTP/1.1";

  this->debugHandle->printLn("Getting Weather Data");
  this->debugHandle->printLn(apiGetData);
  result = "";
  if (weatherClient.connect(servername, 80)) {  //starts client connection, checks for connection
    weatherClient.println(apiGetData);
    weatherClient.println("Host: " + String(servername));
    weatherClient.println("User-Agent: ArduinoWiFi/1.1");
    weatherClient.println("Connection: close");
    weatherClient.println();
  } 
  else {
    this->debugHandle->printLn("connection for weather data failed"); //error message if no client connect
    this->debugHandle->printLn("");
    return;
  }

  while(weatherClient.connected() && !weatherClient.available()) delay(1); //waits for data
 
  this->debugHandle->printLn("Waiting for data");

  // Check HTTP status
  char status[32] = {0};
  weatherClient.readBytesUntil('\r', status, sizeof(status));
  this->debugHandle->printLn("Response Header: " + String(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    this->debugHandle->print("Unexpected response: ");
    this->debugHandle->printLn(status);
    return;
  }

    // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!weatherClient.find(endOfHeaders)) {
    this->debugHandle->printLn("Invalid response");
    return;
  }

  const size_t bufferSize = 710;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  weathers[0].cached = false;
  weathers[0].error = "";
  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(weatherClient);
  if (!root.success()) {
    this->debugHandle->printLn("Weather Data Parsing failed!");
    weathers[0].error = "Weather Data Parsing failed!";
    return;
  }

  weatherClient.stop(); //stop client

  if (root.measureLength() <= 150) {
    this->debugHandle->printLn("Error Does not look like we got the data.  Size: " + String(root.measureLength()));
    weathers[0].cached = true;
    weathers[0].error = (const char*)root["message"];
    this->debugHandle->printLn("Error: " + weathers[0].error);
    return;
  }
  int count = root["cnt"];

  for (int inx = 0; inx < count; inx++) {
    weathers[inx].lat = (const char*)root["list"][inx]["coord"]["lat"];
    weathers[inx].lon = (const char*)root["list"][inx]["coord"]["lon"];
    weathers[inx].dt = (const char*)root["list"][inx]["dt"];
    weathers[inx].city = (const char*)root["list"][inx]["name"];
    weathers[inx].country = (const char*)root["list"][inx]["sys"]["country"];
    weathers[inx].temp = (const char*)root["list"][inx]["main"]["temp"];
    weathers[inx].humidity = (const char*)root["list"][inx]["main"]["humidity"];
    weathers[inx].condition = (const char*)root["list"][inx]["weather"][0]["main"];
    weathers[inx].wind = (const char*)root["list"][inx]["wind"]["speed"];
    weathers[inx].weatherId = (const char*)root["list"][inx]["weather"][0]["id"];
    weathers[inx].description = (const char*)root["list"][inx]["weather"][0]["description"];
    weathers[inx].icon = (const char*)root["list"][inx]["weather"][0]["icon"];

    this->debugHandle->printLn("lat: " + weathers[inx].lat);
    this->debugHandle->printLn("lon: " + weathers[inx].lon);
    this->debugHandle->printLn("dt: " + weathers[inx].dt);
    this->debugHandle->printLn("city: " + weathers[inx].city);
    this->debugHandle->printLn("country: " + weathers[inx].country);
    this->debugHandle->printLn("temp: " + weathers[inx].temp);
    this->debugHandle->printLn("humidity: " + weathers[inx].humidity);
    this->debugHandle->printLn("condition: " + weathers[inx].condition);
    this->debugHandle->printLn("wind: " + weathers[inx].wind);
    this->debugHandle->printLn("weatherId: " + weathers[inx].weatherId);
    this->debugHandle->printLn("description: " + weathers[inx].description);
    this->debugHandle->printLn("icon: " + weathers[inx].icon);
    this->debugHandle->printLn("");
    
  }
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
