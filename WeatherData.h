#ifndef WeatherData_h
#define WeatherData_h

struct WeatherData {

    // yup, some Strings here (easy to handle when working with the screen...)
    String city_name;
    String country;
    String weather_description_1;
    String weather_description_2;
    String weather_description_3;
    
    float temp;
    float temp_min;
    float temp_max;
    int pressure;
    int humidity;
};

#endif
