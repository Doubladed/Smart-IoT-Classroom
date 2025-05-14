#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);


struct User {
  byte uid[4];
  const char* name;
  const char* id;
  bool isInside; // false = outside, true = inside
};


User users[] = {
  {{0x93, 0xEA, 0x90, 0x28}, "Sakkaf", "1201102450", false},
  {{0xC3, 0x0A, 0xD4, 0x2C}, "Ghaleb", "1201102570", false}
};

const int userCount = sizeof(users) / sizeof(users[0]);

void setup() {
  Serial.begin(9600);
  while (!Serial); 

  SPI.begin();
  rfid.PCD_Init();
  delay(4); 

  Serial.println("RFID reader initialized.");
  Serial.println("Scan an RFID tag...");
}

void loop() {

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }


  Serial.print("UID tag: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  
  bool found = false;
  for (int i = 0; i < userCount; i++) {
    bool match = true;
    for (byte j = 0; j < 4; j++) {
      if (rfid.uid.uidByte[j] != users[i].uid[j]) {
        match = false;
        break;
      }
    }

    if (match) {
      if (!users[i].isInside) {
        Serial.print(users[i].name);
        Serial.print(" (");
        Serial.print(users[i].id);
        Serial.println(") has ENTERED.");
        users[i].isInside = true;
      } else {
        Serial.print(users[i].name);
        Serial.print(" (");
        Serial.print(users[i].id);
        Serial.println(") has LEFT.");
        users[i].isInside = false;
      }
      found = true;
      break;
    }
  }

  if (!found) {
    Serial.println("Unknown tag. Access denied.");
  }

  
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
