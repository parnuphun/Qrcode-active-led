
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <iostream>
#include <string.h>
#include <string> 

#include "DHT.h"
#define DHTPIN 13
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

ESP8266WebServer server;
uint8_t pin_led = 2;
char* ssid = "OnlyMyKPK";
char* password = "00000000";
int sw = 12;
int status_led;
char* host = "192.168.43.133";
WiFiClient client;
HTTPClient http;
int apiPort = 4020;
void setup()
{
  pinMode(pin_led, OUTPUT);
  pinMode(sw, INPUT);
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("."); 
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  dht.begin();

}

unsigned long interval = 10000; // 10 minutes in milliseconds
unsigned long previousMillis = 0;

void loop(){
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    API_InsertDHTData();
    Serial.println(" IN IN ");
  }
  
  API_getLedStatus();
  if (digitalRead(sw) == 0) {
    toggleLED();
    delay(500);
  }
  server.handleClient();
}

void API_getLedStatus(){
  String api = "http://"+String(host)+":"+String(apiPort)+"/ledStatus";
  http.begin(client, api.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, http.getString());
    if (!error) {
      int led_id = doc["data"]["MiniProject_Iot_Led_Id"];
      int led_status = doc["data"]["MiniProject_Iot_Led_Status"];
//      Serial.println("Led ID: " + String(led_id));
//      Serial.println("Led Status: " + String(led_status));
//      digitalWrite(pin_led,int(led_status));
    } else {
      Serial.println("Deserialization failed: " + String(error.c_str()));
    }
  } else {
    Serial.println("HTTP Request failed with code: " + String(httpResponseCode));
  }
  
  http.end();
}

void toggleLED(){
  digitalWrite(pin_led, !digitalRead(pin_led));
  API_UpdateLedStatus(String(digitalRead(pin_led)));
}


void API_UpdateLedStatus(String ledStatus){
  Serial.println("state: " + String(ledStatus));

  String api = "http://" +String(host)+ ":" +String(apiPort)+ "/updateLedStatus/" + String(ledStatus);

  Serial.println("api: " + String(api));
  http.begin(client, api.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, http.getString());
    if (!error) {
      String msg = doc["msg"];
      Serial.println("Message: " + msg);
      int state = ledStatus.toInt();
      digitalWrite(pin_led,state);
    } else {;
      Serial.println("Deserialization failed: " + String(error.c_str()));
    }
  } else {
    Serial.println("HTTP Request failed with code: " + String(httpResponseCode));
  }
  
  http.end();
}

void API_InsertDHTData(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.println("temperature: " + String(t));
  Serial.println("humidity: " + String(h));

  String api = "http://" +String(host)+ ":" +String(apiPort)+ "/updateTemp/" + String(t) +"/"+String(h);

  Serial.println("api: " + String(api));
  http.begin(client, api.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, http.getString());
    if (!error) {
      String msg = doc["msg"];
      Serial.println("Message: " + msg);
    } else {;
      Serial.println("Deserialization failed: " + String(error.c_str()));
    }
  } else {
    Serial.println("HTTP Request failed with code: " + String(httpResponseCode));
  }
  
  http.end();
}
