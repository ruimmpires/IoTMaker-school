#include "DHT.h"

#define DHTPIN 4    //Fisicamente é o pino 2 da board // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}


void readsensors(){
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
   
   Serial.print("humidity: ");
   Serial.println(h);
   Serial.print("temperature: ");
   Serial.println(t);

}

void loop() {
   readsensors();
      
 }
