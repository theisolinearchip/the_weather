#include <ESP8266WiFi.h>
#include "ScreenHelper.h"
#include "JsonHelper.h"
#include "WeatherData.h"
#include "MD5.h"
#include "credentials.h"

// PINS
#define BUTTON_PIN                      4

// STATE
#define STATE_IDLE                     1

#define STATE_CONNECT_NET              2
#define STATE_WAITING_NET              3

#define STATE_CONNECT_HOST             4
#define STATE_WAITING_HOST             5

#define STATE_ASK_FETCH                6
#define STATE_WAITING_FETCH            7

#define STATE_SHOWING                  8
#define STATE_ERROR                    -1

// STATE CONSTANTS
#define CONNECTION_TIMEOUT_MILLIS      10000 // general timeout value for every network related operation
#define RESET_STATE_MILLIS             5000  // when prompting an error, time before going back to idle state

WiFiClient client;
ScreenHelper screen_helper;
JsonHelper json_helper;

int long current_millis = 0;
int long previous_millis = 0;
int long tick_millis = 0;
int long acum_millis = 0;

int previous_button_state = HIGH;
int current_button_state = HIGH;

int current_state = STATE_IDLE;

char* json_data;

// ------------

void set_state(int new_state) {
  // reset the acum
  acum_millis = 0;

  int previous_state = current_state;
  current_state = new_state;

  Serial.print("new state: "); Serial.println(current_state);

  switch (current_state) {
    default:
      break;
    case STATE_IDLE:
      screen_helper.set_screen_generic_state(SCREEN_STATE_IDLE);
      break;

    case STATE_WAITING_NET:
      screen_helper.set_screen_generic_state(SCREEN_STATE_WAITING_NET);
      break;

    case STATE_WAITING_HOST:
      screen_helper.set_screen_generic_state(SCREEN_STATE_WAITING_HOST);
      break;

    case STATE_WAITING_FETCH:
      screen_helper.set_screen_generic_state(SCREEN_STATE_WAITING_FETCH);
      break;

    case STATE_SHOWING: {
      // parse json data and pass (json_data)
      WeatherData weather_data = json_helper.parse_weather_data(json_data);
      screen_helper.set_screen_weather_state(weather_data);
      break;
    }

    case STATE_ERROR:
      screen_helper.set_screen_generic_state(SCREEN_STATE_ERROR, previous_state);
      break;

  }
}

// STATE_IDLE:
void check_action_requested() {
  if (current_button_state == LOW && previous_button_state == HIGH) {
    set_state(STATE_CONNECT_NET);
  }
}

void check_restart_state() {
  acum_millis += tick_millis;
  if (acum_millis >= RESET_STATE_MILLIS) {
    set_state(STATE_IDLE);
  }
}

// STATE_CONNECT_NET
void connect_network() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(NETWORK_NAME, NETWORK_PASS);
    set_state(STATE_WAITING_NET);
  } else {
    set_state(STATE_CONNECT_HOST);
  }
}

// STATE_WAITING_NET
void check_network_connection() {
  acum_millis += tick_millis;
  if (WiFi.status() != WL_CONNECTED) {
    if (acum_millis > CONNECTION_TIMEOUT_MILLIS) set_state(STATE_ERROR);
  } else {
    set_state(STATE_CONNECT_HOST);
  }
}

// STATE_CONNECT_HOST
void connect_host() {
  if (!client.connected()) {
    client.connect(openweather_service_host, 80);
    set_state(STATE_WAITING_HOST);
  } else {
    set_state(STATE_ASK_FETCH);
  }
}

// STATE_WAITING_HOST
void check_host_connection() {
  acum_millis += tick_millis;
  if (!client.connected()) {
    if (acum_millis > CONNECTION_TIMEOUT_MILLIS) set_state(STATE_ERROR);
  } else {
    set_state(STATE_ASK_FETCH);
  }
}

