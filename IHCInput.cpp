#include <Arduino.h>
#include <PinChangeInt.h>
#include "IHCinput.h"

#define STARTPULSELENGTH 2500
#define MAXPULSELENGTH 400
#define THRESSHOLDPULSELENGTH 236

IHCinput* IHCinput::pTheFirst = NULL;


IHCinput::IHCinput() {

	dataline = 0;
	pNext = NULL;
}

IHCinput::~IHCinput() {

	//	detachInterrupt(0);
}

void IHCinput::Begin( int pin) {

	// We can only call begin one time, and we can only have one object
	if (dataline != 0) return;

	pNext = pTheFirst;
	pTheFirst = this;

	this->pin = pin;
	pinMode(pin, INPUT);
	// -1 when searching for start pulse
	dataline = -1;
	changemask = 0;
	input = 0;
	startpulse = micros();
	attachPinChangeInterrupt( pin, _PinChange, CHANGE); // Any state change will trigger the interrupt.
}


void IHCinput::_PinChange() {

	IHCinput* pInput = pTheFirst;
	byte intpin = PCintPort::arduinoPin;
	while (pInput->pin != intpin) {
		pInput = pInput->pNext;
		if (pInput == NULL) return;
	}
	pInput->PinChange();
}

void IHCinput::PinChange() {

	unsigned long time = micros();
	if (PCintPort::pinState) {
		startpulse = time;
	}
	else {
		// pulse length
		unsigned long l = time - startpulse;
		// Start pulse ?
		if (dataline < 0) {
			if (l > STARTPULSELENGTH) {
				// We have a start
				dataline = 0;
			}
		}
		else {
			if (l > MAXPULSELENGTH) {
				// if the pulse is more than 300 it is an error - wait for new start pulse
				dataline = -1;
				return;
			}
			if (dataline >= 8) {
				// We ignore bit 8-15
				if (dataline == 16) {
					// bit 16 is parity
					// done - now search for start pulse again
					dataline = -1;
					byte mask = 0x01;
					byte newinput = 0;
					byte parity = 0;
					for (int i = 0; i < 8; i++) {
						if (pulse_duration[i] < THRESSHOLDPULSELENGTH) {
							newinput |= mask;
							parity++;
						}
						mask <<= 1;
					}
					if ((l > THRESSHOLDPULSELENGTH) ^ (parity & 0x01)) {
						// parity error
						return;
					}
					changemask |= input ^ newinput;
					if (changemask) {
						input = newinput;
						OnChange(changemask, input);
					}
				}
				dataline++;
				return;
			}
			pulse_duration[dataline++] = l;
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


void IHCinput::OnChange(byte changemask, byte data) {

}

byte IHCinput::GetChangeMask() {

	byte mask = changemask;
	changemask = 0;
	return mask;
}
