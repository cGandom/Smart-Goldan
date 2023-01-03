/*
 * Smart Goldan
 * TH Board Program
 * Sajjad Gandom Malmiri, Alireza Tavakoli, Erfan Razzaqi, Kaveh Masoumi
 */

#include <Wire.h>

#define START_COMM_DELIM '!'

#define TH_SENSOR_ADDRESS 0x40

const int motorDriverPinInput1  = 12;
const int motorDriverPinInput2  = 11;
const int motorDriverPinEn1     = 10;

unsigned long previousMillis = 0;
const long samplingIntervalMillis = 5000;

byte motorSpeed;

void setup() {
  // Initialize DC Motor Driver
  pinMode(motorDriverPinInput1, OUTPUT);
  pinMode(motorDriverPinInput2, OUTPUT);
  pinMode(motorDriverPinEn1, OUTPUT);
  digitalWrite(motorDriverPinInput1, HIGH);
  digitalWrite(motorDriverPinInput2, LOW);

  // Initialize Temperature/Humidity Sensor I2C Communication
  Wire.begin();

  // Initialize Bluetooth Serial Communication
  Serial.begin(9600);
  while (!Serial);
  
}

void loop() {
  if (Serial.available() > 0) {
    char startComm = Serial.read();
    if (startComm == START_COMM_DELIM) {
      motorSpeed = (byte) Serial.read();
      analogWrite(motorDriverPinEn1, motorSpeed);
    }
  }
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= samplingIntervalMillis) {
    previousMillis = currentMillis;

    float humidity = getHumidity();
    float temperature = getTemperature();

    Serial.print(START_COMM_DELIM);
    sendFloat(humidity);
    sendFloat(temperature);
  }

  delay(100);
}

void sendFloat(float num) {
  byte* f_bytes = (byte*) &num;
  for (int idx = 0; idx < sizeof(float); Serial.write(f_bytes[idx++]) );
}

float getHumidity() {
  unsigned int data[2];
  Wire.beginTransmission(TH_SENSOR_ADDRESS);
  Wire.write(0xF5); // Send humidity measurement command
  Wire.endTransmission();
  delay(200);

  Wire.requestFrom(TH_SENSOR_ADDRESS, 2); // Read 2 bytes of data

  while (Wire.available() < 2)
    delay(200);

  data[0] = Wire.read();
  data[1] = Wire.read();

  float humidity = (((data[0] * 256.0 + data[1]) * 125.0) / 65536.0) - 6;

  return humidity;
}

float getTemperature() {
  unsigned int data[2];
  Wire.beginTransmission(TH_SENSOR_ADDRESS);
  Wire.write(0xF3); // Send temperature measurement command
  Wire.endTransmission();
  delay(200);

  Wire.requestFrom(TH_SENSOR_ADDRESS, 2); // Read 2 bytes of data

  while (Wire.available() < 2)
    delay(200);
    
  data[0] = Wire.read();
  data[1] = Wire.read();

  float tempCentigrade = (((data[0] * 256.0 + data[1]) * 175.72) / 65536.0) - 46.85;

  return tempCentigrade;
}
