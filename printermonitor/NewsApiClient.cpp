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


#include "NewsApiClient.h"



#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

NewsApiClient::NewsApiClient(String ApiKey, String NewsSource) {
  updateNewsClient(ApiKey, NewsSource);
}

void NewsApiClient::updateNewsClient(String ApiKey, String NewsSource) {
  mySource = NewsSource;
  myApiKey = ApiKey;
}

void NewsApiClient::updateNews() {
  JsonStreamingParser parser;
  parser.setListener(this);
  HTTPClient http;

  String apiGetData = "http://" + String(servername) + "/v2/top-headlines?sources=" + mySource + "&apiKey=" + myApiKey;
  //String apiGetData = "http://" + String(servername) + "/v2/top-headlines?country=nl&apiKey=" + myApiKey;

  Serial.println("Getting News Data");
  Serial.println(apiGetData);
  http.begin(apiGetData);
  int httpCode = http.GET();

  if (httpCode > 0) {  // checks for connection
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if(httpCode == HTTP_CODE_OK) {
      // get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      // create buffer for read
      char buff[128] = { 0 };
      // get tcp stream
      WiFiClient * stream = http.getStreamPtr();
      // read all data from server
      Serial.println("Start parsing...");
      while(http.connected() && (len > 0 || len == -1)) {
        // get available data size
        size_t size = stream->available();
        if(size) {
          // read up to 128 byte
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          for(int i=0;i<c;i++) {
            parser.parse(buff[i]); 
          }
            
          if(len > 0)
            len -= c;
          }
        delay(1);
      }
    }
    http.end();
  } else {
    Serial.println("connection for news data failed: " + String(apiGetData)); //error message if no client connect
    Serial.println();
    return;
  }
}

String NewsApiClient::getTitle(int index) {
  return news[index].title;
}

String NewsApiClient::getDescription(int index) {
  return news[index].description;
}

String NewsApiClient::getUrl(int index) {
  return news[index].url;
}

void NewsApiClient::updateNewsSource(String source) {
  mySource = source;
}

void NewsApiClient::whitespace(char c) {

}

void NewsApiClient::startDocument() {
  counterTitle = 0;
}

void NewsApiClient::key(String key) {
  currentKey = key;
}

void NewsApiClient::value(String value) {
  if (counterTitle == 10) {
    // we are full so return
    return;
  }
  if (currentKey == "title") {
    news[counterTitle].title = cleanText(value);
  }
  if (currentKey == "description") {
    news[counterTitle].description = cleanText(value);
  }
  if (currentKey == "url") {
    news[counterTitle].url = value;
    counterTitle++;
  }
  Serial.println(currentKey + "=" + value);
}

void NewsApiClient::endArray() {
}

void NewsApiClient::endObject() {
}
void NewsApiClient::startArray() {
}

void NewsApiClient::startObject() {
}

void NewsApiClient::endDocument() {
}

String NewsApiClient::cleanText(String text) {
  text.replace("’", "'");
  text.replace("“", "\"");
  text.replace("”", "\"");
  text.replace("`", "'");
  text.replace("‘", "'");
  text.replace("\\\"", "'");
  text.replace("•", "-");
  text.replace("é", "e");
  text.replace("è", "e");
  text.replace("ë", "e");
  text.replace("ê", "e");
  text.replace("à", "a");
  text.replace("â", "a");
  text.replace("ù", "u");
  text.replace("ç", "c");
  text.replace("î", "i");
  text.replace("ï", "i");
  text.replace("ô", "o");
  text.replace("…", "...");
  text.replace("–", "-");
  text.replace("Â", "A");
  text.replace("À", "A");
  text.replace("æ", "ae");
  text.replace("Æ", "AE");
  text.replace("É", "E");
  text.replace("È", "E");
  text.replace("Ë", "E");
  text.replace("Ô", "O");
  text.replace("Ö", "O");
  text.replace("œ", "oe");
  text.replace("Œ", "OE");
  text.replace("Ù", "U");
  text.replace("Û", "U");
  text.replace("Ü", "U");
  return text;
}
