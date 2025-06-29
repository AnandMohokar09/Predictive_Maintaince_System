#define BLYNK_TEMPLATE_ID "TMPL3ZhYXgUc8"
#define BLYNK_TEMPLATE_NAME "Predictive Maintenance System"
#define BLYNK_AUTH_TOKEN "gBvS_JuGYh64GQRh3utTTWrFFPLmZ2fP"

String receivedData = ""; 
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <BlynkSimpleEsp8266.h>

const int flashButtonPin = 0; 

void setup() 
{
  Serial.begin(9600);
  pinMode(flashButtonPin, INPUT);
  pinMode(D8, OUTPUT);
  pinMode(flashButtonPin, INPUT);
  WiFiManager wifiMn;
  if (!wifiMn.autoConnect("Esp Web Portal")) 
  {
    Serial.println("Failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  Serial.println("Connected to Wi-Fi!");
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}

void loop() 
{
  Blynk.run();
  receiveData();
  if (digitalRead(flashButtonPin) == LOW) 
  {
    WiFiManager wifiMn;
    wifiMn.resetSettings();
    ESP.reset();
  }
}

void receiveData() 
{
  while (Serial.available() > 0) 
  {
    char incomingChar = Serial.read();
    if (incomingChar == '<') 
    {
      
      receivedData = "";
    } 
    else if (incomingChar == '>') 
    {
      parseData(receivedData);
    } 
    else 
    {
      receivedData += incomingChar;
    }
  }
}

void parseData(String data) 
{
  float intvalue = data.substring(0, data.indexOf(',')).toFloat();
  data.remove(0, data.indexOf(',') + 1); // P Tank
  Blynk.virtualWrite(V0, intvalue);
  int intvalue1 = data.substring(0, data.indexOf(',')).toInt();
  data.remove(0, data.indexOf(',') + 1); // P Tank
  Blynk.virtualWrite(V1, intvalue1);
}
