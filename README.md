# Arduino-Lifx-API
A library that serves as a wrapper for the [Lifx HTTP API](https://api.developer.lifx.com/docs)
You can read more about it here: http://www.joeybabcock.me/blog/projects/arduino-lifx-api-control-library-for-esp8266/

It requires the [ArduinoJson Library](https://github.com/bblanchon/ArduinoJson)

It can query the status of the bulb to find things like color/brightness/hue, as well as toggle the power, and set the state of the bulb (change brightness, color, hue, anything...).

I need to do a few things:

1. Add multiple bulb support
2. Fix implementation of Json library(it kind of does a wierd substring to get rid of some content)
3. Esp32...