// STATE_ASK_FETCH
void fetch_data() {

  // get key and hash
  char key[5];
  itoa(random(10000, 100000), key, 10);

  char* hashable = new char[5 + openweather_service_secret_size];
  strcpy(hashable, key);
  strncat(hashable, openweather_service_secret, openweather_service_secret_size);

  // yup, maybe md5 is not the safest hash to use here...
  unsigned char* hash = MD5::make_hash(hashable);
  char *md5_str = MD5::make_digest(hash, 16);

  delete hash;
  
  Serial.println(hashable);
  Serial.println(md5_str);
  
  // build request
  char* request_get_str = new char[4 + openweather_service_endpoint_size + 5 + 5 + 6 + 32 + 9];
  strcpy(request_get_str, "GET ");
  strncat(request_get_str, openweather_service_endpoint, openweather_service_endpoint_size);
  strncat(request_get_str, "?key=", 5);
  strncat(request_get_str, key, 5);
  strncat(request_get_str, "&hash=", 6);
  strncat(request_get_str, md5_str, 32); // 128 bits for md5
  strncat(request_get_str, " HTTP/1.1", 9);

  client.println(request_get_str);
  Serial.println(request_get_str);

  delete request_get_str;
  delete md5_str;

  char* request_host_str = new char[6 + openweather_service_host_size];
  strcpy(request_host_str, "Host: ");
  strncat(request_host_str, openweather_service_host, openweather_service_host_size);

  client.println(request_host_str);

  delete request_host_str;

  client.println();

  set_state(STATE_WAITING_FETCH);
}

// STATE_WAITING_FETCH
void check_fetched_data() {
  acum_millis += tick_millis;
  if (acum_millis > CONNECTION_TIMEOUT_MILLIS) set_state(STATE_ERROR);
  else if (client.available()) {
    acum_millis = 0;

    bool found_json = false;
    while (!found_json && client.available() > 0) {
      char byte_fetched = client.read();
      found_json = (byte_fetched == '{');
    }

    if (found_json) {
      /*
       * I'm trying to avoid using String, but here it helps fitting an undetermined json string into
       * it's own structure. Needless to say this structure is added to the char* json_data as a new
       * pointer (each time we have new info we remove the old info and re-use the pointer name)
       */
      String line = client.readStringUntil('\r');

      int line_length = line.length();
      if (line_length > 0) {
        delete[] json_data;
        json_data = new char[1 + line_length];

        char line_buffer[line_length + 1]; // leave space for the \0 terminating char. Otherwise the last line char will be removed to leave the space
        line.toCharArray(line_buffer, sizeof(line_buffer));

        strcpy(json_data, "{");
        strncat(json_data, line_buffer, line_length);

        set_state(STATE_SHOWING);
      }
    }
    
  }
}


// ------------

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  current_button_state = digitalRead(BUTTON_PIN);
  previous_button_state = current_button_state;

  screen_helper.begin();
  set_state(STATE_IDLE);
  current_millis = millis();

}

void loop() {
  current_millis = millis();
  tick_millis = current_millis - previous_millis;

  current_button_state = digitalRead(BUTTON_PIN);

  switch (current_state) {
    default:
    case STATE_IDLE:
      check_action_requested();
      break;

    case STATE_CONNECT_NET:
      connect_network();
      break;

    case STATE_WAITING_NET:
      check_network_connection();
      break;

    case STATE_CONNECT_HOST:
      connect_host();
      break;

    case STATE_WAITING_HOST:
      check_host_connection();
      break;

    case STATE_ASK_FETCH:
      fetch_data();
      break;

    case STATE_WAITING_FETCH:
      check_fetched_data();
      break;

    case STATE_SHOWING:
      check_action_requested();
      break;

    case STATE_ERROR:
      check_action_requested();
      check_restart_state();
      break;
  }

  previous_millis = current_millis;
  previous_button_state = current_button_state;

  screen_helper.update_screen(tick_millis);
}
