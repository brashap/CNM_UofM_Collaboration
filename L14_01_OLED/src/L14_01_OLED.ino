/*
 * Project L14_01_OLED
 * Description: Learn to use OLED display with Particle Argon
 * Author: Brian Rashap
 * Date: 04-AUG-2021
 */

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define OLED_RESET D4
Adafruit_SSD1306 display(OLED_RESET);

const unsigned int updateTime = 5000;
unsigned int lastTime;

SYSTEM_MODE(SEMI_AUTOMATIC);

void setup() {

  Serial.begin(9600);
  Time.zone(-6); // -6 for MDT, -7 for MST
  Particle.syncTime();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  //0x3C obtained from I2C_Scan  
  display.display(); 
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer  
}

void loop() {
  if(millis()-lastTime>updateTime) {
    displayTime();
    lastTime = millis();
  }
}

void displayTime() {
  String DateTime, TimeOnly;
  
  display.clearDisplay();   // clears the screen and buffer
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.printf("My name is Brian\n");

  DateTime = Time.timeStr();
  TimeOnly = DateTime.substring(11,19);

  display.setTextColor(BLACK, WHITE); 
  Serial.printf("UNIX Date is %s\n",DateTime.c_str());
  display.printf("Time is %s\n",TimeOnly.c_str());
  display.display();
}