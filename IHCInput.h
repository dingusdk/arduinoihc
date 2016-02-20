#ifndef _ihcinput_h
#define _ihcinput_h

/*
This class simulates a IHC output module. Digital PIN 2 must be connected to 
the IHC output.
*/

class IHCinput {

protected:

	byte pin;
	volatile unsigned long startpulse;
	volatile short int dataline;
	volatile byte input;
	volatile int pulse_duration[8];
	volatile byte changemask;
	IHCinput* pNext;

	static IHCinput* pTheFirst;

	static void _PinChange();
	void PinChange();

	/*
	Callback when output changes.
	This it from the interrupt so do not make code running too long
	*/
	virtual void OnChange(byte changemask, byte data);

public:
	
	IHCinput();
	virtual ~IHCinput();

	void Begin( int pin);

	bool GetData(int channel);
	byte GetInput();
	byte GetChangeMask();
};


#endif
