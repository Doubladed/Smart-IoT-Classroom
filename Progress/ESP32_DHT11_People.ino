#include "DHT.h"
#define DHTPIN 15     
#define DHTTYPE DHT11 
#define LED 12
  float h;
  float t;
  int people = 1;

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  pinMode(LED, OUTPUT);

  Serial.begin(9600);
  Serial.println(F("DHT11 test!"));

  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(1000);

if(people > 0)
{
  h = dht.readHumidity();
  t = dht.readTemperature();
}
else
{
  h = 0;
  t = 0;
}
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("\n"));

if(t>20.0)
{
  digitalWrite(LED, HIGH);
}
else
{
 digitalWrite(LED, LOW);
}
 
}
