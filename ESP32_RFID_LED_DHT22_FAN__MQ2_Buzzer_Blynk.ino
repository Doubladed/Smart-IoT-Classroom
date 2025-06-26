//BLYNK 
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN    ""   
//WIFI
const char* ssid     = "";                     
const char* password = "";                 
//LIBRARIES
#include <WiFi.h>                                          
#include <BlynkSimpleEsp32.h>                              
#include <SPI.h>
#include <MFRC522.h>
#include "DHT.h"
//PINS
#define DHTPIN 15     
#define DHTTYPE DHT22
#define SS_PIN   21
#define RST_PIN  22
#define RFID_LED  12
#define DHT_FAN 25
#define MQ2_A0_PIN 32
#define BUZZER_PIN 27 

#define Virt_RFID V0
#define Virt_Lights V1
#define Virt_NumPeople V2
#define Virt_DHTstatus V3
#define Virt_Temperature V4
#define Virt_Humidity V5
#define Virt_Fan V6
#define Virt_AirQuality V8
#define Virt_Buzzer V7

DHT dht(DHTPIN, DHTTYPE);
MFRC522 rfid(SS_PIN, RST_PIN);

//USER DATABASE
struct User {
  byte uid[4];
  const char* name;
  const char* id;
  bool  isInside;
};

User users[] = {
  {{0x93, 0xEA, 0x90, 0x28}, "Sakkaf", "1201102450", false},
  {{0xC3, 0x0A, 0xD4, 0x2C}, "Ghaleb", "1201102570", false}
};
const int userCount = sizeof(users) / sizeof(users[0]);
int numPeople = 0;
float h, t;

void setup() {
  Serial.begin(9600);
                        
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);          

  SPI.begin();
  rfid.PCD_Init();
  delay(4);
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);

  pinMode(RFID_LED, OUTPUT);
  pinMode(DHT_FAN, OUTPUT);
  dht.begin();

  pinMode(MQ2_A0_PIN , INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); 

}

void loop() {
  Blynk.run();    
  delay(1000);
  int analogValue = analogRead(MQ2_A0_PIN);                                         

  Serial.print("Analog Value: ");
  Serial.print(analogValue);
  Blynk.virtualWrite(Virt_AirQuality, analogValue);

  if (analogValue > 1000) {
    tone(BUZZER_PIN, 1000); 
    Serial.println(" - Smoke Detected!");
    Blynk.virtualWrite(Virt_Buzzer, 1);
  } else {
    noTone(BUZZER_PIN);   
    Serial.println(" - Clean Air");
    Blynk.virtualWrite(Virt_Buzzer, 0);
  }

if(numPeople > 0)
{
  Blynk.virtualWrite(Virt_DHTstatus, 1);
  h = dht.readHumidity();
  t = dht.readTemperature();
}
else
{
  Blynk.virtualWrite(Virt_DHTstatus, 0);
  h = 0;
  t = 0;
}

Blynk.virtualWrite(Virt_Temperature, t);
Blynk.virtualWrite(Virt_Humidity, h);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("\n"));

if(t>30.0)
{
  Blynk.virtualWrite(Virt_Fan, 1);
  digitalWrite(DHT_FAN, HIGH);
}
else
{
  Blynk.virtualWrite(Virt_Fan, 0);
  digitalWrite(DHT_FAN, LOW);
}

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  char msg[64];                                           
  bool found = false;

  for (int i = 0; i < userCount; i++) {
    if (memcmp(rfid.uid.uidByte, users[i].uid, 4) == 0) {

      if (!users[i].isInside) {
        snprintf(msg, sizeof(msg), "%s (%s) has ENTERED.", users[i].name, users[i].id);   
        numPeople++;
        users[i].isInside = true;
      } else {
        snprintf(msg, sizeof(msg), "%s (%s) has LEFT.", users[i].name, users[i].id);      
        numPeople--;
        users[i].isInside = false;
      }
      Serial.println(msg);
      Blynk.virtualWrite(Virt_RFID, msg);                        
      found = true;
      break;
    }
  }

  if (!found) {
    strcpy(msg, "Unknown tag. Access denied.");            
    Serial.println(msg);
    Blynk.virtualWrite(Virt_RFID, msg);                           
  }

  bool anyoneInside = false;
  for (int i = 0; i < userCount; i++) {
    if (users[i].isInside) {
      anyoneInside = true;
      break;
    }
  }
  digitalWrite(RFID_LED, anyoneInside ? HIGH : LOW);
  Blynk.virtualWrite(Virt_Lights, anyoneInside ? 255 : 0);          
  Blynk.virtualWrite(Virt_NumPeople, numPeople); 

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
