#include <LifxApi.h>

/*******************************************************************
 *  An example of lifx request that makes a request to the server  *
 *  Go to: https://cloud.lifx.com/settings for your token          *
 *  written by Joeybab3 (joeybab3 on Github)                       *
 *******************************************************************/


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>

#define API_KEY "lifx-key"  // your google apps API Token
#define SELECTOR "all" // find more selectors here https://api.developer.lifx.com/docs/selectors




WiFiClientSecure client;
LifxApi api(API_KEY, client);

int api_mtbs = 15000; //mean time between api requests
long api_lasttime;   //last time api request has been done

long subs = 0;

void setup() {

  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously
  // connected
  WiFiManager wifiManager;
  wifiManager.autoConnect();

  Serial.println("Connected to WiFi");

}

void loop() {
  if (millis() > api_lasttime + api_mtbs)  {
    Serial.println("Getting info");
    delay(1);
    if(api.getBulbInfo(SELECTOR))
    {
      Serial.println("---------Stats---------");
      Serial.print("Id: ");
      Serial.println(api.bulbinfo.id);
      Serial.print("UUID: ");
      Serial.println(api.bulbinfo.uuid);
      Serial.print("Label: ");
      Serial.println(api.bulbinfo.label);
      Serial.print("Power Status: ");
      Serial.println(api.bulbinfo.power);
      Serial.println("------------------------");

    }
    api_lasttime = millis();
  }
}