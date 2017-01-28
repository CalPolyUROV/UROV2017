void setup() {
  Serial.begin(9600);
  for (int i = 0; i <= 360; i ++)
  {
    Serial.print(", ");
    Serial.print(cos(i * PI / 180));
  }
  Serial.print("\n");
}

void loop() {
  
  
}
