#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for Serial Monitor to open

  SPI.begin();            // Init SPI bus
  rfid.PCD_Init();        // Init MFRC522
  Serial.println("RFID reader initialized.");

  // Check firmware version to confirm module is connected
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
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Print UID of the tag
  Serial.print("UID tag: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Halt PICC communication and stop encryption
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

