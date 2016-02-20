#include <Arduino.h>
#include "IHCOutput.h"


IHCOutput TheIHCOutput;

IHCOutput* IHCOutput::pTheOneAndOnly = NULL;

IHCOutput::IHCTemperature::IHCTemperature(int ch) {

	channel = ch;
	memset( &data, 0, sizeof(data));
	bitpos = 0;
	gulvibrug = false;
	next = NULL;
	starttime = 0;
}

word NibbleChecksum(int n) {

	return ((n >> 8) & 0x000f) + ((n >> 4) & 0x000f) + (n & 0x000f);
}

void IHCOutput::IHCTemperature::Init() {

	int rum = int(temperature * 10.0);
	int h = int(humidity * 10.0);
	int gulv = int(gulvtemp * 10.0);
	word checksum_rum = NibbleChecksum(rum);
	word checksum_h = NibbleChecksum( h);
	word checksum_gulv = NibbleChecksum( gulv);
	word checksum_5bit = ((checksum_rum + checksum_h + checksum_gulv) & 0x001F);
	if (gulvibrug)
		checksum_5bit = checksum_5bit & 0x000F;
	else {
		checksum_5bit |= 0x0010;
		checksum_5bit ^= 0x0008;
	}
	memset(&data, 0, sizeof(data));
	bitpos = 0;
	AddBits(rum);
	AddBits(h);
	AddBits(gulv);
	AddBits(checksum_5bit, 5);

	starttime = millis();
	IHCOutput::pTheOneAndOnly->Set(channel, HIGH);
}

void IHCOutput::IHCTemperature::Tick() {

	if (starttime == 0) {
		Init();
		return;
	}
	unsigned long time = millis();
	long dt = time - starttime;
	int bitnr = dt / 122;
	if (bitnr >= 41) {
		// Wait 10 sec until next update
		if (dt > 10000) starttime = 0;
		return;
	}
	int t = dt % 122;
	int bp = 0;
	if (t >= 41) bp = 1;
	if (t >= 81) bp = 2;
	switch (bp) {
		case 0: IHCOutput::pTheOneAndOnly->Set(channel, HIGH); break;
		case 1: {
			int bytenr = bitnr / 8;
			int bit = bitnr % 8;
			if ((data[bytenr] & (1 << bit)) == 0)
				IHCOutput::pTheOneAndOnly->Set(channel, LOW);
			break;
		}
		case 2: 
			IHCOutput::pTheOneAndOnly->Set(channel, LOW); 
			break;
	}
}

void IHCOutput::IHCTemperature::AddBits(word value, int bits) {

	word valuemask = 0x01 << (bits - 1);
	int i = bitpos / 8;
	int bitofs = bitpos % 8;
	uint8_t mask = 1 << bitofs;
	do {
		if (value & valuemask) {
			data[i] |= mask;;
		}
		bitpos++;
		if ( (mask <<= 1) == 0) {
			i++;
			mask = 1;
		}
	} while (valuemask >>= 1);
}


IHCOutput::IHCOutput() {

	pTheOneAndOnly = this;
	output = 0;
	outputp = 0;
	TemperatureToProcess = NULL;
	FirstTemperature = NULL;
}


IHCOutput::~IHCOutput() {
}

#ifdef ESP8266

uint32_t usToTicks(uint32_t us) {
	return (clockCyclesPerMicrosecond() * us / 256);     // converts microseconds to tick
}

void EspTimerInterrupt() {
	IHCOutput::pTheOneAndOnly->Tick();
}

#else
ISR(TIMER2_COMPA_vect) {
	IHCOutput::pTheOneAndOnly->Tick();
}
#endif



void IHCOutput::Begin(int pin) {
	
	this->pin = pin;
	pinMode(pin, OUTPUT);
	digitalWrite(pin, IHC_HIGH);
	pulsepos = -30;
	outputmask = 0x01;
#ifdef ESP8266
	timer1_isr_init();
	timer1_attachInterrupt(EspTimerInterrupt);

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



void IHCOutput::Tick() {

	if (++pulsepos < 0) {
		if (TemperatureToProcess) {
			TemperatureToProcess->Tick();
			TemperatureToProcess = TemperatureToProcess->next;
		}
		return;
	}
	if (pulsepos == 0) {
		// take a snapshot of the current output
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
			TemperatureToProcess = FirstTemperature;
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

void IHCOutput::SetOutput(word output) {

	this->output = output;
}

void IHCOutput::Set(int channel, int state) {

	if (state == LOW) {
		output &=  ~(0x01 << channel);
	}
	else {
		output |=  (0x01 << channel);
	}
}

void IHCOutput::AddTemperature(IHCTemperature& temp) {

	temp.next = FirstTemperature;
	FirstTemperature = &temp;
}


