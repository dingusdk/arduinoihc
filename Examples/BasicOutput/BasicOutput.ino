#include <arduino.h>
#include <IHCOutput.h>

#define OUTPUTPIN 3

void setup() {

	TheIHCOutput.Begin(OUTPUTPIN);
}

word out = 0;

void loop() {

	TheIHCOutput.SetOutput( out++);
	delay( 2000);
}
