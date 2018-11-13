#include "Led.h"

#define NUM 0
#define TYPE 1

void Led::Init(byte pin)
{
	this->pins_count = 1;
	this->pins[0][NUM] = pin;
	this->pins[0][TYPE] = OUTPUT;
	pinMode(pin, OUTPUT);
}

void Led::On(byte value)
{
	analogWrite(this->pins[0][NUM], value);
}

void Led::On()
{
	digitalWrite(this->pins[0][NUM], HIGH);
}

void Led::Off()
{
	digitalWrite(this->pins[0][NUM], LOW);
}