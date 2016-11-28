#include <LifxApi.h>

/*******************************************************************
 *  An example of lifx request that makes a request to the server  *
 *  Go to: https://cloud.lifx.com/settings for your token          *
 *  written by Joeybab3 (joeybab3 on Github)                       *
 *******************************************************************/


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
MDNSResponder mdns;
#include <ArduinoOTA.h>

#define API_KEY "cabbdc10a24eeb6a7b971691b29d4ed952b4db9876d1d31d129c5451896f7ed4"  // your google apps API Token
#define SELECTOR "all" // find more selectors here https://api.developer.lifx.com/docs/selectors

#define SERIAL_DATA_THRESHOLD_MS 500
#define SERIAL_ERROR_TIMEOUT "E: Serial"
#define ETHERNET_ERROR_DHCP "E: DHCP"
#define ETHERNET_ERROR_CONNECT "E: Connect"

void handleSerialRead();
void ethConnectError();

WiFiClientSecure client;
LifxApi api(API_KEY, client);

int api_mtbs = 15000; //mean time between api requests
long api_lasttime;   //last time api request has been done

String lastCmd;

#define HTTPPORT 88

ESP8266WebServer server(HTTPPORT);

void handleRoot();
void handleCmd();
void handleNotFound();
void handleResponse();
void handleGet();
void handleGt();

void setup()
{
  Serial.begin(115200);
  WiFiManager wifiManager;

  wifiManager.autoConnect();

  Serial.println("connected to WiFi");
  if (mdns.begin("LifxController", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  ArduinoOTA.setHostname("sonos");
  ArduinoOTA.setPort(8266);
  
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /* WebServer stuff */
  server.on("/", handleRoot);
  server.on("/cmd", handleCmd);
  server.on("/get", handleGt);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("HTTP server started on ");
  Serial.println(HTTPPORT);
}

void ethConnectError()
{
  Serial.println(ETHERNET_ERROR_CONNECT);
  Serial.println("Wifi died.");
}

void loop()
{
  ArduinoOTA.handle();
  server.handleClient();
}

String handleGet(String cmd)
{
    api.getBulbInfo(SELECTOR);
    Serial.println("Handling command " + cmd);
    if (cmd == "brightness")
    {
      return String(api.bulbinfo.brightness);
    }
    else if (cmd == "hue")
    {
      return String(api.bulbinfo.color.hue);
    }
    else if (cmd == "kelvin")
    {
      return String(api.bulbinfo.color.kelvin);
    }
    else if (cmd == "saturation")
    {
      return String(api.bulbinfo.color.saturation);
    }
    else if(cmd == "power")
    {
      return String(api.bulbinfo.power);
    }
    else if(cmd == "label")
    {
      return String(api.bulbinfo.label);
    }
    else if(cmd == "id")
    {
      return String(api.bulbinfo.id);
    }
}

void handleInput(String cmd)
{
    // Play
    Serial.println("Handling command " + cmd);
    if (cmd == "tp" || cmd == "toggle" || cmd == "togglepower" || cmd == "power")
    {
      api.togglePower("id:"+api.bulbinfo.id);
    }
    // Pause
    else if (cmd == "t" || cmd == "test")
    {
      Serial.println("test cmd recieved");
    }
}

/* WebServer Stuff */

void handleRoot() {
  api.getBulbInfo(SELECTOR);
  int brightness = api.bulbinfo.brightness;
  String msg = "<html>\n";
  msg += "<head>\n";
  msg += "<title>ESP8266 Lifx Controller</title>\n";
  msg += "<link rel=\"stylesheet\" type=\"text/css\" href=\"http://joeybabcock.me/iot/hosted/hosted-lifx.css\">";
  msg += "<script src=\"https://code.jquery.com/jquery-3.1.1.min.js\"></script>\n";
  msg += "<script src=\"http://joeybabcock.me/iot/hosted/hosted-lifx.js\"></script>\n";
  msg += "</head>\n";
  msg += "<body>\n";
  msg += "<div id=\"container\">\n";
  msg += "<h1>Lifx - Esp8266 Web Controller!</h1>\n";
  msg += "<p id=\"linkholder\"><a href=\"#\" onclick=\"sendCmd('pr');\"><img src=\"http://joeybabcock.me/iot/hosted/rw.png\"/></a> \n";
  msg += "<a href=\"#\" onclick=\"sendCmd('pl');\"><img src=\"http://joeybabcock.me/iot/hosted/play.png\"/></a> \n";
  msg += "<a href=\"#\" onclick=\"sendCmd('pa');\"><img src=\"http://joeybabcock.me/iot/hosted/pause.png\"/></a> \n";
  msg += "<a href=\"#\" onclick=\"sendCmd('nx');\"><img src=\"http://joeybabcock.me/iot/hosted/ff.png\"/></a></p>\n";
  msg += "<h3>Volume: <span id=\"brt\">"+String(brightness)+"</span><input type=\"hidden\" id='brightness' value='"+String(brightness)+"' onchange=\"setBrightness(this.value)\"/></h3><br/>\n";
  msg += "<input type=\"range\" class=\"slider\"  min=\"0\" max=\"99\" value=\""+String(brightness)+"\" name=\"volume-slider\" id=\"volume-slider\" onchange=\"setVolume(this.value)\" />\n";
  msg += "<p>Server Response:<div id=\"response\" class=\"response\"></div></p>\n";
  msg += "<p><form action=\"/\" method=\"get\" id=\"console\"><input placeholder=\"Enter a command...\" type=\"text\" id='console_text'/></form></p>\n";
  msg += "<script>var intervalID = window.setInterval(getBrightness, 5000);\n$('#console').submit(function(){parseCmd($(\"#console_text\").val());\nreturn false;\n});\n</script>\n";
  msg += "</div>\n";
  msg += "<div id=\"tips\"></div>\n";
  msg == "</body>\n";
  msg += "</html>\n";
  server.send(200, "text/html", msg);
}

void handleCmd(){
  for (uint8_t i=0; i<server.args(); i++){
    if(server.argName(i) == "cmd") 
    {
      lastCmd = server.arg(i);
      handleInput(lastCmd);
    }
  }
  handleResponse();
}

void handleGt(){
  String resp;
  for (uint8_t i=0; i<server.args(); i++){
    if(server.argName(i) == "cmd") 
    {
      lastCmd = server.arg(i);
      resp = handleGet(lastCmd);
    }
  }
  handleGetResponse(resp);
}

void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);

}

void handleResponse() {
      server.send(200, "text/html", "Worked("+lastCmd+")<br/>");
      Serial.println("Got client.");
}

void handleGetResponse(String response) {
      server.send(200, "text/html", response);
      Serial.println("Got client.");
}
