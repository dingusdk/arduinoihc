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
*/
#define IHC_USEPINCHANGEINT
#include <Arduino.h>
#ifndef ESP8266
	#ifdef IHC_USEPINCHANGEINT
		#include <PinChangeInt.h>
	#else
		#include <pcint.h>
	#endif
#endif
#include "IHCInput.h"

#define STARTPULSELENGTH 2500
#define MAXPULSELENGTH 330
#define MINPULSELENGTH 149
#define THRESSHOLDPULSELENGTHLOW 166
#define THRESSHOLDPULSELENGTHHIGH 294
#define MINPULSESPACE 300

IHCinput* IHCinput::pTheFirst = NULL;


IHCinput::IHCinput() {

	dataline = 0;
#ifndef IHCINPUT_NO_MULTIPORTSUPPORT
	pNext = NULL;
#endif
}


void IHCinput::Begin( int pin) {

	// We can only call begin one time, and we can only have one object
	if (dataline != 0) return;

#ifndef IHCINPUT_NO_MULTIPORTSUPPORT
	pNext = pTheFirst;
#endif
	pTheFirst = this;

	this->pin = pin;
	pinMode(pin, INPUT);
	// -1 when searching for start pulse
	dataline = -1;
	changemask = 0;
	input = 0;
	startpulse = micros();
	// Any state change will trigger the interrupt.
#ifdef ESP8266
	attachInterrupt(digitalPinToInterrupt(pin), _PinChangeInterrupt, CHANGE);
#else
	#ifdef IHC_USEPINCHANGEINT
		attachPinChangeInterrupt( pin, _PinChangeInterrupt, CHANGE); 
	#else
		PCattachInterrupt(pin, _PinChangeInterrupt, CHANGE);
	#endif
#endif
}


#ifdef ESP8266
void ICACHE_RAM_ATTR IHCinput::_PinChangeInterrupt() {
#else
void IHCinput::_PinChangeInterrupt() {
#endif

	IHCinput* pInput = pTheFirst;
#ifdef ESP8266
	pInput->PinChangeInterrupt(digitalRead(pInput->pin));
#else
	#ifndef IHCINPUT_NO_MULTIPORTSUPPORT
		byte intpin = PCintPort::arduinoPin;
		while (pInput->pin != intpin) {
			pInput = pInput->pNext;
			if (pInput == NULL) return;
		}
	#endif
	#ifdef IHC_USEPINCHANGEINT
		pInput->PinChangeInterrupt(PCintPort::pinState);
	#else
		pInput->PinChangeInterrupt( digitalRead( pInput->pin));
	#endif
#endif
}

void IHCinput::PinChangeInterrupt(byte pinstate) {

	unsigned long time = micros();
	if (pinstate) {
		startpulse = time;
		if (dataline >= 0) {
			unsigned long l = time - endpulse;
			if (l < MINPULSESPACE) {
				dataline = -1;
				return;
			}
		}
	}
	else {
		endpulse = time;
		// pulse length
		unsigned long l = time - startpulse;
		// Looking for start pulse ?
		if (dataline < 0) {
			if (l > STARTPULSELENGTH) {
				// We have a start
				dataline = 0;
				newinput = 0;
				parity = 0;
#ifdef _DEBUG
				pulsecount++;
#endif
			}
		}
		else {
			if (l > MAXPULSELENGTH || l < MINPULSELENGTH ||
				(l > THRESSHOLDPULSELENGTHLOW && l < THRESSHOLDPULSELENGTHHIGH)) {
				// if the pulse is more than 300 it is an error - wait for new start pulse
				dataline = -1;
				return;
			}
#ifdef _DEBUG
			if (l < (THRESSHOLDPULSELENGTHHIGH + THRESSHOLDPULSELENGTHLOW) / 2) {
				if (l < lowmin) lowmin = l;
				if (l > lowmax) lowmax = l;
			}
			else {
				if (l < highmin) highmin = l;
				if (l > highmax) highmax = l;
			}
#endif
			if (dataline >= 8) {
				// We ignore bit 8-15
				if (dataline == 16) {
					// bit 16 is parity
					// done - now search for start pulse again
					dataline = -1;
					// parity error ?
					if ((l > THRESSHOLDPULSELENGTHHIGH) ^ (parity & 0x01)) {
						return;
					}
					changemask |= input ^ newinput;
					if (changemask) {
						input = newinput;
#ifndef IHCINPUT_NO_CALLBACK
						OnChange(changemask, input);
#endif
					}
				}
			}
			else if (l < THRESSHOLDPULSELENGTHLOW) {
				newinput |= 1 << dataline;
				parity++;
			}
			dataline++;
		}
	}
}

bool IHCinput::GetData(int channel) {
	if (channel < 0 || channel > 7) return false;
	return (input & (0x01 << channel)) != 0;
}

byte IHCinput::GetInput() {
	return input;
}


#ifndef IHCINPUT_NO_CALLBACK
void IHCinput::OnChange(byte changemask, byte data) {

}
#endif

byte IHCinput::GetChangeMask() {

	byte mask = changemask;
	changemask = 0;
	return mask;
}
