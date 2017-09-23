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
*/#include <Arduino.h>
#include "IHCOutput.h"
#include "IHCtemperature.h"


IHCtemperature::IHCtemperature(int ch) {

	channel = ch;
	memset(&data, 0, sizeof(data));
	bitpos = 0;
	temperature = gulvtemp = humidity = 0;
	gulvibrug = false;
	next = NULL;
	starttime = 0;
}

word NibbleChecksum(int n) {

	return ((n >> 8) & 0x000f) + ((n >> 4) & 0x000f) + (n & 0x000f);
}

void IHCtemperature::Init() {

	int rum = int(temperature * 10.0);
	int h = int(humidity * 10.0);
	int gulv = int(gulvtemp * 10.0);
	word checksum_rum = NibbleChecksum(rum);
	word checksum_h = NibbleChecksum(h);
	word checksum_gulv = NibbleChecksum(gulv);
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
	ihcoutput->Set(channel, HIGH);
}

void IHCtemperature::Tick() {

	if (starttime == 0) {
		Init();
		return;
	}
	unsigned long time = millis();
	long dt = time - starttime;
	int bitnr = dt / 122;
	if (bitnr >= 41) {
		// Wait 8 sec until next update
		if (dt > 8000) starttime = 0;
		return;
	}
	int t = dt % 122;
	int bp = 0;
	if (t >= 41) bp = 1;
	if (t >= 81) bp = 2;
	switch (bp) {
	case 0: ihcoutput->Set(channel, HIGH); break;
	case 1: {
		int bytenr = bitnr / 8;
		int bit = bitnr % 8;
		if ((data[bytenr] & (1 << bit)) == 0)
			ihcoutput->Set(channel, LOW);
		break;
	}
	case 2:
		ihcoutput->Set(channel, LOW);
		break;
	}
}


void IHCtemperature::AddBits(word value, int bits) {

	word valuemask = 0x01 << (bits - 1);
	int i = bitpos / 8;
	int bitofs = bitpos % 8;
	uint8_t mask = 1 << bitofs;
	do {
		if (value & valuemask) {
			data[i] |= mask;;
		}
		bitpos++;
		if ((mask <<= 1) == 0) {
			i++;
			mask = 1;
		}
	} while (valuemask >>= 1);
}


