#include "FischertechnikInterface.h"

//Static Member Init
 FischerFace* FischerFace::pInstance = NULL;
 
 
//Static Cyclic Function
 void FischerFace::CyclicTask(void)
 {
	 if (pInstance)
		 pInstance->DoCyclic();
 }

//Constructor set Pin directions.
FischerFace::FischerFace(void) : iOutputBits (8)
{
	// initialize digital pin directions
	pinMode(FT_LOAD_OUT, OUTPUT);
	pinMode(FT_LOAD_IN, OUTPUT);
	pinMode(FT_DATA_OUT, OUTPUT);
	pinMode(FT_CLOCK, OUTPUT);
	pinMode(FT_DATA_IN, INPUT);

	digitalWrite(FT_LOAD_OUT, LOW);
	digitalWrite(FT_CLOCK, HIGH);
	digitalWrite(FT_DATA_OUT, LOW);
	digitalWrite(FT_LOAD_IN, LOW);
	
	//initialize static instance pointer
	pInstance = this;
	
#ifdef FT_LIB_USE_T0
	// Timer0 is already used for millis() - we'll just interrupt somewhere
	// in the middle and call the "Compare A" function below
	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);
#endif
}

#ifdef FT_LIB_USE_T0
// Interrupt is called once a millisecond, 
SIGNAL(TIMER0_COMPA_vect) 
{
	FischerFace::CyclicTask();
}
#endif

//Set the Direction of one Motor, Motor Number is given as 1 based int.
void FischerFace::SetMotorDirections(int iMotorNr, unsigned long uwData)
{
	switch(iMotorNr)
	{
		case 1:
			SetMotorDirectionsByMask(FT_MOTOR01, uwData);
			break;
		case 2:
			SetMotorDirectionsByMask(FT_MOTOR02, uwData);
			break;
		case 3:
			SetMotorDirectionsByMask(FT_MOTOR03, uwData);
			break;
		case 4:
			SetMotorDirectionsByMask(FT_MOTOR04, uwData);
			break;

		case 5:
			SetMotorDirectionsByMask(FT_MOTOR05, uwData);
			break;
		case 6:
			SetMotorDirectionsByMask(FT_MOTOR06, uwData);
			break;
		case 7:
			SetMotorDirectionsByMask(FT_MOTOR07, uwData);
			break;
		case 8:
			SetMotorDirectionsByMask(FT_MOTOR08, uwData);
			break;			
		
		default:
			break;
	}
}

//Set the Direction of one Motor, given by a bit Offset.
void FischerFace::SetMotorDirectionsByMask(int iMotorMask, unsigned long uwData)
{
  uwInterfaceOut = (uwInterfaceOut & (~ 0x3 << iMotorMask)) | (uwData << iMotorMask);
}

//Asyncronous reading of Analog Inputs
int FischerFace::GetAnalogInputs(int iInputNr)
{
	switch (iInputNr)
	{
		case 1:
			return analogRead(FT_EX);
			break;
		case 2:
			return analogRead(FT_EY);
			break;	
		case 3:
			return analogRead(FT_EX_01);
			break;
		case 4:
			return analogRead(FT_EY_01);
			break;	
			
		default:
			break;
	}
	
	return 0;
}

bool FischerFace::GetInputState(int iInputNr)
{
	bool bReturnValue = false;
	
	switch(iInputNr)
	{
		case 1:
			bReturnValue = GetInputStateByMask(FT_INPUT_E1);
			break;
		case 2:
			bReturnValue = GetInputStateByMask(FT_INPUT_E2);
			break;
		case 3:
			bReturnValue = GetInputStateByMask(FT_INPUT_E3);
			break;
		case 4:
			bReturnValue = GetInputStateByMask(FT_INPUT_E4);
			break;
		case 5:
			bReturnValue = GetInputStateByMask(FT_INPUT_E5);
			break;
		case 6:
			bReturnValue = GetInputStateByMask(FT_INPUT_E6);
			break;
		case 7:
			bReturnValue = GetInputStateByMask(FT_INPUT_E7);
			break;
		case 8:
			bReturnValue = GetInputStateByMask(FT_INPUT_E8);
			break;		

		case 9:
			bReturnValue = GetInputStateByMask(FT_INPUT_E9);
			break;
		case 10:
			bReturnValue = GetInputStateByMask(FT_INPUT_E10);
			break;
		case 11:
			bReturnValue = GetInputStateByMask(FT_INPUT_E11);
			break;
		case 12:
			bReturnValue = GetInputStateByMask(FT_INPUT_E12);
			break;
		case 13:
			bReturnValue = GetInputStateByMask(FT_INPUT_E13);
			break;
		case 14:
			bReturnValue = GetInputStateByMask(FT_INPUT_E14);
			break;
		case 15:
			bReturnValue = GetInputStateByMask(FT_INPUT_E15);
			break;
		case 16:
			bReturnValue = GetInputStateByMask(FT_INPUT_E16);
			break;	
	}
	
	return bReturnValue;
}

//Check for one Input
bool FischerFace::GetInputStateByMask(int iInputMask)
{
  if ((uwInterfaceIn & iInputMask) == iInputMask)
    return true;
  else
    return false; 
}

//Bitbanging quantisized.
void FischerFace::DoCyclic(void)
{
    static enum eBitState CurrentState = E_BIT_STATE_HIGH;
    static int iLowCount = 0;
    static int iHighCount = 0;

   switch (CurrentState)
   {
      case E_BIT_STATE_HIGH:
      {
        //Output Section
        if (iHighCount == iOutputBits + 1) 
        {
            //Idle is LOW  
            digitalWrite(FT_LOAD_OUT, LOW);            
        } 

        //Generate Clock
        digitalWrite(FT_CLOCK, HIGH);

        //Input Section
        digitalWrite(FT_LOAD_IN, LOW); //PARALLEL/SERIAL CONTROL        

        //State Transition
        CurrentState = E_BIT_STATE_LOW;
        iHighCount++;
    

        break;  
      }
      
      case E_BIT_STATE_LOW:
      {
        digitalWrite(FT_CLOCK, LOW);

        //Input Section
        if (iLowCount == 0)
        {
          digitalWrite(FT_LOAD_IN, HIGH); //PARALLEL/SERIAL CONTROL
        }
        else if (iLowCount <= iOutputBits)
        {
          unsigned long val;
          val = digitalRead(FT_DATA_IN);

          uwInterfaceIn = (uwInterfaceIn & ~(0x1 << (iLowCount - 1))) | (val << (iLowCount - 1));    
        }

        //Output Section
        if (iLowCount < iOutputBits)
        {
          if (uwInterfaceOut & (0x01 << iLowCount))
          {
             digitalWrite(FT_DATA_OUT, HIGH);
          }
          else
          {
             digitalWrite(FT_DATA_OUT, LOW);
          }
        }
        else
        {
          //Data Idle is LOW  
           digitalWrite(FT_DATA_OUT, LOW);
       
        }

       if (iLowCount == iOutputBits)
        {
           //The data in each shift register stage is transferred to the storage register when the strobe (STR) input is HIGH.
           digitalWrite(FT_LOAD_OUT, HIGH);
        }


        //State Transition
        CurrentState = E_BIT_STATE_HIGH;
        iLowCount++;

        //Reset Counters
        if (iOutputBits + 2 == iLowCount)
        {
          iLowCount = 0;
          iHighCount = 0; 
        }
        break;  
      }      
   }	
}