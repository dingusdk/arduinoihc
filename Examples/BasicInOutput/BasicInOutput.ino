#include <arduino.h>
#include <IHCInput.h>
#include <IHCOutput.h>

// Input always uses pin 2
IHCinput ihcinput;

#define OUTPUTPIN 3

void setup() {

	ihcinput.Begin();
	TheIHCOutput.Begin(OUTPUTPIN);
}

void loop() {

	TheIHCOutput.SetOutput(ihcinput.GetInput());
}