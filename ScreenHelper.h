#ifndef ScreenHelper_h
#define ScreenHelper_h

#include "Arduino.h"
#include "WeatherData.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Wire.h>

#define SCREEN_STATE_IDLE              1
#define SCREEN_STATE_WAITING_NET       2
#define SCREEN_STATE_WAITING_HOST      3
#define SCREEN_STATE_WAITING_FETCH     4
#define SCREEN_STATE_SHOW_DATA         5
#define SCREEN_STATE_ERROR             -1

class ScreenHelper {

  public:
    ScreenHelper();

    void begin();
    void display_demo();
    
    void set_screen_generic_state(int new_state);
    void set_screen_generic_state(int new_state, int code);
    void set_screen_generic_state(int new_state, char* str);

    void set_screen_weather_state(WeatherData weather_data);
    
    void update_screen(int long tick_millis);

  private:
    // 128x64 OLED SCREEN
    static const int screen_address = 0x3C;
    static const int screen_width = 128;
    static const int screen_height = 64;
    
    Adafruit_SSD1306 display;

    WeatherData current_weather_data;

    int current_state;
    int long acum_millis;
    
    bool initialized = false;

    // internal display values when showing weather info
    int display_weather_name_size;

    void error_init_screen();
    void display_static_message(char* text, int size);
    void display_splash_screen();
    void display_weather_screen();
  
};

#endif
