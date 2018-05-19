#include "MCP3008.h"
#include <Wire.h>

#define CS_PIN 13
#define CLOCK_PIN 10
#define MOSI_PIN 12
#define MISO_PIN 11

// put pins inside MCP3008 constructor
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

#define VBATpin 4
#define VApin 5
#define VCpin 1
#define VLOADpin 0
#define VCCpin 6
#define IApin 2
#define ICpin 3

#define SwitchA 5
#define SwitchB 4

bool loadOn = false;

int VBAT_WENS = 0;
int IBAT_WENS = 0;                            

#define N_SAMPLES_I 16
#define N_SAMPLES_V 16

int Ia,Ic,Vcc,Vbat,Vload;


float IP, IPI, Ifeedback;
float VP,VPI,Vfeedback;

int Imeasurements[N_SAMPLES_I];
int Imedian[N_SAMPLES_V];

int Vbat_measurements[N_SAMPLES_I];
int Vbat_median[N_SAMPLES_V];

//----------------------------------------
void setup() 
{
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(115200);
  pinMode(VBATpin,INPUT);
  pinMode(VApin,INPUT);
  pinMode(VCpin,INPUT);
  pinMode(IApin,INPUT);

  pinMode(SwitchA,OUTPUT);
  pinMode(SwitchB,OUTPUT);
  /*
  Wire.begin(9); 
  Wire.onReceive(receiveEvent);
  */
}
//----------------------------------------
void loop() 
{
  if(loadOn){
    digitalWrite(SwitchB, HIGH);
  }else{
    digitalWrite(SwitchB, LOW);
  }
  
  int Idiff,Vdiff;
  for (int i=0;i<N_SAMPLES_I-1;i++) 
  {
    Imeasurements[i] = Imeasurements[i+1];     
  }
  for (int i=0;i<N_SAMPLES_V-1;i++) 
  {
    Vbat_measurements[i] = Vbat_measurements[i+1];
  }
  //Ia = analogRead(IApin);
  //Vbat = analogRead(VBATpin);
  Ia = adc.readADC(IApin);
  //delay(1);
  Vbat = adc.readADC(VApin);
  
  Imeasurements[N_SAMPLES_I-1] = Ia;
  Vbat_measurements[N_SAMPLES_V-1] = Vbat;
  //Ia = runningMedian(Imeasurements,Imedian,N_SAMPLES_I);
  //Vbat = runningMedian(Vbat_measurements,Vbat_median,N_SAMPLES_V);

  /* regulate current */
  Idiff = (IBAT_WENS-Ia);
  IP = 0.2*Idiff;
  IPI= IPI + 0.015*Idiff;
  Ifeedback = IP + IPI;
  /* regulate voltage */
  Vdiff = (VBAT_WENS-Vbat);
  VP = 3*Vdiff;
  VPI = VPI + 0.000001*Vdiff;
  Vfeedback = VP + VPI;
  
  if (Ifeedback>=0)
  {    
    float value = Ifeedback;
    value = value>255?255:value;
    if (Vfeedback>=0)
    {
      analogWrite(SwitchA,(uint8_t)value);
      //VPI=0;
    }
    else 
    {
      digitalWrite(SwitchA,LOW);
      //value = -value*Vfeedback/255;
      //analogWrite(SwitchA,(uint8_t)value);
      //digitalWrite(SwitchA,LOW);
      IPI=0;
    }
  }
  else 
  {
    digitalWrite(SwitchA,LOW);
    IPI=0;
    //VPI=0;
  }
//  if(printOn == true){

//Serial.print("VCC: ");
//Serial.print(adc.readADC(VCCpin));
//Serial.print("\t VA: ");
//Serial.print(adc.readADC(VApin));
//Serial.print("\t VBAT: ");
//Serial.print(adc.readADC(VBATpin));
//Serial.print("\t VC: ");
//Serial.print(adc.readADC(VCpin));
//Serial.print("\t VLOAD: ");
//Serial.print(adc.readADC(VLOADpin));
//Serial.print("\t IA: ");
//Serial.print(adc.readADC(IApin));
//Serial.print("\t IC: ");
//Serial.println(adc.readADC(ICpin));
/*
    Serial.print(Ia);
    Serial.print("|");
    Serial.print(Idiff);
    Serial.print("|");
    Serial.print(Ifeedback); 
    Serial.print("|"); 
    Serial.print(Vfeedback); 
    Serial.print("|");
    Serial.println(Vbat);
    */
//  }
  
}
//----------------------------------------
int runningMedian(int* values,int* median,int len)
{
  int temp;
  for (int i=0;i<len;i++) median[i] = values[i];
  for (int i=0;i<len;i++)
  {
    for (int j=0;j<len-1;j++)
    {
      if (median[j]>median[j+1])
      {
        temp = median[j];
        median[j] = median[j+1];
        median[j+1] = temp;
      }
    }
  }
  return median[len/2];
}
//----------------------------------------
void receiveEvent(int howMany) {
  VBAT_WENS = (Wire.read() | Wire.read() << 8);
  IBAT_WENS = (Wire.read() | Wire.read() << 8);
  loadOn = Wire.read();
  Serial.println("running");
  Serial.println(String(VBAT_WENS) + " ; " + String(IBAT_WENS) + " ; "+String(loadOn));
}
void requestEvent()
{
  Ia = adc.readADC(IApin);
  Ic = adc.readADC(ICpin);
  Vcc = adc.readADC(VCCpin);
  Vbat = adc.readADC(VBATpin);
  Vload = adc.readADC(VLOADpin);
  Wire.write(Ia);
  Wire.write((Ia >> 8));
  Wire.write(Ic);
  Wire.write((Ic >> 8));
  Wire.write(Vcc);
  Wire.write((Vcc >> 8));
  Wire.write(Vbat);
  Wire.write((Vbat >> 8));
  Wire.write(Vload);
  Wire.write((Vload >> 8));
}

