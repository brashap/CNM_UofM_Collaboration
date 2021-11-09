/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "c:/Users/IoT_Instructor/Documents/jarvis/BLESend/src/BLESend.ino"
/*
 * Project BLESend
 * Description: Example of sending UART data over BLE
 * Author: Brian Rashap
 * Date: 2-MAR-2021
 */

#include "Particle.h"
#include "neopixel.h"
#include "colors.h"

// Setup BLE UART

void setup();
void loop();
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
#line 14 "c:/Users/IoT_Instructor/Documents/jarvis/BLESend/src/BLESend.ino"
const size_t UART_TX_BUF_SIZE = 12;
uint8_t txBuf[UART_TX_BUF_SIZE];
uint8_t i;

int red, green, blue;

// These UUIDs were defined by Nordic Semiconductor and are now the defacto standard for
// UART-like services over BLE. Many apps support the UUIDs now, like the Adafruit Bluefruit app.
const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");

BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);
BleAdvertisingData data;

// Setup NeoPixels
#define PIXEL_COUNT 100
#define PIXEL_PIN D2
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel pixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

SYSTEM_MODE(SEMI_AUTOMATIC); //Using BLE and not Wifi

void setup() { 
    Serial.begin();
    waitFor(Serial.isConnected, 15000);  
    delay(2000);
 
    BLE.on();

    Serial.printf("Starting up BLE Connection \n");

    BLE.addCharacteristic(txCharacteristic);
    BLE.addCharacteristic(rxCharacteristic);
    data.appendServiceUUID(serviceUuid);
    BLE.advertise(&data);

    Serial.printf("Argon BLE Address: %s\n",BLE.address().toString().c_str());
}

void loop() {
    //for(i=0;i<UART_TX_BUF_SIZE-1;i++) {
    //    txBuf[i] = random(0x40,0x5B); //Captial ASCII characters plus @
    //}
    red = random(256);
    green = random(256);
    blue = random(256);

    txBuf[0] = 0x30 + red / 100;
    txBuf[1] = 0x30 + (red % 100) / 10;
    txBuf[2] = 0x30 + red % 10;
    txBuf[3] = 0x2C;
    txBuf[4] = 0x30 + green / 100;
    txBuf[5] = 0x30 + (green % 100)/ 10;
    txBuf[6] = 0x30 + green % 10;
    txBuf[7] = 0x2C;
    txBuf[8] = 0x30 + blue / 100;
    txBuf[9] = 0x30 + (blue % 100)/ 10;
    txBuf[10] = 0x30 + blue % 10;
    txBuf[11] = 0x0A;

    //txBuf[UART_TX_BUF_SIZE-1]=0x0A;  //Append a carriage return to txBuf
    txCharacteristic.setValue(txBuf, UART_TX_BUF_SIZE);
    for(i=0;i<UART_TX_BUF_SIZE;i++) {
        Serial.printf("%c",txBuf[i]);
    }
    Serial.printf("\n");
    delay(5000);
}

//onDataReceived is used to recieve data from Bluefruit Connect App
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
    int pixelcount,pixelcolor;
    uint8_t i;

    Serial.printf("Received data from: %02X:%02X:%02X:%02X:%02X:%02X \n", peer.address()[0], peer.address()[1],peer.address()[2], peer.address()[3], peer.address()[4], peer.address()[5]);
    for (i = 0; i < len; i++) {
        Serial.printf("%X",data[i]);
    }
    Serial.printf("\n");
    pixelcount = atoi((char *)data);
    Serial.printf("The data from Adafruit = %i",pixelcount);
    pixel.clear();
    pixelcolor=colorwheel[random(0,13)];
    pixel.setBrightness(50);
    for(i = 0; i <= pixelcount; i++) {
        pixel.setPixelColor(i,pixelcolor);
    }
    pixel.show();
}