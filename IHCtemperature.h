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
#ifndef _ihctemperature_h
#define _ihctemperature_h

class IHCoutput;

class IHCtemperature {

	int channel;
	float temperature;
	float humidity;
	float gulvtemp;
	bool gulvibrug;

	IHCtemperature* next;
	IHCoutput* ihcoutput;

	uint8_t data[9];
	int bitpos;
	unsigned long starttime;

	friend class IHCOutput;

	void Init();

	void AddBits(word value, int bits = 12);

	friend class IHCoutput;

public:
	void Tick();

	IHCtemperature(int ch);

	void SetTemperature(float t) { temperature = t; }
	void SetHumidity(float h) { humidity = h; gulvibrug = true; }
	void SetSecondTemperature(float t) { gulvtemp = t; gulvibrug = true; }

	int GetChannel() { return channel; }
};

	
#endif
