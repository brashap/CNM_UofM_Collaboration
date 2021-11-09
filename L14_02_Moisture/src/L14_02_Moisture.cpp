/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/IoT_Instructor/Documents/IoT/instructor_master/L14_Moisture/L14_02_Moisture/src/L14_02_Moisture.ino"
/*
 * Project L14_02_Moisture
 * Description: Calibrate Soil Moisture Probe
 * Author: Brian Rashap
 * Date: 04-AUG-2021
 */

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

void setup();
void loop();
void displayMoisture(int moist);
#line 11 "c:/Users/IoT_Instructor/Documents/IoT/instructor_master/L14_Moisture/L14_02_Moisture/src/L14_02_Moisture.ino"
#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

const int MOISTPIN = A0;
const unsigned int updateTime = 5000;
unsigned int lastTime;
int moisture;

void setup() {

  Time.zone(-6); // -6 for MDT, -7 for MST
  Particle.syncTime();

  pinMode(MOISTPIN,INPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //0x3C obtained from I2C_Scan  
  display.display(); 
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer  
}

void loop() {
  if(millis()-lastTime>updateTime) {
    moisture = analogRead(MOISTPIN);
    displayMoisture(moisture);
    lastTime = millis();
  }
}

void displayMoisture(int moist) {
  String DateTime, TimeOnly;

  DateTime = Time.timeStr();
  TimeOnly = DateTime.substring(11,19);

  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("Time is %s\n",TimeOnly.c_str());
  display.printf("Moisure Level is %i", moist);
  display.display();
}