#include "JsonHelper.h"

#include "ArduinoJson.h"

JsonHelper::JsonHelper() {

}

// so in case we wanna change the amount of info parsed we should make those adjustments here...
WeatherData JsonHelper::parse_weather_data(char* json_data) {

  WeatherData weather_data;
  
  StaticJsonDocument<JSON_DOCUMENT_CAPACITY> doc;

  DeserializationError err = deserializeJson(doc, json_data);
  if (err) {
    Serial.print("deserializeJson() failed with code ");
    Serial.println(err.c_str());
  } else {
    weather_data.temp = doc["main"]["temp"].as<float>();
    
    weather_data.city_name = doc["name"].as<String>();
    if (weather_data.city_name == "null") weather_data.city_name = "";
    
    weather_data.country = doc["sys"]["country"].as<String>();
    if (weather_data.country == "null") weather_data.country = "";

    // since we're dealing with a small screen only create up to 3 "weather" elements (that's basically 3 "weather descriptions")
    weather_data.weather_description_1 = doc["weather"][0]["description"].as<String>();
    if (weather_data.weather_description_1 == "null") weather_data.weather_description_1 = "";
    weather_data.weather_description_2 = doc["weather"][1]["description"].as<String>();
    if (weather_data.weather_description_2 == "null") weather_data.weather_description_2 = "";
    weather_data.weather_description_3 = doc["weather"][2]["description"].as<String>();
    if (weather_data.weather_description_3 == "null") weather_data.weather_description_3 = "";
    
  }
  
  return weather_data;
}
