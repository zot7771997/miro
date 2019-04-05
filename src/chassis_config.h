#ifndef chassis_config_h
#define chassis_config_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define PIN_VBAT A0	//��� ��� ��������� ���������� ������� (������ 1S - �� 4.2�)
#define VOLTAGE_DIVIDER 1

#define WHEEL_COUNT 2	//����� ������� ����� � ������

static byte wheel_PWM_pins[WHEEL_COUNT] = { 5, 6 };
//wheel_PWM_pins[0] = 4;
//wheel_PWM_pins[1] = 7;

static byte wheel_DIR_pins[WHEEL_COUNT] = { 4, 7 };
//wheel_DIR_pins[0] = 5;
//wheel_DIR_pins[1] = 6;

static byte wheel_TACHOM_pins[WHEEL_COUNT] = { 2, 3 };
//wheel_TACHOM_pins[0] = 2; //��� ��������� ������ 0 (���� ��� ������������ ���������� ����������)
//wheel_TACHOM_pins[1] = 3; //��� ��������� ������ 1 (���� ��� ������������ ���������� ����������)

#define WHEEL_SEGMENTS (30.0) //����� ��������� ��������� ������� ���������. ���� ���������� �� 18, ����� ������� ������ �� 90 �������� ��� ������.
#define WHEEL_RADIUS (0.0345) //������ ������ (������)

#define MEAN_DEPTH 8 //����� �� �������! ������� ������� ��������� �������� �� ���������� (������ ������� ��� ����������)

/*��� ��������� ��������, ����������� ������ ��� ����������.*/
#define WHEEL_ANGLE_SPEED_MAX (1500) //������������ ������� �������� �������� �����, ������� ����� ������� ��������� (����/���). ���� ���� �������� ������ ��������� �� �����
#define WHEEL_ANGLE_SPEED_MIN (100) //����������� ������� �������� �������� ����� (����/���). ���� ���� �������� ������ ��������� �� �����
#define MAXWHEELBREAKLDELAY (100)

#define EEPROM_WHEEL_TABLE_ADDR 0
#define WHEEL_TABLE_SIZE (10)

#define DEFAULT_PID_Kp (0.0192) //0.039 - 2,5� ������ ���������
#define DEFAULT_PID_Kd (0.29)
#define DEFAULT_PID_Ki (0.000265)

//#############################################################################################################
//#############################################################################################################

static volatile unsigned long _wheelTimeArray[WHEEL_COUNT][MEAN_DEPTH];
static volatile long _wheelTime[WHEEL_COUNT];
static volatile long _wheelTime_last[WHEEL_COUNT];
static volatile unsigned int _wheelTachomCount[WHEEL_COUNT];
static volatile bool _syncloop[WHEEL_COUNT] = { false, false };

static void wheelISR0()
{
	_wheelTime[0] = micros();
	_wheelTimeArray[0][_wheelTachomCount[0] % MEAN_DEPTH] = _wheelTime[0] - _wheelTime_last[0];
	_wheelTime_last[0] = _wheelTime[0];
	_wheelTachomCount[0]++;
	_syncloop[0] = true;
}

static void wheelISR1()
{
	_wheelTime[1] = micros();
	_wheelTimeArray[1][_wheelTachomCount[1] % MEAN_DEPTH] = _wheelTime[1] - _wheelTime_last[1];
	_wheelTime_last[1] = _wheelTime[1];
	_wheelTachomCount[1]++;
	_syncloop[1] = true;
}

static void(*wheel_ISR[WHEEL_COUNT])() = { wheelISR0, wheelISR1 };

#endif
