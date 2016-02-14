#ifndef FischertechnikInterface_h_
#define FischertechnikInterface_h_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//Should a Interrupt be used?
#define FT_LIB_USE_T0

//Arduino Pins for Fischer Interface 
#define FT_DATA_IN  3 //Connector #3
#define FT_LOAD_OUT 4 //Connector #13
#define FT_LOAD_IN  5 //Connector #14
#define FT_DATA_OUT 6 //Connector #11 
#define FT_CLOCK    7 //Connector #12 

#define FT_EX       A0
#define FT_EY       A1
#define FT_EX_01    A0 //Change for secondary interface
#define FT_EY_01    A1 //Change for secondary interface

//Macros for Accessing i/o
#define FT_MOTOR_R 0x1
#define FT_MOTOR_L 0x2
#define FT_MOTOR_OFF 0x00

#define FT_MOTOR01 6
#define FT_MOTOR02 4
#define FT_MOTOR03 2
#define FT_MOTOR04 0

#define FT_MOTOR05 14
#define FT_MOTOR06 12
#define FT_MOTOR07 10
#define FT_MOTOR08 8


#define FT_INPUT_E8 0x01
#define FT_INPUT_E7 0x02
#define FT_INPUT_E6 0x04
#define FT_INPUT_E5 0x08
#define FT_INPUT_E4 0x10
#define FT_INPUT_E3 0x20
#define FT_INPUT_E2 0x40
#define FT_INPUT_E1 0x80

#define FT_INPUT_E16 0x0100
#define FT_INPUT_E15 0x0200
#define FT_INPUT_E14 0x0400
#define FT_INPUT_E13 0x0800
#define FT_INPUT_E12 0x1000
#define FT_INPUT_E11 0x2000
#define FT_INPUT_E10 0x4000
#define FT_INPUT_E9  0x8000

//Statemachine for Bitbanging
enum eBitState
{
  E_BIT_STATE_HIGH,
  E_BIT_STATE_LOW
};

class FischerFace
{
  public:
    FischerFace(void);
	static void CyclicTask(void);
	
	bool GetInputState(int iInputNr);
	void SetMotorDirections(int iMotorNr, unsigned long uwData);
  
	int GetAnalogInputs(int iInputNr);
  
	static FischerFace* pInstance;
  
  private:
	//Cyclic Tasks (Bitbanging)
	void DoCyclic(void);
  
	//Mirrord Shift-registers
	unsigned long uwInterfaceOut = 0x00;
	unsigned long uwInterfaceIn = 0x00;

	//Output Bit Size (8 per cascaded Interface)
	int iOutputBits;
	
	//Internal Functions, operating on Bit Masks / Offsets
	void SetMotorDirectionsByMask(int iMotorMask, unsigned long uwData);
	bool GetInputStateByMask(int iInputMask);
	
	
};

#endif