#include <Wire.h>


#define SLAVE_ADDRESS 8
bool loadOn = false;

int DataReceived[5];
int Ia = 0;
int Ic = 10;
int VCC = 20;
int VBAT = 30;
int VLOAD = 40;

int VBAT_WENS = 0;
int IBAT_WENS = 0 ;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  
  delay(1000);
  SendToSlave();

  RequestFromSlave();
  for (int i = 0; i < sizeof(DataReceived) / sizeof(int); i++) {
    Serial.print(DataReceived[i]);
    Serial.print(';');
  }
  Serial.println("");
}

void RequestFromSlave() {
  Wire.requestFrom(SLAVE_ADDRESS, 10);
  while (0 < Wire.available()) { // loop through all but the last
    DataReceived[sizeof(DataReceived) / sizeof(int) - (Wire.available()/2)] = (Wire.read() | Wire.read() << 8);
    //Serial.println(Wire.read());
  }
  Ia = DataReceived[0];
  Ic = DataReceived[1];
  VCC = DataReceived[2];
  VBAT = DataReceived[3];
  VLOAD = DataReceived[4];
}
void SendToSlave(){
  Wire.beginTransmission(SLAVE_ADDRESS); // transmit to device #8        // sends five bytes
  Wire.write(VBAT_WENS);              // send the lower 8 bits
  Wire.write((VBAT_WENS >> 8));
  Wire.write(IBAT_WENS);              // sends one byte
  Wire.write((IBAT_WENS >> 8));
  Wire.write(loadOn);
  Wire.endTransmission();    // stop transmitting
}

