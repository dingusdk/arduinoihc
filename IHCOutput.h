/*
(C) 2015 dingus.dk J.Ø.N.

This file is part of ArduinoIHC.

ArduinoIHC is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ArduinoIHC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ArduinoIHC.  If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------

*/
#ifndef _IHCoutput_h
#define _IHCoutput_h

typedef unsigned long dword;

//#define IHC_LOW 1
//#define IHC_HIGH 0

#define IHC_LOW 0
#define IHC_HIGH 1

class IHCtemperature;

class IHCoutput {

protected:

	int pin;
	word output;

	int pulsepos;
	// a snapshow of output including the parity bit
	// a bit mask for the bit we are sending
	dword outputmask;
	dword outputp;
	IHCoutput* pNext;

#ifndef IHC_NOTEMPERATURE
	IHCtemperature* FirstTemperature;
	IHCtemperature* TemperatureToProcess;
#endif

	static IHCoutput* pTheFirst;

	void Tick();

public:

	static void Interrupt();

	IHCoutput();
	virtual ~IHCoutput();

	void Begin(int pin);
	void SetOutput(word output);
	void Set(int channel, int state);

#ifndef IHC_NOTEMPERATURE
	void AddTemperature(IHCtemperature& temp);
#endif
};


extern IHCoutput TheIHCoutput;

#endif
