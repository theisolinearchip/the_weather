#include "ScreenHelper.h"

ScreenHelper::ScreenHelper() {

}

void ScreenHelper::begin() {
  this->display = Adafruit_SSD1306(this->screen_width, this->screen_height, &Wire, -1);
  this->initialized = this->display.begin(SSD1306_SWITCHCAPVCC, this->screen_address);
  
  this->set_screen_generic_state(SCREEN_STATE_IDLE);
}

void ScreenHelper::display_demo() {
  if (!this->initialized) this->error_init_screen();

  this->display.clearDisplay();
  this->display.setTextSize(2); // Draw 2X-scale text
  this->display.setTextColor(SSD1306_WHITE);
  this->display.setCursor(0, 0);
  this->display.println("Test");
  this->display.display();
}

void ScreenHelper::set_screen_generic_state(int new_state) {
  this->set_screen_generic_state(new_state, "");
}

void ScreenHelper::set_screen_generic_state(int new_state, int code) {
  char code_str[1];
  itoa(code, code_str, 10);
  this->set_screen_generic_state(new_state, code_str); 
}


void ScreenHelper::set_screen_generic_state(int new_state, char* str) {
  if (!this->initialized) this->error_init_screen();

  this->acum_millis = 0;
  this->current_state = new_state;
  
  switch(this->current_state) {
    default:
    case SCREEN_STATE_IDLE:
      this->display_splash_screen();
      break;
      
    case SCREEN_STATE_WAITING_NET:
      this->display_static_message("Connecting...", 1);
      break;
      
    case SCREEN_STATE_WAITING_HOST:
      this->display_static_message("Connecting..", 1);
      break;

    case SCREEN_STATE_WAITING_FETCH:
      this->display_static_message("Wait...", 2);
      break;

    case SCREEN_STATE_ERROR:
      // String err = "Error" + (str != "" ? (" :" + str) : ". Push to retry");
      
      this->display_static_message("Error. Retry!", 1);
      break;
  }
}

void ScreenHelper::set_screen_weather_state(WeatherData weather_data) {
  if (!this->initialized) this->error_init_screen();

  this->acum_millis = 0;
  this->current_state = SCREEN_STATE_SHOW_DATA;

  // set the current weather_data to handle it on the update_screen method too
  this->current_weather_data = weather_data;

  this->display_weather_screen();
}

void ScreenHelper::update_screen(int long tick_millis) {
  if (!this->initialized) this->error_init_screen();
  
  this->acum_millis += tick_millis;

  // some views needs and update every X ticks to perform some extra graphic actions
  switch(this->current_state) {
    case SCREEN_STATE_WAITING_NET:
    case SCREEN_STATE_WAITING_HOST:
    case SCREEN_STATE_WAITING_FETCH:
      if (this->acum_millis > 500 && this->acum_millis < 1000) {
        this->display.invertDisplay(true);
      } else if (this->acum_millis > 1000) {
        this->display.invertDisplay(false);
        this->acum_millis = 0;
      }
      
      break;
  }
  
}

// ---

void ScreenHelper::error_init_screen() {
  Serial.println("error with screen during display.begin");
}

// show a dummy standard message like "connecting...", "error, try again", and so on...
void ScreenHelper::display_static_message(char* text, int size) {
  if (!this->initialized) this->error_init_screen();
  this->display.clearDisplay();
  this->display.invertDisplay(false);

  this->display.setTextSize(size);
  this->display.setTextColor(SSD1306_WHITE);
  this->display.setCursor(10, (size > 1 ? 25 : 28));
  this->display.println(text);
  
  this->display.display();
}

void ScreenHelper::display_splash_screen() {
  this->display.clearDisplay();
  this->display.invertDisplay(false);
  
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  
  this->display.fillRect(0, 0, display.width(), 44, SSD1306_INVERSE);
  
  this->display.setTextSize(2);
  this->display.setCursor(2, 2);
  this->display.println("The");
  this->display.setCursor(this->display.getCursorX() + 2, this->display.getCursorY());
  this->display.setTextSize(3);
  this->display.println("Weather");

  this->display.setTextColor(SSD1306_WHITE);
  this->display.setTextSize(1);
  this->display.setCursor(0, this->display.height() - 16);
  this->display.println("Another awesome idea");
  this->display.println("from Albert Gonzalez!");
  
  this->display.display();
}

void ScreenHelper::display_weather_screen() {

  String temp_full = String(this->current_weather_data.temp) + "c";
  int city_name_length = this->current_weather_data.city_name.length();

  String weather_description_full = this->current_weather_data.weather_description_1
    + (this->current_weather_data.weather_description_2 != "" ? ", " + this->current_weather_data.weather_description_2 : "")
    + (this->current_weather_data.weather_description_3 != "" ? ", " + this->current_weather_data.weather_description_3 : ""); // assume 1 will exist if the other two aren't empty
  int weather_description_full_length = weather_description_full.length();
  
  int country_length = this->current_weather_data.country.length();

  int city_name_width_padding = 0;
  int city_name_padding_increment = 0;
  int city_name_padding_remaining_characters = -1;
  int city_name_bottom_padding = 0;
  
  this->display.clearDisplay();
  this->display.invertDisplay(false);

  // city_name
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  if (city_name_length <= 6) {
    city_name_padding_increment = 22;
    city_name_padding_remaining_characters = 7 - city_name_length;
    city_name_bottom_padding = 4;
    this->display.setTextSize(3);
    this->display.fillRect(0, 0, display.width(), 27, SSD1306_INVERSE);    
  } else if (city_name_length <= 10) {
    city_name_padding_increment = 16;
    city_name_padding_remaining_characters = 10 - city_name_length;
    city_name_bottom_padding = 6;
    this->display.setTextSize(2);
    this->display.fillRect(0, 0, display.width(), 20, SSD1306_INVERSE);
  } else {
    city_name_bottom_padding = 8;
    this->display.setTextSize(1);
    this->display.fillRect(0, 0, display.width(), (city_name_length <= 21 ? 12 : 20), SSD1306_INVERSE);
  }
  
  while (city_name_padding_remaining_characters > 0) {
    city_name_width_padding += city_name_padding_increment;
    city_name_padding_remaining_characters--;
  }
  city_name_width_padding /= 2;  
  this->display.setCursor(city_name_width_padding + 2, 2);
  this->display.println(this->current_weather_data.city_name);

  // weather_descriptions (1 to 3)
  this->display.setCursor(2, this->display.getCursorY() + city_name_bottom_padding);
  this->display.setTextColor(SSD1306_WHITE);
  this->display.setTextSize(1);
  this->display.println(weather_description_full);

  // temperature
  // this->display.setTextColor(SSD1306_WHITE);
  this->display.setTextSize(2);
  this->display.setCursor(1, this->display.height() - 16);
  this->display.println(temp_full);
  // this->display.startscrollleft(0x00, 0x02);

  // country
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  // this->display.setTextSize(2);
  this->display.fillRect(this->display.width() - 2 - 12 * country_length, this->display.height() - 18, display.width(), this->display.height(), SSD1306_INVERSE);
  this->display.setCursor(this->display.width() - 12 * country_length, this->display.height() - 16); // height at font size 1 is 7, add some padding
  this->display.println(this->current_weather_data.country);
  
  this->display.display();

}
