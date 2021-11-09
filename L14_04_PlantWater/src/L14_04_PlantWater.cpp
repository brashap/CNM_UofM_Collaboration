/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/IoTPa/Documents/IoT/instructor_master/L14_Moisture/L14_04_PlantWater/src/L14_04_PlantWater.ino"
/*
 * Project PlantWater2
 * Description: Revised IoT Class House Plant Watering System
 * Author: Brian Rashap
 * Date: 3-DEC-2020
 */

#include "Credentials.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"
#include "Air_Quality_Sensor.h"

// Setup SSD_1306 Display
void setup();
void loop();
void waterPlant(int waterTime);
void printhello();
void oledprint(float Otemp, float Opres, float Ohum, float Odust, int Omoist, int Oaq);
float getDust();
void MQTT_connect();
#line 16 "c:/Users/IoTPa/Documents/IoT/instructor_master/L14_Moisture/L14_04_PlantWater/src/L14_04_PlantWater.ino"
#define SSD1306_128_64
Adafruit_SSD1306 display(-1);

// Declare Air Quality Sensor Object
AirQualitySensor aq(A0);

// THese #include statement is for MQTT to Adafruit.io
#include "Adafruit_MQTT/Adafruit_MQTT.h" 
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h" 
#include "Adafruit_MQTT/Adafruit_MQTT.h" 

/************ Global State (you don't need to change this!) ***   ***************/ 
TCPClient TheClient; 

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details. 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 

/****************************** Feeds ***************************************/ 
Adafruit_MQTT_Publish Htemp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Plant_Temperature");
Adafruit_MQTT_Publish Hmoist = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Plant_Moisture");
Adafruit_MQTT_Publish Hwater = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Plant_Water"); 
Adafruit_MQTT_Publish Hpres = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Plant_Pressure"); 
Adafruit_MQTT_Publish Hhum = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Plant_Humidity"); 
Adafruit_MQTT_Publish Hdust = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Plant_Dust"); 
Adafruit_MQTT_Publish Hqual = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Plant_AQ"); 

Adafruit_MQTT_Subscribe Hpump = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Plant_Pump"); 

// Declare Constants
const byte BME_ADDR = 0x76;
const byte OLED_ADDR = 0x3C;
const byte BLUEPIN = D12;
const byte YELLOWPIN = D11;
const byte RELAYPIN = D10;
const byte MOISTPIN = A4;
const byte AQPIN = A2;
const byte DUSTPIN = A1;
const int threshold = 2400;

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; 

/************Declare Variables*************/
float tempC,prs,hum,conc;
int moist, quality, watered;
int lastPublish, lastSoil;
unsigned int soilDelay = 600000;
unsigned int publishDelay = 120000;
bool status,blueState,yellowState;

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

void setup() {
  pinMode(RELAYPIN,OUTPUT);
  digitalWrite(RELAYPIN,LOW);
  Serial.begin(9600);
  waitFor(Serial.isConnected, 15000);        //give time for Serial Monitor to initalize

  // initialize and clear display
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay();
  display.display();
  printhello();

  //Connect to WiFi without going to Particle Cloud
  WiFi.connect();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.clearDisplay();
  display.printf("  Waiting\n  for Wifi\n");
  display.display();
  while(WiFi.connecting()) {
    Serial.printf(".");
    delay(50);
  }
  Serial.printf("\n\n\n");

  // Initialize Pins
  pinMode(BLUEPIN,INPUT_PULLDOWN);
  pinMode(YELLOWPIN,INPUT_PULLDOWN);
  
  pinMode(MOISTPIN,INPUT); 
  pinMode(DUSTPIN,INPUT);
  pinMode(D7,OUTPUT);

  status = bme.begin(BME_ADDR);
  if (status) {
    Serial.printf("BME280 Online\n");
  }
  else{
    Serial.printf("Error Initializing BME280\n");
  }

  if (aq.init()) {
    Serial.println("AQ Sensor ready.");
  }
  else {
    Serial.println("AQ Sensor ERROR!");
  }

  mqtt.subscribe(&Hpump);

  // Set timers so that moisture checked and data published at startup.
  lastPublish=-999999;
  lastSoil=-999999;
}


