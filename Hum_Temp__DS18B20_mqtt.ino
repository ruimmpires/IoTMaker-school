#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>
//#include "DHT.h"
//#include <Arduino.h>
#include </home/rpires/Arduino/dht11/Hum_Temp__DS18B20_mqtt/DS18B20.h>

// Update these with values suitable for your network.
const char* ssid = "ESCRITORIO";
const char* password = "g0mesp1res";
const char* mqtt_server = "192.168.1.201";
#define mqtt_port 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_PUBLISH "home/watertemp/1/tx"
#define MQTT_RECEIVER "home/watertemp/1/rx"

float t = 0;
DS18B20 myDS18B20(2); //ESP8266 pin D4, GPIO2
char* hour_and_date; 
//#define DHTPIN 4    //Fisicamente é o pino 2 da board // what digital pin we're connected to
//#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
//DHT dht(DHTPIN, DHTTYPE);
//int h_before = 0;
float t_before = 0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    // The function randomSeed(seed) resets Arduino’s pseudorandom number generator.
    //randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //Connect to NTP and acquire time
    configTime(1 * 3600, 1, "pool.ntp.org", "time.nist.gov");
    Serial.println("\nWaiting for time");
    while (!time(nullptr)) {
      Serial.print(".");
      delay(500);
    }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      time_t now = time(nullptr);
      hour_and_date = ctime(&now);
      publishSerialData(hour_and_date);
      publishSerialData("home/watertemp/1 is online");
      // ... and resubscribe
      client.subscribe(MQTT_RECEIVER);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();
    payload[length]=0;
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
}

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_PUBLISH, serialData);
}


void readsensors(){
  String dataString;
  t = myDS18B20.Get_temp();
  delay(1000);
      
  //transform the data into a string and send it
  dataString = String(t);
   
  Serial.println(dataString);
  
  //SEND DATA TO MQTT if there are changes to the previous value
  if (t!= t_before ) {
     char copy[3];
     dataString.toCharArray(copy, 32);
     publishSerialData(copy);
  }
  t_before=t;
}
  

void loop() {
   client.loop();
   readsensors();
      
 }
