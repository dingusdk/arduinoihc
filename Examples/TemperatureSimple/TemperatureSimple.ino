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

*/
#include <Arduino.h>
#include <IHCOutput.h>
#include <IHCtemperature.h>

/*
 * Data line output is set to GPIO 8 below.
 * The temperature is on channel 0
 * In your IHC project you should connect a temperature sensor to channel 1 on the 
 * input dataline you have connected GPIO 8 to.
 */
 
IHCoutput output;
// Channel 0 on the output (Will be 1 in IHC Visual because it is 1 based)
IHCtemperature temperature( 0);

void setup() {

  // Use GPIO 8
	output.Begin(8);
	temperature.SetTemperature(20);
	temperature.SetSecondTemperature(19);
	output.AddTemperature( temperature);

	Serial.begin(115200);
	Serial.println("Start");
}

void loop() {

	Serial.println("Setting temperature");
	for (float i = 15; i < 25; i += 1) {
  	temperature.SetTemperature(i);
    temperature.SetSecondTemperature(i-1);
		delay(8000);
	}
}
