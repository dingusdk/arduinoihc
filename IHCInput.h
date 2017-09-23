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

The IHCinput class simulates an IHC input module.

To make the code footprint smaller you can define the following to remove
features

#define IHCINPUT_NO_CALLBACK
This will remove function for callback on change

#define IHCINPUT_NO_MULTIPORTSUPPORT

Multiport support is only suppoted using the PinChangeInt library

#define IHC_USEPINCHANGEINT
This will use the PinChangeInt library

Also note the defines in PinChangeInt.h:
#define NO_PORT?_PINCHANGE
*/
#ifndef _ihcinput_h
#define _ihcinput_h
#define IHC_USEPINCHANGEINT
#ifdef ESP8266
#define IHCINPUT_NO_MULTIPORTSUPPORT
#endif

#ifndef IHC_USEPINCHANGEINT
#define IHCINPUT_NO_MULTIPORTSUPPORT
#endif

class IHCinput {

protected:

	byte pin;
	volatile unsigned long startpulse;
	volatile short int dataline;
	volatile byte input;
	volatile byte newinput;
	volatile byte parity;
	volatile byte changemask;
#ifndef IHCINPUT_NO_MULTIPORTSUPPORT
	IHCinput* pNext;
#endif
	static IHCinput* pTheFirst;

	static void _PinChangeInterrupt();
	void PinChangeInterrupt( byte pinstate);

#ifndef IHCINPUT_NO_CALLBACK
	/*
	Callback when output changes.
	This it from the interrupt - do NOT make code running too long
	*/
	virtual void OnChange(byte changemask, byte data);
#endif

public:
	IHCinput();

	void Begin( int pin);

	bool GetData(int channel);
	byte GetInput();
	byte GetChangeMask();
};


#endif
