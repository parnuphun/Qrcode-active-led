
// Common
#include <iostream> 
#include <string.h>
#include <string> 

// WiFi & API
#include <ESP8266WiFi.h> // Connect WiFi
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h> // Call API 
#include <ArduinoJson.h> // Get Response API from JSON 

// DHT 
#include "DHT.h"
#define DHTPIN 13 // Defind Pin
#define DHTTYPE DHT22 // Defind DHT Type
DHT dht(DHTPIN, DHTTYPE);

// OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <qrcode.h>
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#if (SSD1306_LCDHEIGHT != 64) // Check Err OLED
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// QR Code
const int qrCodeVersion = 3;
const int pixelSize = 2;

// ?? 
ESP8266WebServer server;

// Setup WiFi Name And Password
char* ssid = "zm";
char* password = "maizmaiz";

// Defind Pin
int sw = 12; // Switch on off LED (Common)
int swQrCode = 16 ; // Switch show Qr code
int status_led; // status led 

// API
WiFiClient client;
HTTPClient http;
char* host = "128.199.86.217";
int apiPort = 4020;


void setup()
{
  // Setup Pin
  pinMode(pin_led, OUTPUT);
  pinMode(sw, INPUT);
  pinMode(swQrCode, INPUT);

  // Setup OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.display();
  delay(2000);
  display.clearDisplay();
  
  // Connect WiFi
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

// Update Temp And Humidity Data every 1 minuit 
unsigned long interval = 60000;  
unsigned long previousMillis = 0;
// Update OLED (Reset) every 2 minuit
unsigned long interval2 = 120000; / 
unsigned long previousMillis2 = 0;

void loop(){
  API_getLedStatus(); // get current led status from db 
  
  // update temp and humidity data
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    API_InsertDHTData();
  }

  // clear display 
  unsigned long currentMillis2 = millis();
  if (currentMillis2 - previousMillis2 >= interval2) {
      previousMillis2 = currentMillis2;
      Serial.println("Reset OLED !!");
      display.display();
      delay(2000);
      display.clearDisplay();
  }

  // toggle led (common)
  if (digitalRead(sw) == 0) {
    toggleLED();
    delay(500);
  }

  // show qr code
  if (digitalRead(swQrCode) == 0) {
    Serial.println("generated !!");
    showQRCode("http://" +String(host)+ ":" +String(apiPort)+ "/updateLedStatus/" + String(!digitalRead(pin_led)));
    delay(500);
  }
  server.handleClient();
}


// get current led status
void API_getLedStatus(){
  String api = "http://"+String(host)+":"+String(apiPort)+"/getCurrentLedStatus";
  http.begin(client, api.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, http.getString());
    if (!error) {
      int led_id = doc["data"]["Iot_Lab_LED_Id"];
      int led_status = doc["data"]["Iot_Lab_LED_Status"];
//      Serial.println("Led ID: " + String(led_id));
//      Serial.println("Led Status: " + String(led_status));
      digitalWrite(pin_led,int(led_status));
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


// update led status
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

// insert dht data 
void API_InsertDHTData(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.println("temperature: " + String(t));
  Serial.println("humidity: " + String(h));

  String api = "http://" +String(host)+ ":" +String(apiPort)+ "/updateTempAndHumidity/" + String(t) +"/"+String(h);

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

// show qr code
void showQRCode(String qrCodeString) {
  QRCode qrcode;
  Serial.println(qrCodeString);
  uint8_t qrcodeBytes[qrcode_getBufferSize(qrCodeVersion)];
  Serial.println(String(qrcodeBytes[qrcode_getBufferSize(qrCodeVersion)]));
  // Generate qr code 
  qrcode_initText(&qrcode, qrcodeBytes, qrCodeVersion, ECC_LOW,
                  qrCodeString.c_str());
  display.clearDisplay(); 

  // set oled fill
  int startX = (SCREEN_WIDTH - (qrcode.size * pixelSize) - (pixelSize * 2)) / 2;
  int startY = (SCREEN_HEIGHT - (qrcode.size * pixelSize) - (pixelSize * 2)) / 2;
  int qrCodeSize = qrcode.size;

  display.fillRect(startX, startY, (qrCodeSize * pixelSize) + (pixelSize * 2),
                   (qrCodeSize * pixelSize) + (pixelSize * 2), WHITE);

  // render qrcode to display using loop 
  for (uint8_t y = 0; y < qrCodeSize; y++) {
    for (uint8_t x = 0; x < qrCodeSize; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        display.fillRect(x * pixelSize + startX + pixelSize,
                         y * pixelSize + startY + pixelSize, pixelSize,
                         pixelSize, BLACK);
      }
    }
  }
  display.display();
}
