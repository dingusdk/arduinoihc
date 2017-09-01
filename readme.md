# Introduction

This is an Arduino library for making an IHC in or output module using an Arduino.
(IHC controller is a home automation controller made by LK)
Additionally ArduinoIHC should work on ESP8266

## Connecting your IHC controller to Arduino

The IHC Controller and Arduino are both standard 5v TTL levels so you can 
connect them directly. I will recommend you use a 1K resistor just to be 
more safe, or if you want complete seperation use an optocoupler. 

## Interrupt library

ArduinoIHC can use either the "pcint" library http://playground.arduino.cc/Main/PcInt

or the "PinChangeInt" library https://github.com/GreyGnome/PinChangeInt

define IHC_USEPINCHANGEINT to use PinChangeInt otherwise pcint is used by default.
PinChangeInt is required to have more than one IHCinput.

## Examples

See the examples folder

# License

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

