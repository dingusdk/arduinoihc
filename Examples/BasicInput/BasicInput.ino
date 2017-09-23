/*
(C) 2015 dingus.dk J.Ø.N.

This file is part of ArduinoIHC.

ArduinoIHC is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ArduinoIHC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ArduinoIHC.If not, see <http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------------

This example will create an IHCInput on arduino port 2, and print the input 
value (binary) on the serial port when it changes.

*/
#include <Arduino.h>
#include <IHCInput.h>

IHCinput ihcinput;


void setup() {

	ihcinput.Begin( 2);
	Serial.begin(115200);
	Serial.println("Start");
}

void loop() {

	while (ihcinput.GetChangeMask() == 0) {
		// Whitout this delay ESP8266 will reboot?
		delay(1);
	}
	Serial.println(ihcinput.GetInput(),BIN);
}