#ifndef LifxApi_h
#define LifxApi_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Client.h>

#define HOST "api.lifx.com"
#define SSL_PORT 443
#define HANDLE_MESSAGES 1
#define MAX_BUFFER_SIZE 1250

struct Color{
	float brightness;
	float hue;
	long kelvin;
};

struct BulbInfo{
  String id;
  String uuid;
  String label;
  bool connected;
  String power;
  Color color;
};

class LifxApi
{
  public:
    LifxApi (String apiKey, Client &client);
    String sendGetToLifx(String command);
    bool getBulbInfo(String selector);
    BulbInfo bulbinfo;
	//const char* fingerprint = "‎9D:01:5C:8E:FD:4D:DF:71:A4:99:CE:29:93:40:3F:5F:EE:74:d0:95";
    const char* fingerprint = "3F:AE:17:DA:03:65:45:BE:CE:77:2F:8A:DC:5B:C9:08:98:7E:9E:E7";  //Lifx https Certificate
	

  private:
    //JsonObject * parseUpdates(String response);
    String _apiKey;
    Client *client;
    const int maxMessageLength = 1500;
    bool checkForOkResponse(String response);
};

#endif
