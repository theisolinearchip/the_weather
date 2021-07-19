# The Weather

I wrote a small state machine to use with a **Sparkfun ESP8266 Thing Dev** that performs only ONE operation:

### Fetch random weather data from the [OpenWeather Api](https://openweathermap.org/api) when the user hits a big red button.

(well, actually it gets the info from a custom script on my own server that connects with OpenWeather and performs all the "select a random city ID" operations, but the idea is the same).

There's more information about the project, how it works and how it started (as a **stupid Slack command**, by the way) on my [Hackaday.io project page](https://hackaday.io/project/180861-the-weather).

## Overview

- **weather.ino** is the classic Arduino file with the main loop. It's a simple state machine that works from an **idle** state to a **displaying info** one (among with a couple of "connecting" and "fetching data from remote host").
- The _connection_ is done as a regular "send request example" (check the [ESP8266Wifi library page](https://esp8266-arduino.readthedocs.io/en/latest/esp8266wifi/readme.html) for more info). In my case I'm using a remote script to fetch a random city id from the OpenWeather API + getting the weather for that city (changing this to connect to the OpenWeather API itself should be easy - actually I did it before this and it worked without any problem).
- The remote script is not here, maybe I'll add it in the future, but it's basically a raw PHP script that _connects-and-gets_ some data from a standard OpenWeather API endpoint.
- There's a couple of helpers (**JsonHelper** and **ScreenHelper**) that deals with the data parsing and data displaying. All the screen-related and data-related operations are there (**WeatherData.h** also contains a small struct to add the "interesting" data for the screen).

## TODO
- Avoid the serial connection by default (used only for debugging)
- I need to improve my C and C++ skills (practise, practise!)
- Some visual improvements could be made to the **display state**: switching between different "views" (with different info) making a custom homemade text-scrolling system, tinkering a little bit more with the Adafruit library... As a quick and simple proof of concept this works pretty fine, sure, but maybe I'll add some extra weather data to each request.
- Use less String objects (for things like the screen control it's really useful, and since the project itself it's actually pretty small, there's no related performance issues)
