#include <arduino.h>
#include <IHCInput.h>

IHCinput ihcinput1;
IHCinput ihcinput2;


void setup() {

  ihcinput1.Begin( 4);
	ihcinput2.Begin(5);
	Serial.begin( 115200);
  Serial.println( "Start");
}

void loop() {

	if (byte mask = ihcinput1.GetChangeMask()) {
		Serial.print("IHC1 ");
		Serial.print(ihcinput1.GetInput());
		Serial.print(" mask ");
		Serial.println(mask,BIN);
	}
	if (byte mask = ihcinput2.GetChangeMask()) {
		Serial.print("IHC2 ");
		Serial.print(ihcinput2.GetInput());
		Serial.print(" mask ");
		Serial.println(mask,BIN);
	}
}
