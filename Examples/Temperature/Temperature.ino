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

IHCoutput output;
IHCtemperature temperature( 0);
IHCtemperature temperature2(2);
IHCtemperature temperatureAndHumidity(1);


void setup() {

	output.Begin(13);
	temperature.SetTemperature(20);
	temperature.SetSecondTemperature(20);
	temperatureAndHumidity.SetTemperature(20);
	temperatureAndHumidity.SetHumidity(50);
	temperature2.SetTemperature(20);
	temperature.SetSecondTemperature(0);
	output.AddTemperature( temperature);
	output.AddTemperature(temperature2);
	output.AddTemperature( temperatureAndHumidity);

	Serial.begin(115200);
	Serial.println("Start");
}

void loop() {

	Serial.println("Setting temperature");
	for (float i = 15; i < 25; i += 1) {
		temperature.SetTemperature(i);
		temperatureAndHumidity.SetTemperature(i);
		temperature2.SetTemperature(i);
		delay(8000);
	}
	Serial.println("Setting humidity/temp2");
	for (float i = 45; i < 55; i += 0.5) {
		temperature.SetSecondTemperature( i - 30);
		temperatureAndHumidity.SetHumidity( i);
		delay(8000);
	}
}