void loop() {

  MQTT_connect();

  moist = analogRead(A4);

  //Right now both buttons manually water the plant. They are there for future expandability.
  blueState = digitalRead(BLUEPIN);
  yellowState = digitalRead(YELLOWPIN);

  if(blueState | yellowState){
    waterPlant(750);
    watered = 1;
    oledprint(tempC,prs,hum,conc,moist,quality);
  }

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &Hpump) {
      watered = atoi((char *)Hpump.lastread);
      Serial.printf("Received %i from Adafruit.io feed Plant_Pump \n",watered);
      if(watered){
        waterPlant(1250);
        oledprint(tempC,prs,hum,conc,moist,quality);
      }
    }
  }

  if(millis()-lastSoil > soilDelay){
    if(moist > threshold) {
      watered = 1;
      waterPlant(1000);
      oledprint(tempC,prs,hum,conc,moist,quality);
    }
    else {
      watered = 0;
    }
    lastSoil=millis();
  }

  if(millis()-lastPublish > publishDelay) {
    tempC = bme.readTemperature();
    prs = (bme.readPressure()*0.0002953)+5;
    hum = bme.readHumidity();
    conc = getDust();
    quality = aq.getValue();
    oledprint(tempC,prs,hum,conc,moist,quality);
    if(mqtt.Update()) {
      Serial.printf("Publishing to MQTT\n");
      Htemp.publish(tempC);
      Hmoist.publish(moist);
      Hhum.publish(hum);
      Hpres.publish(prs);
      Hdust.publish(conc);
      Hqual.publish(quality);
      Hwater.publish(watered);
    }
    else {
      Serial.printf("Publish failed\n");
    } 
    lastPublish = millis();
    watered = 0;
  }
}

void waterPlant(int waterTime) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.clearDisplay();
  display.printf(".Watering.");
  display.display();
  delay(500);
  digitalWrite(RELAYPIN,HIGH);
  delay(waterTime);
  digitalWrite(RELAYPIN,LOW);
}

void printhello() {
  display.clearDisplay();
  // display a pixel in each corner of the screen
  display.drawPixel(0, 0, WHITE);
  display.drawPixel(127, 0, WHITE);
  display.drawPixel(0, 63, WHITE);
  display.drawPixel(127, 63, WHITE);
  // display a line of text
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.print("   Hello\n   World!");
  display.display();
  delay(2000);
}

void oledprint(float Otemp, float Opres, float Ohum, float Odust, int Omoist, int Oaq) {

   // initialize and clear display
  display.clearDisplay();
  display.display();

  // display a pixel in each corner of the screen
  display.drawPixel(0, 0, WHITE);
  display.drawPixel(127, 0, WHITE);
  display.drawPixel(0, 63, WHITE);
  display.drawPixel(127, 63, WHITE);

  // display a line of text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,5);
  display.printf("Environmental Reading");
  display.setCursor(0,18);
  display.printf("Temperature(F) %0.2f\n",Otemp);
  display.printf("Pressure(inHg) %0.2f\n",Opres);
  display.printf("Humidity (%%rH)  %0.2f\n",Ohum);
  display.printf("Moisture:      %04i \n",Omoist);
  display.printf("Dust: %05.0f  AQ: %i \n",Odust,Oaq);

  display.display();
}

float getDust() {
  unsigned long duration;
  unsigned long starttime;
  unsigned long sampletime_ms = 30000;//sampe 30s ;
  unsigned long lowpulseoccupancy = 0;
  float ratio = 0;
  float concentration = -1;

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
  display.clearDisplay();
  display.printf(".Getting.\nDustData\n");
  display.display();

  starttime = millis();//get the current time;

  while(concentration == -1) {
    duration = pulseIn(DUSTPIN, LOW);
    lowpulseoccupancy = lowpulseoccupancy+duration;

    if ((millis()-starttime) > sampletime_ms)//if the sampel time == 30s
    {
        ratio = lowpulseoccupancy/(sampletime_ms*10.0);  // Integer percentage 0=>100
        concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; // using spec sheet curve
        Serial.print(lowpulseoccupancy);
        Serial.print(",");
        Serial.print(ratio);
        Serial.print(",");
        Serial.println(concentration);
        lowpulseoccupancy = 0;
        starttime = millis();
    }
  }
return concentration;
}



// Function to connect and reconnect as necessary to the MQTT server.
void MQTT_connect() {
  int8_t ret;
 
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("%s\n",(char *)mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds..\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.printf("MQTT Connected!\n");
}