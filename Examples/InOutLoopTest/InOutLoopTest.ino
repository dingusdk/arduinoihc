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

This test has 1 IHCinput and 1 IHCoutput on port 2 and 13. 
You should connect port 2 and 3 on the arduino.
The output will count up and print the result on the serial port.
The input will also be printed on the serial port.
If the input and output does not match "ERROR" is printed on the serial port
*/
#include <Arduino.h>
#include <IHCInput.h>
#include <IHCOutput.h>

IHCinput ihcinput;
IHCoutput ihcoutput;

void setup() {

	ihcinput.Begin( 2);
	ihcoutput.Begin( 13);
	Serial.begin(115200);
	Serial.println("Start");
}

byte out = 1;

void loop() {

	Serial.print("Out ");
	Serial.println( out);
	ihcoutput.SetOutput( out);
	delay(500);
	byte in = ihcinput.GetInput();
	Serial.print("In  ");
	Serial.println( in);
	if (out != in) Serial.println("ERROR");

	out++;
}