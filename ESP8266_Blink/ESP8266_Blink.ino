unsigned long count=0;
void setup() {
  Serial.begin(115200);
  pinMode(0, OUTPUT);
}

void loop() {
  digitalWrite(0, HIGH);
  delay(1900);
  Serial.println(count++);
  digitalWrite(0, LOW);
  delay(100);
}
