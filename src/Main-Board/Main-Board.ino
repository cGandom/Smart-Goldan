/*
 * Smart Goldan
 * Main Board Program
 * Sajjad Gandom Malmiri, Alireza Tavakoli, Erfan Razzaqi, Kaveh Masoumi
 */

#include <LiquidCrystal.h>

#define START_COMM_DELIM '!'

const int lcdRs = 12, lcdEn = 11, lcdD4 = 5, lcdD5 = 4, lcdD6 = 3, lcdD7 = 2; 
LiquidCrystal lcd(lcdRs, lcdEn, lcdD4, lcdD5, lcdD6, lcdD7);

void setup() {
  // Initialze LCD With 20x4 Size
  lcd.begin(20, 4);

  // Initialize Bluetooth Communication via Serial
  Serial.begin(9600);
  while (!Serial);

  displayStatus(0, 0, "Booting Up");
}

void loop() {
  if (Serial.available() > 0) {
    char startComm = (char) Serial.read();
    if (startComm == START_COMM_DELIM) {  
      float humidity = readFloat();
      float temperature = readFloat();
      updateDisplayTH(humidity, temperature);
      updateDisplayAction("Deciding...");

      byte motorSpeed = decideMotorSpeed(humidity, temperature);
      Serial.print(START_COMM_DELIM);
      Serial.write(motorSpeed);
      
    }
    
  }
  delay(100);
}

byte decideMotorSpeed(float humidity, float temperature) {
  byte motorSpeed;
  if (humidity > 50.0f) {
    updateDisplayAction("No Watering");
    motorSpeed = 0x00;
  }
  else if (humidity > 20.0f) {
    if (temperature < 25.0f) {
      updateDisplayAction("No Watering");
      motorSpeed = 0x00;
    }
    else {
      updateDisplayAction("Watering 10cc/m");
      motorSpeed = 0x19;
    }
  }
  else {
      updateDisplayAction("Watering 20cc/m");
      motorSpeed = 0x40;
  }
  return motorSpeed;
}

float readFloat() {
  byte val[4];
  Serial.readBytes(val, 4);
  float* floatVal = (float*) val;
  return *floatVal;
}

void updateDisplayTH(float humidity, float temperature) {
  lcd.setCursor(11, 0);
  lcd.print(humidity);

  lcd.setCursor(11, 1);
  lcd.print(temperature);
}

void updateDisplayAction(String action) {
  lcd.setCursor(0, 3);
  int spaces = (20 - action.length())/2;
  int i;
  for (i = 0; i < spaces; i++)
    lcd.print(' ');
  lcd.print(action);
  for (i = 0; i < spaces; i++)
    lcd.print(' ');
}

void displayStatus(float humidity, float temperature, String action) {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Humidity:  ");
  lcd.print(humidity);
  lcd.print("  %RH");

  lcd.setCursor(0, 1);
  lcd.print("Temp    :  ");
  lcd.print(temperature);
  lcd.print("  C");

  lcd.setCursor(0, 2);
  lcd.print("======|Action|======");
  lcd.setCursor((20 - action.length())/2, 3);
  lcd.print(action);
}
