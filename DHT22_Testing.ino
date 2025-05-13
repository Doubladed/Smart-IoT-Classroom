#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22
#define FANPIN 3
#define TEMP_THRESHOLD 30.0 // Temporary just for testing 

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(FANPIN, OUTPUT);
  digitalWrite(FANPIN, LOW); 
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    digitalWrite(FANPIN, LOW);  
    return;
  }

  Serial.print("Humidity: "); Serial.print(h); Serial.print(" %\t");
  Serial.print("Temperature: "); Serial.print(t); Serial.println(" °C");

  if (t >= TEMP_THRESHOLD) {
    digitalWrite(FANPIN, HIGH);
    Serial.println("Fan ON");
  } else {
    digitalWrite(FANPIN, LOW);
    Serial.println("Fan OFF");
  }
}
