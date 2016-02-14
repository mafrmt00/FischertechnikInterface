#include "FischertechnikInterface.h"

FischerFace Interface;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

int iEX;
int iEY;

void loop() 
{
  int iDiff;
  
  iEX = Interface.GetAnalogInputs(1);
  iEY = Interface.GetAnalogInputs(2);

  Serial.print("SOLL=");  
  Serial.print(iEX);
  Serial.print(" IST="); 
  Serial.println(iEY);

  iDiff = abs(iEX - iEY);

  if (iDiff > 10)
  {
    if (iEY > iEX)
      Interface.SetMotorDirections(1, FT_MOTOR_R); 
  
    if (iEX > iEY)
      Interface.SetMotorDirections(1, FT_MOTOR_L);   
  }
  else
  {
      delay(1); //Bremsphase
      Interface.SetMotorDirections(1, FT_MOTOR_OFF);   
  } 
}
