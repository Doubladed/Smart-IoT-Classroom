#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED_PIN 8  // LED to indicate at least one person inside

MFRC522 rfid(SS_PIN, RST_PIN);

// Structure to hold UID + user data and state
struct User {
  byte uid[4];
  const char* name;
  const char* id;
  bool isInside; // false = outside, true = inside
};

// Predefined UID database
User users[] = {
  {{0x93, 0xEA, 0x90, 0x28}, "Sakkaf", "1201102450", false},
  {{0xC3, 0x0A, 0xD4, 0x2C}, "Ghaleb", "1201102570", false}
};

const int userCount = sizeof(users) / sizeof(users[0]);

void setup() {
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();
  delay(4); // Some boards need time after init

  // Set antenna gain to maximum
  rfid.PCD_SetAntennaGain(rfid.RxGain_max);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // LED off initially

  Serial.println("RFID reader initialized.");
 byte version = rfid.PCD_ReadRegister(rfid.VersionReg);
  Serial.print("Firmware Version: ");
  Serial.println(version, HEX);

  if (version == 0x00 || version == 0xFF) {
    Serial.println("ERROR: RFID reader not connected or not functioning.");
  } else {
    Serial.println("Scan an RFID tag...");
  }
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Show the UID
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

  // Update LED state
  bool anyoneInside = false;
  for (int i = 0; i < userCount; i++) {
    if (users[i].isInside) {
      anyoneInside = true;
      break;
    }
  }
  digitalWrite(LED_PIN, anyoneInside ? HIGH : LOW);

  // Halt card communication
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
