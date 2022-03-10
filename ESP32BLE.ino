#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID "3feb1e8a-3981-4045-ad39-b225135013a0"
#define CONTROL_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
char ledStatus;
int llegadaARD = 22;
//int envioARD = 23;
int clave = 5555;
int claveCel;
int pos1 =0;
int pos2 =0;
int pos3 =0;
int pos4 =0;
char ledStatus1 ;
char ledStatus2 ;
char ledStatus3 ;
const int ledPin = 23;
// New characteristic with object to manage it
BLECharacteristic controlCharacteristic(
  CONTROL_CHARACTERISTIC_UUID,
  BLECharacteristic::PROPERTY_READ |
  BLECharacteristic::PROPERTY_WRITE
);
void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(115200);
  Serial.println("Starting BLE!");
  Serial.println("Initializing device");
  BLEDevice::init("grupo3"); // Initializing the device with its name
  Serial.println("Creating server");
  BLEServer *pServer = BLEDevice::createServer(); // Create the server
  Serial.println("Adding service UUID");
  BLEService *pService = pServer->createService(SERVICE_UUID); // Creating a new service into server
  // Adding a characteristic with the object name (official UUID), without object (this characteristic will not change)
  Serial.println("Adding name characteristic");
  BLECharacteristic *nameCharacteristic = pService->createCharacteristic(
      BLEUUID((uint16_t)0x2A00),
      BLECharacteristic::PROPERTY_READ
                                          );
  nameCharacteristic->setValue("Led");
  // Adding a characteristic to control the led with 0 and 1
  Serial.println("Adding control characteristic");
  pService->addCharacteristic(&controlCharacteristic);
  controlCharacteristic.setValue(&ledStatus); // Value uint8_t with length 1

  Serial.println("Starting...");
  pService->start();
  Serial.println("Creating advertising");
  // BLEAdvertising *pAdvertising = pServer->getAdvertising(); // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
  pinMode (ledPin, OUTPUT); // Set the LED pin as OUTPUT
}
void loop() {
  std::string controlValue = controlCharacteristic.getValue();

  //int pos4=0;
  // if(pos4 == 0){

  // }
  pos1=0;
  pos2=0;
  pos3=0;
  pos4=0;
  if ((controlValue[0] != ledStatus)||(controlValue[1] != ledStatus1)||(controlValue[2] != ledStatus2)||(controlValue[3] != ledStatus3)) {


    Serial.print("Value changed... new value: ");

    ledStatus = controlValue[0];
    ledStatus1 = controlValue[1];
    ledStatus2 = controlValue[2];
    ledStatus3 = controlValue[3];
    pos1 = controlValue[0];
    pos1 -= 48;
    pos1 = pos1 * 1000;
    Serial.println(pos1);
    pos2 = controlValue[1];
    pos2 -= 48;
    pos2 = pos2 * 100;
    Serial.println(pos2);
    pos3 = controlValue[2];
    pos3 -= 48;
    pos3 = pos3 * 10;
    Serial.println(pos3);
    pos4 = controlValue[3];
    pos4 -= 48;
    pos4 = pos4 * 1;
    Serial.println(pos4);
    claveCel = pos1 + pos2 + pos3 + pos4;
    Serial.println(claveCel);
    if (claveCel != clave) {
      digitalWrite(ledPin, LOW); // LED Off
    }
    if (claveCel == clave) {
      digitalWrite(ledPin, HIGH); // LED On
    }
  }
}
