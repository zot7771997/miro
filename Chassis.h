#ifndef chassis_h
#define chassis_h

#include "chassis_config.h"

class Chassis {
public:
	/*������������� (���������� �����, �� �������, ���������� � ��. - ������� ������ ���������� � setup()*/
	void Init();

	void Sync();

	/*�������� � ��������� ���������� �������*/
	float getVoltage();

	/*���������� ����������. � �������� ���������� ������������ ������������ � ����������� �������� �������� ����������.
	��� ���������� � ������ EEPROM ��������� ������� ���� "<������� �������� ������> - <���������� �� ������>".
	���������� ���������� ��������� ���� ��� �� ����� ������ � ��������� ���������� � ������������������ ��������.
	�� ������ ��������� ���������� ��������� ������ ���������, ������� ����� ���������� ��������� �� ���������.
	������ ���������� � ���� ���������� ������������ �� ������ �������� ��� ��������� ��������� �������� ��� ����������,
	� ����������� �� ��������� ������� �������� ������.
	*/
	void wheelCalibrate();

	/*��������� �������� �����. ��������� �������!
	speed[] - ������-�������� ������� ��������� (����/���) ��� ������� ������
	ang[] - ������-������� ����� �������� ������� ������ (����)
	*/
	int wheelSyncRotateAng(float *speed, float *ang);

	/*�������� ����� �������� ��������� (��� ����������� �� ������� ��� ���� - ���� ������ �� ����� ���� �����������).
	������� �� ������� ���������� ���������������. ������� ���������� ��������� ������ ����� ����������� � ������� Sync().
	*/
	int wheelRotate(float *speed); //��������� �� �����������

	/*���������� �������� �������� ��������� ���������� �����-������*/
	unsigned long wheelGetTachom(byte wheel);

	/*����� �������� �������� ��������� ���������� ����� - ������*/
	void wheelResetTachom(byte wheel);

	/*���������� �������� ������� �������� ���������� ������ (����/���)*/
	float wheelGetAngSpeed(byte wheel);

	/*���������� �������� ������� �������� ���������� ������ (���/���)*/
	float wheelGetAngSpeedRad(byte wheel);

	/*���������� True ���� ������ ��������� � False ���� �� ���������*/
	bool wheelIsMoving(byte wheel);

	/*���������� ����� ����, ����������� ��������� ������� (������)*/
	float wheelGetPath(byte wheel);

	/*���������� �������� �������� ��� ������ (�/�)*/
	float wheelGetLinSpeed(byte wheel);

	/*�������� ������ ������.
	speed - �������� ������� �������� (����/���) ��� ������
	ang - ������� ���� �������� ������ (����)
	wheel - ����� ������
	*/
	int wheelRotateAng(float speed, float ang, byte wheel);

private:
	void _wheel_rotate(byte wheel);
	void _wheel_rotate_sync();
	int _eepromReadWheelTable(float *table);
	int _eepromWriteWheelTable(float *table);
	int _wheelGetU(float ang_speed, int wheel, float volts);

	float _calib_wheel_table[WHEEL_COUNT][2][WHEEL_TABLE_SIZE];
	char _wheelDir[WHEEL_COUNT];
	float _wheelAngSpeed[WHEEL_COUNT];

	float _wheelSetAng[WHEEL_COUNT];
	float _wheelSetAngSpeed[WHEEL_COUNT];

	unsigned long _wheelLastSync[WHEEL_COUNT];
	bool _wheelIsMoving[WHEEL_COUNT];

	float _vbat; //���������� ������� ������

	bool _wheel_sync_move;
	bool _wheel_move[WHEEL_COUNT];
};

#endif