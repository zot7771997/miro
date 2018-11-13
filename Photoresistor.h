#include "Device.h"

class Photoresistor : virtual public Device {
public:
	void Init(byte pin);
	float getVoltage(float vref);
	int getValue();

private:
	byte pin;
};
