
/*
  Temperature and Humidity Program

*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiGeneric.h>
#include <WiFiUdp.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiServerSecureAxTLS.h>
#include <BearSSLHelpers.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiServerSecureBearSSL.h>
#include <WiFiClientSecureAxTLS.h>
#include <WiFiClientSecureBearSSL.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFiScan.h>
#include <CertStoreBearSSL.h>
#include <ESP8266WiFiSTA.h>
#include <WiFiServer.h>
#include <WiFiServerSecure.h>

#include <PubSubClient.h>
#include <dht.h>


const char* ssid = "TechnicalHub";
const char* password = "Technicalhub@AEC";
const char* mqttServer = "18.232.46.180";
const int mqttPort = 1883;

const char* PUB_TOPIC = "dht-resp";


WiFiClient espClient;
PubSubClient mqttclient(espClient);

// INDICATION LED
int wifiLed = 16;
int mqttLed = 5;

#define CLIENT_ID "ESP_Temp_Hum"

const int dhtPin = 4;
dht dht_var;
float temperature;
float humidity;

char payload[100];


void wifisetup()
  {
    WiFi.begin(ssid,password); // Initializing the wifi    
    
    while (WiFi.status()!=WL_CONNECTED)
      {
        digitalWrite(wifiLed,1);
        delay(1000); //  waiting for some time
        Serial.println("connecting to the WiFi..");
        digitalWrite(wifiLed,0);        
      }

    Serial.println("Wifi connected");
    digitalWrite(wifiLed,1);
    
  }

void mqttconnect()
  {
    mqttclient.setServer(mqttServer, mqttPort);
    while (!mqttclient.connected())
      {
        digitalWrite(mqttLed,1);
        delay(1000);
        if(mqttclient.connect(CLIENT_ID))
          {
            Serial.println("MQTT Connected");
            digitalWrite(mqttLed,1);
            
          }
        else
          {
            Serial.println("Trying to connect to MQTT Broker");
            digitalWrite(mqttLed,0);
            delay(2000);  
          }
             
      }
  }  

void sendData()
  {
     if (mqttclient.connect(CLIENT_ID))
      {
        Serial.println("sending message");
        mqttclient.publish(PUB_TOPIC, payload); 
      } 
  }
  
void setup() 
  {
    pinMode(wifiLed,OUTPUT);
    pinMode(mqttLed,OUTPUT);
    pinMode(dhtPin,INPUT);
    
    Serial.begin(115200); // starting the serial communication.
    wifisetup();
    mqttconnect();
  }

void loop() {
   mqttclient.loop();
   if (!mqttclient.connected())
    {
      mqttconnect();      
    }

   dht_var.read11(dhtPin);
   temperature = dht_var.temperature;
   humidity = dht_var.humidity;

   String temp = String(temperature); 
   String hum  = String(humidity);
   String jsonMsg = "{";
   jsonMsg += "\"ID\":\"Temp_Hum\",";jsonMsg += "\"temperature\":"; 
   jsonMsg += temperature; jsonMsg += ",";jsonMsg += "\"humidity\":"; jsonMsg += humidity;
   jsonMsg += "}";
  
   jsonMsg.toCharArray(payload,100);

   sendData();

   delay(60000);
   
}
