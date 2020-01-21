#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include "DHT.h"

// Update these with values suitable for your network.
const char* ssid = "ESCRITORIO";
const char* password = "*******";
const char* mqtt_server = "192.168.1.200";
#define mqtt_port 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_PUBLISH "home/humtemp/2/tx"
#define MQTT_RECEIVER "home/humtemp/2/rx"

char* hour_and_date;
#define DHTPIN 4    //Fisicamente é o pino 2 da board // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
int h_before = 0;
int t_before = 0;

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
      publishSerialData("home/humtemp/2 is online");
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
    //Serial.print("payload length"); 
    //Serial.println(length); 
    payload[length]=0;
    //received = atoi((char*)(payload));    
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.setTimeout(500);// Set time out for 
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
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  while (isnan(h) || isnan(t)){
    h = dht.readHumidity();
    t = dht.readTemperature();
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(1000);
  }
  delay(1000);

  
  
  //time_t now = time(nullptr);
  //hour_and_date = ctime(&now);
      
  //transform the data into a string and send it
  //when the temperature is between 0 an 9, I want a trailing 0
  //dataString = String(hour_and_date)+","+h+","+t;
  //dataString = String(h)+","+t;
  if ((t < 10)&&(t>=0)) dataString = String(h)+"0"+t;
  else dataString = String(h)+t;
 
  
  Serial.println(dataString);
  
  //SEND DATA TO MQTT if there are changes to the previous value
  if (h != h_before | t!= t_before ) {
     char copy[3];
     dataString.toCharArray(copy, 32);
     publishSerialData(copy);
  }
  t_before=t;
  h_before=h;
}
  

void loop() {
   client.loop();
   readsensors();
      
 }
