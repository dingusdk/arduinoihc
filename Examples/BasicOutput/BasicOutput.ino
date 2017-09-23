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

This example will create 2 IHCOutput on arduino port 2 and 3. Port 2 will count,
and port 3 will shift 1 bit. 

*/
#include <Arduino.h>
#include <IHCOutput.h>

IHCoutput output1;
IHCoutput output2;


void setup() {

	output1.Begin(3);
	output2.Begin(2);
	Serial.begin(115200);
	Serial.println("Start");
}

word out = 0;
byte out2 = 0;

void loop() {

	Serial.print("Out1 ");
	Serial.print( out);
	Serial.print(" Out2 ");
	Serial.print(out2,BIN);
	Serial.println("");

	output1.SetOutput( out++);
	output2.SetOutput(out2);
	if (out2 == 0)
		out2 = 1;
	else
		out2 <<= 1;
	delay( 2000);
}
