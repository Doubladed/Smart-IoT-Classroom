#define MQ2_D0_PIN 2     
#define MQ2_A0_PIN A0    
#define BUZZER_PIN 8     

void setup() {
  Serial.begin(9600);
  pinMode(MQ2_D0_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); 
}

void loop() {
  int analogValue = analogRead(MQ2_A0_PIN);
  int digitalValue = digitalRead(MQ2_D0_PIN);

  Serial.print("Analog Value: ");
  Serial.print(analogValue);
  Serial.print(" | Digital Alarm: ");
  Serial.println(digitalValue == LOW ? "⚠️ GAS!" : "✅ Clean");

  if (digitalValue == LOW) {
    digitalWrite(BUZZER_PIN, HIGH); 
  } else {
    digitalWrite(BUZZER_PIN, LOW);  
  }

  delay(1000);
}
