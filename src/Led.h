#include "Device.h"

class Led : virtual public Device {
public:
	void Init(byte pin);
	void On(byte value);
	void On();
	void Off();
};
