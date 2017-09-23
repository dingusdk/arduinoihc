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
#include <Arduino.h>
#include "IHCOutput.h"
#ifndef IHC_NOTEMPERATURE
#include <IHCtemperature.h>
#endif

IHCoutput* IHCoutput::pTheFirst = NULL;


IHCoutput::IHCoutput() {

	output = 0;
	outputp = 0;
#ifndef IHC_NOTEMPERATURE
	TemperatureToProcess = NULL;
	FirstTemperature = NULL;
#endif
}


IHCoutput::~IHCoutput() {
}

#ifndef ESP8266

ISR(TIMER2_COMPA_vect) {

	IHCoutput::Interrupt();
}
#endif


#ifdef ESP8266
void  ICACHE_RAM_ATTR IHCoutput::Interrupt() {
#else
void  IHCoutput::Interrupt() {
#endif

	IHCoutput* pOutput = pTheFirst;
	while (pOutput != NULL) {
		pOutput->Tick();
		pOutput = pOutput->pNext;
	}
}


void IHCoutput::Begin(int pin) {
	
	this->pin = pin;
	pinMode(pin, OUTPUT);
	digitalWrite(pin, IHC_HIGH);
	pulsepos = -30;
	outputmask = 0x01;

	pNext = pTheFirst;
	pTheFirst = this;
	if (pNext != NULL) return;

#ifdef ESP8266
	timer1_isr_init();
	timer1_attachInterrupt(Interrupt);

	timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
	timer1_write( 777);
#else
	cli();//stop interrupts

	//set timer2 interrupt at 8kHz
	TCCR2A = 0;// set entire TCCR2A register to 0
	TCCR2B = 0;// same for TCCR2B
	TCNT2 = 0;//initialize counter value to 0
	// set compare match register for 6,6666khz increments
	OCR2A = 38;// = (16*10^6) / (6666*64) - 1 (must be <256)
	// turn on CTC mode
	TCCR2A |= (1 << WGM21);
	// Set CS22 bit for 64 prescaler
	TCCR2B |= (1 << CS22);;
	// enable timer compare interrupt
	TIMSK2 |= (1 << OCIE2A);

	sei();//allow interrupts
#endif
}


void IHCoutput::Tick() {

	if (++pulsepos < 0) {
#ifndef IHC_NOTEMPERATURE
		if (TemperatureToProcess) {
			TemperatureToProcess->Tick();
			TemperatureToProcess = TemperatureToProcess->next;
		}
#endif
		return;
	}
	if (pulsepos == 0) {
		// make a snapshot of the current output
		outputp = output;
		outputmask = 0x01;
		// Calculate the parity
		int parity = 1;
		byte mask = 0x1;
		do {
			if (byte(outputp) & mask)
				parity++;
		} while (mask <<= 1);
		outputp |= dword(parity & 0x01) << 16;
	}
	if ((pulsepos & 0x3) == 0) {
		// Start of each bit
		digitalWrite(pin, IHC_LOW);
	}
	if ((pulsepos & 0x3) == 2) {
		digitalWrite(pin, IHC_HIGH);
		if (pulsepos >= 4 * 17) {
			pulsepos = -30;
#ifndef IHC_NOTEMPERATURE
			TemperatureToProcess = FirstTemperature;
#endif
			return;
		}
	}
	if ((pulsepos & 0x3) == 3) {
		if (outputp & outputmask) {
			digitalWrite(pin, IHC_LOW);
		}
		outputmask <<= 1;
	}
}


void IHCoutput::SetOutput(word output) {

	this->output = output;
}

void IHCoutput::Set(int channel, int state) {

	if (state == LOW) {
		output &=  ~(0x01 << channel);
	}
	else {
		output |=  (0x01 << channel);
	}
}

#ifndef IHC_NOTEMPERATURE

void IHCoutput::AddTemperature(IHCtemperature& temp) {

	temp.next = FirstTemperature;
	FirstTemperature = &temp;
	temp.ihcoutput = this;
}

#endif