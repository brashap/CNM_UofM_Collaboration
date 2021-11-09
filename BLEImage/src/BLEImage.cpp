/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#line 1 "c:/Users/IoT_Instructor/Documents/jarvis/BLEImage/src/BLEImage.ino"
/*
 * Project BLEImage
 * Description:
 * Author:
 * Date:
 */


#include "Particle.h"
#include "neopixel.h"

// Setup BLE UART

void setup();
void loop();
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
#line 14 "c:/Users/IoT_Instructor/Documents/jarvis/BLEImage/src/BLEImage.ino"
const size_t UART_TX_BUF_SIZE = 12;
uint8_t txBuf[UART_TX_BUF_SIZE];
uint8_t imgBuf[3];
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

}

//onDataReceived is used to recieve data from Bluefruit Connect App
void onDataReceived(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context) {
    //int pixelcount,pixelcolor;
    uint8_t i,j;
    int color;

    Serial.printf("Received data from: %02X:%02X:%02X:%02X:%02X:%02X \n", peer.address()[0], peer.address()[1],peer.address()[2], peer.address()[3], peer.address()[4], peer.address()[5]);
    for (i = 0; i < len; i++) {
        Serial.printf("%02X\n",data[i]);
    }
    Serial.printf("\n");

    pixel.setBrightness(128);
    pixel.clear();
    for (i=0;i<64;i++) {
      j = 7+(i*3);
      memcpy(imgBuf,&data[j],3);
      Serial.printf("1: 0x%02X, 2: 0x%02X, 3: 0x%02X\n",imgBuf[0],imgBuf[1],imgBuf[2]);
      color = imgBuf[0]<<16 | imgBuf[1]<<8 | imgBuf[2];
      Serial.printf("byte %i, color %06X\n",i,color);
      pixel.setPixelColor(i,color);
    }
    pixel.show();
}