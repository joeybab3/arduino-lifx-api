# Arduino-Lifx-API
A library that serves as a wrapper for the [Lifx HTTP API](https://api.developer.lifx.com/docs)
You can read more about it here: https://www.joeybabcock.me/blog/projects/arduino-lifx-api-control-library-for-esp8266/

It requires the [ArduinoJson Library](https://github.com/bblanchon/ArduinoJson)

It can query the status of the bulb to find things like color/brightness/hue, as well as toggle the power, and set the state of the bulb (change brightness, color, hue, anything...).

Arduino-Lifx-Api can  be run on ESP8266 and ESP32(including M5Stack).

This library makes get requests to the lifx cloud API rather than sending LAN traffic, but I do want to implement the LAN method eventually.
