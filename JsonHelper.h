#ifndef JsonHelper_h
#define JsonHelper_h

#include "Arduino.h"
#include "WeatherData.h"

// https://arduinojson.org/v6/assistant/
// assuming a regular "data/2.5/weather" object with 10 elements
// on the "weather" array. For ESP8266 and fetched from char*
#define JSON_DOCUMENT_CAPACITY    1536

class JsonHelper {

  public:
    JsonHelper();
    WeatherData parse_weather_data(char* data);

  private:
  
};

#endif
