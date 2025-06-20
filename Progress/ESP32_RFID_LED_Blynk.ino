/*********  Wi-Fi + Blynk credentials  *********/          // *** BLYNK ADD
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN    ""                // *** BLYNK ADD

const char* ssid     = "";                     // *** BLYNK ADD
const char* password = "";                 // *** BLYNK ADD

/*********  Libraries  *********/
#include <WiFi.h>                                          // *** BLYNK ADD
#include <BlynkSimpleEsp32.h>                              // *** BLYNK ADD
#include <SPI.h>
#include <MFRC522.h>

/*********  Hardware pins  *********/
#define SS_PIN   21
#define RST_PIN  22
#define RFID_LED  12

#define Virt_RFID V0
#define Virt_Lights V1
#define Virt_NumPeople V2

MFRC522 rfid(SS_PIN, RST_PIN);

/*********  User database  *********/
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

void setup() {
  Serial.begin(9600);

  /**** Wi-Fi + Blynk ****/                                 // *** BLYNK ADD
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);           // *** BLYNK ADD

  /**** RFID ****/
  SPI.begin();
  rfid.PCD_Init();
  delay(4);
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);

  pinMode(RFID_LED, OUTPUT);

}

void loop() {
  Blynk.run();                                             // *** BLYNK ADD

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  char msg[64];                                            // *** BLYNK ADD
  bool found = false;

  for (int i = 0; i < userCount; i++) {
    if (memcmp(rfid.uid.uidByte, users[i].uid, 4) == 0) {

      if (!users[i].isInside) {
        snprintf(msg, sizeof(msg), "%s (%s) has ENTERED.", users[i].name, users[i].id);   // *** BLYNK ADD (msg)
        numPeople++;
        users[i].isInside = true;
      } else {
        snprintf(msg, sizeof(msg), "%s (%s) has LEFT.", users[i].name, users[i].id);      // *** BLYNK ADD (msg)
        numPeople--;
        users[i].isInside = false;
      }
      Serial.println(msg);
      Blynk.virtualWrite(Virt_RFID, msg);                        // *** BLYNK ADD
      found = true;
      break;
    }
  }

  if (!found) {
    strcpy(msg, "Unknown tag. Access denied.");            // *** BLYNK ADD
    Serial.println(msg);
    Blynk.virtualWrite(Virt_RFID, msg);                           // *** BLYNK ADD
  }

  // Check if anyone is inside and update LEDs (physical + app)
  bool anyoneInside = false;
  for (int i = 0; i < userCount; i++) {
    if (users[i].isInside) {
      anyoneInside = true;
      break;
    }
  }
  digitalWrite(RFID_LED, anyoneInside ? HIGH : LOW);
  Blynk.virtualWrite(Virt_Lights, anyoneInside ? 255 : 0);          // *** BLYNK ADD
  Blynk.virtualWrite(Virt_NumPeople, numPeople); 

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
