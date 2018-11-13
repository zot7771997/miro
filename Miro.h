#ifndef miro_h
#define miro_h

#include "Chassis.h"
#include "Device.h"

char MIRO_VERSION[] = "v1.0.0";

#define MAX_DEVICES 16	//������������ ����� ���������, ������� ����� ���������� � ������.
#define ROBOT_DIAMETER (0.16) //���������� ����� ����� � ������ ������� (������)

#define LEFT 0
#define RIGHT 1

class Miro {
public:

	Chassis chassis;
	
	void Init();
	
	void Sync();

	/*�������� ������.
	lin_speed - �������� �������� ������ (�/�),
	ang_speed - ������� �������� ������ (����/���),
	dist - ����� ����, ������� ����� ���������� ������ (�),
	*/
	int MoveDist(float lin_speed, float ang_speed, float dist); //�������� ������ � ��������� �������� � �������� ����������

	/*������� ������ �� �����.
	ang - ���� ��������,
	ang_speed - ������� �������� �������� (����/���),
	*/
	int RotateAng(float ang, float ang_speed);

	/*���������� ������� ������� ����� (���/���).*/
	float getAngSpeedRad();

	/*���������� ������� ������� ����� (����/���).*/
	float getAngSpeed();

	/*���������� �������� ������� ����� (�/�).*/
	float getLinSpeed();

	/*���������� ����� ����������� ����� ����.*/
	float getPath(); //�� ����������� ��������� - ��� ������� ����������� �������� �� �����, ����� ����� ���� ������ ���������� ������ �� ��������

	/*����������� ���������� � ������*/
	int attachDevice(Device *dev);

	int dettachDevice(Device *dev);
	int dettachDevice(byte DeviceIndex);

	char* version = MIRO_VERSION;
	
private:

	Device* devices[MAX_DEVICES];
	unsigned int _device_count;
};

#endif