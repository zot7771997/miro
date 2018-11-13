#ifndef device_h
#define device_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define MAX_PINS 8 //ћаксимальное число пинов, которое может подключатьс€ к контроллеру роботу от одного устройства
#define NUM 0
#define TYPE 1

class Device
{
public:
	virtual void Sync();
	byte pins_count;
	byte pins[MAX_PINS][2];//ћассив пинов. ѕервый байт определ€ет номер пина, второй - тип (INPUT/OUTPUT).
};
#endif