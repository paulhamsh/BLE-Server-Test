#include <M5Core2.h> 
//#include "heltec.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define BLE_SERVICE "FCC0"
#define BLE_CHAR_RECEIVE "FCC1"
#define BLE_CHAR_SEND "FCC2"

static BLEServer* pServer;
BLECharacteristic *pCharacteristic_send;
BLECharacteristic *pCharacteristic_receive;
BLEService *pService;

class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        Serial.println("Client connected");
        Serial.println("Multi-connect support: start advertising");
        BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Client disconnected - start advertising");
        BLEDevice::startAdvertising();
    };
};


class CharacteristicCallbacks: public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic* pCharacteristic){
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onRead(), value: ");

        int j, l;
        const char *p;
        l = pCharacteristic->getValue().length();
        p = pCharacteristic->getValue().c_str();
        for (j=0; j < l; j++) {
          Serial.print(" ");
          Serial.print(p[j], HEX); 
        }
        Serial.println();
    };

    void onWrite(BLECharacteristic* pCharacteristic) {
        Serial.print(pCharacteristic->getUUID().toString().c_str());
        Serial.print(": onWrite(), value: ");
      
        int j, l;
        const char *p;
        l = pCharacteristic->getValue().length();
        p = pCharacteristic->getValue().c_str();
        for (j=0; j < l; j++) {
          Serial.print(" ");
          Serial.print(p[j], HEX); 
        }
        Serial.println();
    };
    
    void onNotify(BLECharacteristic* pCharacteristic) {
        Serial.print("Sending notification to clients - value: ");

        int j, l;
        const char *p;
        l = pCharacteristic->getValue().length();
        p = pCharacteristic->getValue().c_str();
        for (j=0; j < l; j++) {
          Serial.print(" ");
          Serial.print(p[j], HEX); 
        }
        Serial.println();
    };

    void onStatus(BLECharacteristic* pCharacteristic, Status status, int code) {
        String str = ("Notification/Indication status code: ");
        str += status;
        str += ", return code: ";
        str += code;
        Serial.println(str);
    };

};
    
/** Handler class for descriptor actions */  
/*  
class DescriptorCallbacks : public BLEDescriptorCallbacks {
    void onWrite(BLEDescriptor* pDescriptor) {
        std::string dscVal((char*)pDescriptor->getValue(), pDescriptor->getLength());
        Serial.print("Descriptor witten value:");
        Serial.println(dscVal.c_str());
    };

    void onRead(BLEDescriptor* pDescriptor) {
        Serial.print(pDescriptor->getUUID().toString().c_str());
        Serial.println(" Descriptor read");
    };
};


static DescriptorCallbacks dscCallbacks;
*/
static CharacteristicCallbacks chrCallbacks_s, chrCallbacks_r;

int a_val;

void setup() {

  M5.begin();
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextSize(4);
  M5.Lcd.println("BLE BT Test");
  Serial.println("Started");

/*
  Heltec.begin(true , false , true );
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "BLE Server Test");
  Heltec.display->display();
*/

/*  
  Serial.begin(115200);
*/  
  BLEDevice::init("BLE Server Test");
      
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
     
  pService = pServer->createService(BLE_SERVICE);
  pCharacteristic_receive = pService->createCharacteristic(BLE_CHAR_RECEIVE, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  pCharacteristic_send = pService->createCharacteristic(BLE_CHAR_SEND, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic_receive->setValue("TestRead");
  pCharacteristic_receive->setCallbacks(&chrCallbacks_r);
  pCharacteristic_send->setValue("TestWrite");
  pCharacteristic_send->setCallbacks(&chrCallbacks_s);
  pCharacteristic_send->addDescriptor(new BLE2902());
        
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
 
  pAdvertising->addServiceUUID(pService->getUUID()); 

  //pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  //pAdvertising->setMinPreferred(0x12);
  pAdvertising->setMinPreferred(0x20);    
  
  pAdvertising->setScanResponse(true);
  pAdvertising->start(); 

  a_val = 0;
}


void loop() {
  M5.update();
  delay(4000);
  a_val++;
  pCharacteristic_send->setValue(a_val);
  pCharacteristic_send->notify(true);
}
