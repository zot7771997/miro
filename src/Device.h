#ifndef device_h
#define device_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define MAX_PINS 8 //������������ ����� �����, ������� ����� ������������ � ����������� ������ �� ������ ����������
#define NUM 0
#define TYPE 1

class Device
{
public:
	virtual void Sync();
	byte pins_count;
	byte pins[MAX_PINS][2];//������ �����. ������ ���� ���������� ����� ����, ������ - ��� (INPUT/OUTPUT).
};
#endif