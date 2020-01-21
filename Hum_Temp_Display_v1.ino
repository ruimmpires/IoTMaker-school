#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>
//#include "DHT.h"

// Update these with values suitable for your network.
const char* ssid = "ESCRITORIO";
const char* password = "g0mesp1res";
const char* mqtt_server = "192.168.1.200";
#define mqtt_port 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_PUBLISH "home/humtemp/display"
#define MQTT_RECEIVER "home/humtemp/1/tx"

// ESP8266 compatible pin assignments
int aPin = 16;  // D0        A
int bPin = 5;   // D1     ________
int cPin = 4;   // D2    |        |
int dPin = 0;   // D3  F |        |  B
int ePin = 2;   // D4    |   G    |
int fPin = 14;  // D5    |________|
int gPin = 12;  // D6    |        |
int GND1 = 13;  // D7  E |        |   C  
int GND2 = 15;   // D8   |________|
int GND3 = 9;   // SD2        D      
int GND4 = 10;  // SD3     
//int dPnt = 15;  //
        
int dig1 = 0;
int dig2 = 0;
int dig3 = 0;
int dig4 = 0;

int DTime = 4;

int number =0;

char* hour_and_date;


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
    randomSeed(micros());
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
      publishSerialData("home/humtemp/display is online");
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
    number = atoi((char*)(payload));
    Serial.print("The number is ");
    Serial.println(number);
}

void setup() {
  pinMode(aPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(cPin, OUTPUT);
  pinMode(dPin, OUTPUT);
  pinMode(ePin, OUTPUT); 
  pinMode(fPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(GND1, OUTPUT);
  pinMode(GND2, OUTPUT);
  pinMode(GND3, OUTPUT);
  pinMode(GND4, OUTPUT);
  Serial.begin(115200);
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


void loop() {
   // turn off all display digits
  digitalWrite( GND1, HIGH);
  digitalWrite( GND2, HIGH);
  digitalWrite( GND3, HIGH);
  digitalWrite( GND4, HIGH);
  client.loop();

  dig1 = number / 1000;
  dig2 = (number - (dig1 * 1000)) / 100;
  dig3 = (number - (dig1 * 1000 + dig2 * 100)) / 10;
  dig4 = number % 10;
    
  digitalWrite( GND4, LOW);    //digit 4
  pickNumber(dig4);
  delay(DTime);
  digitalWrite( GND4, HIGH);
 
  digitalWrite( GND3, LOW);    //digit 3
  pickNumber(dig3);
  delay(DTime);
  digitalWrite( GND3, HIGH);
 
  digitalWrite( GND2, LOW);   //digit 2
  pickNumber(dig2);
  delay(DTime);
  digitalWrite( GND2, HIGH);
 
  digitalWrite( GND1, LOW);   //digit 1
  pickNumber(dig1);
  delay(DTime);
  digitalWrite( GND1, HIGH);

}

void pickNumber(int x){
   switch(x){
     case 1: one(); break;
     case 2: two(); break;
     case 3: three(); break;
     case 4: four(); break;
     case 5: five(); break;
     case 6: six(); break;
     case 7: seven(); break;
     case 8: eight(); break;
     case 9: nine(); break;
     default: zero(); break;
   }
}

void clearLEDs()
{  
  digitalWrite(  2, LOW); // A
  digitalWrite(  3, LOW); // B
  digitalWrite(  4, LOW); // C
  digitalWrite(  5, LOW); // D
  digitalWrite(  6, LOW); // E
  digitalWrite(  7, LOW); // F
  digitalWrite(  8, LOW); // G
}

void one()
{
  digitalWrite( aPin, LOW);
  digitalWrite( bPin, HIGH);
  digitalWrite( cPin, HIGH);
  digitalWrite( dPin, LOW);
  digitalWrite( ePin, LOW);
  digitalWrite( fPin, LOW);
  digitalWrite( gPin, LOW);
}

void two()
{
  digitalWrite( aPin, HIGH);
  digitalWrite( bPin, HIGH);
  digitalWrite( cPin, LOW);
  digitalWrite( dPin, HIGH);
  digitalWrite( ePin, HIGH);
  digitalWrite( fPin, LOW);
  digitalWrite( gPin, HIGH);
}

void three()
{
  digitalWrite( aPin, HIGH);
  digitalWrite( bPin, HIGH);
  digitalWrite( cPin, HIGH);
  digitalWrite( dPin, HIGH);
  digitalWrite( ePin, LOW);
  digitalWrite( fPin, LOW);
  digitalWrite( gPin, HIGH);
}

void four()
{
  digitalWrite( aPin, LOW);
  digitalWrite( bPin, HIGH);
  digitalWrite( cPin, HIGH);
  digitalWrite( dPin, LOW);
  digitalWrite( ePin, LOW);
  digitalWrite( fPin, HIGH);
  digitalWrite( gPin, HIGH);
}

void five()
{
  digitalWrite( aPin, HIGH);
  digitalWrite( bPin, LOW);
  digitalWrite( cPin, HIGH);
  digitalWrite( dPin, HIGH);
  digitalWrite( ePin, LOW);
  digitalWrite( fPin, HIGH);
  digitalWrite( gPin, HIGH);
}

void six()
{
  digitalWrite( aPin, HIGH);
  digitalWrite( bPin, LOW);
  digitalWrite( cPin, HIGH);
  digitalWrite( dPin, HIGH);
  digitalWrite( ePin, HIGH);
  digitalWrite( fPin, HIGH);
  digitalWrite( gPin, HIGH);
}

void seven()
{
  digitalWrite( aPin, HIGH);
  digitalWrite( bPin, HIGH);
  digitalWrite( cPin, HIGH);
  digitalWrite( dPin, LOW);
  digitalWrite( ePin, LOW);
  digitalWrite( fPin, LOW);
  digitalWrite( gPin, LOW);
}

void eight()
{
  digitalWrite( aPin, HIGH);
  digitalWrite( bPin, HIGH);
  digitalWrite( cPin, HIGH);
  digitalWrite( dPin, HIGH);
  digitalWrite( ePin, HIGH);
  digitalWrite( fPin, HIGH);
  digitalWrite( gPin, HIGH);
}

void nine()
{
  digitalWrite( aPin, HIGH);
  digitalWrite( bPin, HIGH);
  digitalWrite( cPin, HIGH);
  digitalWrite( dPin, HIGH);
  digitalWrite( ePin, LOW);
  digitalWrite( fPin, HIGH);
  digitalWrite( gPin, HIGH);
}

void zero()
{
  digitalWrite( aPin, HIGH);
  digitalWrite( bPin, HIGH);
  digitalWrite( cPin, HIGH);
  digitalWrite( dPin, HIGH);
  digitalWrite( ePin, HIGH);
  digitalWrite( fPin, HIGH);
  digitalWrite( gPin, LOW);
}
