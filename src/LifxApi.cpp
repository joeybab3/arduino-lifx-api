#include "LifxApi.h"

LifxApi::LifxApi(String apiKey, Client &client)	{
  _apiKey = apiKey;
  this->client = &client;
}

String LifxApi::sendGetToLifx(String command) {
  String ourHeaders="Authorization: Bearer " + _apiKey;
  String headers="";
  String body="";
  bool finishedHeaders = false;
  bool currentLineIsBlank = true;
	long now;
	bool avail;
	// Connect with Lifx api over ssl
	if (client->connect(HOST, SSL_PORT)) {
		Serial.println("...Connected to server");
		String a="";
		char c;
		int ch_count=0;
		client->print(String("GET ") + command + " HTTP/1.1\r\n" +
               "Host: " + HOST + "\r\n" + 
               "Authorization: Bearer cdd0291693115a96d23dc7bc8d9c6718c6e2581f003b3fbd79d5af74ddde12dd \r\n" +
               "Connection: close\r\n\r\n");
		now=millis();
		delay(1);
		avail=false;
		Serial.println("Sent CMD.");
		while (millis()-now<3000) {
			while (client->available()) {
				char c = client->read();
				//Serial.write(c);

				if(!finishedHeaders){
				  if (currentLineIsBlank && c == '\n') {
					finishedHeaders = true;
					Serial.println("Finished Headers.");
				  }
				  else{
					headers = headers + c;

				  }
				} 
				else {
					if (ch_count < maxMessageLength)  {
						body=body+c;
						ch_count++;
					}
					else{
						Serial.println("Max Message Length.");
					}
				}

				if (c == '\n') {
				  currentLineIsBlank = true;
				}else if (c != '\r') {
				  currentLineIsBlank = false;
				}

						avail=true;
			}
			yield();
			
			if (avail) {
				Serial.println("Body:");
				Serial.println(body);
				Serial.println("END");
				break;
			}
		}
	}
	else

    //int lastCharIndex = body.lastIndexOf("}");

	//return body.substring(0,lastCharIndex+1);
	//Serial.println("Finished.");
  return body;
}

bool LifxApi::getBulbInfo(String selector){
  String command="https://api.lifx.com/v1/lights/"+selector; //try 'all' if you don't know what this is or: https://api.developer.lifx.com/docs/selectors
  String response = sendGetToLifx(command);       //recieve reply from Lifx
  Serial.println("Got info.");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(response);
  if(root.success()) {
	  Serial.println("Root success.");
      String id = root["id"];
      String uuid = root["uuid"];
      String label = root["label"];
      long connected = root["connected"];
      String power = root["power"];

      bulbinfo.id = id;
      bulbinfo.uuid = uuid;
      bulbinfo.label = label;
      bulbinfo.connected = connected;
      bulbinfo.power = power;

      return true;
  }
  Serial.println("Root failed.");
  return false;
}
