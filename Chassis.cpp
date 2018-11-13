#include "Chassis.h"

#include "math.h"
#include "EEPROM.h"

#define MB_PI (3.14159265)
#define MB_PI2ANG (57.2957795)

#define SEGMENT_ANGLE (360.0/WHEEL_SEGMENTS)
#define WHEEL_TIME_MIN (360000000.0 / (WHEEL_ANGLE_SPEED_MAX * WHEEL_SEGMENTS))
#define WHEEL_TIME_MAX (360000000.0 / (WHEEL_ANGLE_SPEED_MIN * WHEEL_SEGMENTS))
#define WHEEL_TIME(speed) (360000000.0 / ((speed) * WHEEL_SEGMENTS))

#define VOLTS (0)
#define SPEED (1)

unsigned int dist_wheelTachomCount[WHEEL_COUNT];

float U[WHEEL_COUNT];
byte U0[WHEEL_COUNT];
bool dir[WHEEL_COUNT];
float Error[WHEEL_COUNT];
float Error1[WHEEL_COUNT];
float I[WHEEL_COUNT];

unsigned long wheel_time_max[WHEEL_COUNT];
unsigned long wheel_time_min[WHEEL_COUNT];

//PUBLIC

void Chassis::Init()
{
	pinMode(PIN_VBAT_1S, INPUT);

	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		pinMode(wheel_TACHOM_pins[w], INPUT);

		pinMode(wheel_PWM_pins[w], OUTPUT);
		pinMode(wheel_DIR_pins[w], OUTPUT);

		this->_wheelDir[w] = 1;

		_wheelTachomCount[w] = 0;

		attachInterrupt(digitalPinToInterrupt(wheel_TACHOM_pins[w]), wheel_ISR[w], RISING);
		this->_wheel_move[w] = false;
	}

	this->_wheel_sync_move = false;

	this->_eepromReadWheelTable((float*)this->_calib_wheel_table);
	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		wheel_time_max[w] = WHEEL_TIME(this->_calib_wheel_table[w][SPEED][0]);//�. �. ��� ����� ��������� ������� ��� ����� - ��� � ������
		wheel_time_min[w] = WHEEL_TIME(this->_calib_wheel_table[w][SPEED][WHEEL_TABLE_SIZE - 1]);
	}
	/*
	Serial.println("������������� ������� ����������");
	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		Serial.print("������ ");
		Serial.println(w);
		Serial.print("����������");
		Serial.print("   ");
		Serial.println("����/���");

		for (int i = 0; i < WHEEL_TABLE_SIZE; i++)
		{
			Serial.print(this->_calib_wheel_table[w][VOLTS][i]);
			Serial.print("   ");
			Serial.println(this->_calib_wheel_table[w][SPEED][i]);
		}
		Serial.println();
	}
	*/
}

void Chassis::Sync()
{
	unsigned long wheelTime = 0;
	unsigned long synctime = micros();

	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		wheelTime = 0;
		if ((synctime - _wheelTime[w]) > (2 * wheel_time_max[w])) //������ ��������
		{
			for (byte i = 0; i < MEAN_DEPTH; i++)
			{
				_wheelTimeArray[w][i] = 0;
			}
			this->_wheelAngSpeed[w] = 0;
			this->_wheelIsMoving[w] = false;
		}
		else if (_syncloop[w]) //������ ��������� � ��������� ��������� ����������� ��������� ������� ������
		{
			_syncloop[w] = false;
			wheelTime = 0;
			byte k = 0;
			for (byte i = 0; i < MEAN_DEPTH; i++)
			{
				if ((_wheelTimeArray[w][i] <= wheel_time_max[w]) && (_wheelTimeArray[w][i] >= wheel_time_min[w]))
				{
					wheelTime = wheelTime + _wheelTimeArray[w][i];
					k++;
				}
			}
			this->_wheelAngSpeed[w] = 0;
			if (k > 0)
			{
				wheelTime = wheelTime / k;
				this->_wheelAngSpeed[w] = (360000000.0 * (float)_wheelDir[w]) / (WHEEL_SEGMENTS *  wheelTime);
				this->_wheelIsMoving[w] = true;
			}
		}
	}//��������� ������� �������� �����

	/*
	if (this->_wheel_sync_move)//���� ������ ��������� �������� �����
	{
		byte w;
		for (w = 0; w < WHEEL_COUNT; w++)
		{
			if (dist_wheelTachomCount[w] <= _wheelTachomCount[w]) break; //����� �� ����� ��� ���������� ������������ ����� ������� ������� - ������� ��������
		}
		if (w < WHEEL_COUNT)
		{
			this->_wheel_sync_move = false;
			for (w = 0; w < WHEEL_COUNT; w++)
			{
				this->_wheel_move[w] = false;
				dist_wheelTachomCount[w] = _wheelTachomCount[w];
			}
			for (byte w = 0; w < WHEEL_COUNT; w++)
			{
				digitalWrite(wheel_DIR_pins[w], !dir[w]);
				digitalWrite(wheel_PWM_pins[w], dir[w]);
			}
			delay(25);//�������� ��� �������� �������� - ������ �� ��� ����� �� ��������� �������� �������� ����������
			for (byte w = 0; w < WHEEL_COUNT; w++)
			{
				digitalWrite(wheel_DIR_pins[w], dir[w]);
			}
		}
	}//���� ���� ������ �� ���������� �������� � ������ ���� �� ����� ��� ��������� - ������� ��������� �����
	*/

	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		if (this->_wheel_move[w])
		{
			if (_syncloop[w]) //������ ��������� � ��������� ��������� ����������� ��������� ������� ������
			{
				_syncloop[w] = false;
				Error[w] = (this->_wheelSetAngSpeed[w] - this->_wheelAngSpeed[w]);
				//if ((Error[w] < 3) && (Error[w] > -3)) Error[w] = 0;
				U[w] = U[w] + DEFAULT_PID_Kp * Error[w];
				U[w] = U[w] + DEFAULT_PID_Kd * (Error[w] - Error1[w]);
				Error1[w] = Error[w];
				I[w] = I[w] + DEFAULT_PID_Ki*Error[w];
				U[w] = U[w] + I[w];
			}

			if (U[w] > 255) U[w] = 255;
			if (U[w] < -255) U[w] = -255;
			U0[w] = abs((int)U[w]);
			if (dir[w])
			{
				U0[w] = 255 - U0[w];
			}
		}
		else
		{
			U0[w] = 0;
		}
	}
	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		analogWrite(wheel_PWM_pins[w], U0[w]);
	}

	return;
}

int Chassis::wheelSyncRotateAng(float *speed, float *ang)
{
	byte w;
	for (w = 0; w < WHEEL_COUNT; w++)
	{
		if ((fabs(speed[w]) < this->_calib_wheel_table[w][SPEED][0]) && (fabs(speed[w]) > 0))
		{
			return -1; //������ - ���������� ��������� ������� ������ � ����� ��������� ���������
		}
		if ((fabs(ang[w]) < float(SEGMENT_ANGLE)) && (fabs(speed[w]) > 0))
		{
			return -2; //������ - ���������� ��������� ������� ������ �� ����� ��������� ����
		}
	}

	for (w = 0; w < WHEEL_COUNT; w++)
	{
		if (speed[w] != 0) break;
	}
	if (w == WHEEL_COUNT) //��������� �������
	{
		for (w = 0; w < WHEEL_COUNT; w++)
		{
			analogWrite(wheel_PWM_pins[w], 0);
			digitalWrite(wheel_DIR_pins[w], LOW);
			this->_wheelSetAng[w] = 0;
			this->_wheelSetAngSpeed[w] = 0;
			this->_wheel_move[w] = false;
		}
		this->_wheel_sync_move = false;
		return 0;
	}
	else
	{
		for (byte w = 0; w < WHEEL_COUNT; w++)
		{
			this->_wheelSetAng[w] = ang[w];
			this->_wheelSetAngSpeed[w] = speed[w];
		}
		this->_wheel_sync_move = true;
	}

	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		if ((this->_wheelSetAng[w] * this->_wheelSetAngSpeed[w]) >= 0) this->_wheelDir[w] = 1;
		else this->_wheelDir[w] = -1;

		dist_wheelTachomCount[w] = _wheelTachomCount[w] + int(0.51 + (fabs(this->_wheelSetAng[w]) / float(SEGMENT_ANGLE))); //����� ��������� ��������� ������� ������, ������� ����� ���������, ����� ��������� �� ��������� ����;
		//Serial.println(float(SEGMENT_ANGLE));
		//Serial.println(fabs(this->_wheelSetAng[w]) / float(SEGMENT_ANGLE));
		//Serial.println(dist_wheelTachomCount[w] - _wheelTachomCount[w]);
		this->_vbat = getVoltage();
		U[w] = this->_wheelGetU(fabs(this->_wheelSetAngSpeed[w]), w, this->_vbat);
		if (U[w] < 0) return -1;
		U[w] = (float)this->_wheelDir[w] * U[w];
		I[w] = 0;
	}

	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		for (byte i = 0; i < MEAN_DEPTH; i++)
		{
			_wheelTimeArray[w][i] = 0;
		}

		dir[w] = false;
		if (this->_wheelDir[w] < 0) dir[w] = true;

		this->_wheelSetAngSpeed[w] = (float)this->_wheelDir[w] * fabs(this->_wheelSetAngSpeed[w]);

		digitalWrite(wheel_DIR_pins[w], dir[w]);
		digitalWrite(wheel_PWM_pins[w], dir[w]);
		this->_wheel_move[w] = true;
	}

	this->_wheel_rotate_sync();

	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		this->_wheel_move[w] = false;
	}

	return 0;
}

int  Chassis::wheelRotateAng(float speed, float ang, byte wheel)
{
	if ((fabs(speed) < this->_calib_wheel_table[wheel][SPEED][0]) && (fabs(speed) > 0))
	{
		return -1; //������ - ���������� ��������� ������� ������ � ����� ��������� ���������
	}

	if ((fabs(ang) < float(SEGMENT_ANGLE)) && (fabs(speed) > 0))
	{
		return -2; //������ - ���������� ��������� ������� ������ �� ����� ��������� ����
	}

	if (speed == 0) //��������� ������
	{
		analogWrite(wheel_PWM_pins[wheel], 0);
		digitalWrite(wheel_DIR_pins[wheel], LOW);
		this->_wheelSetAng[wheel] = 0;
		this->_wheelSetAngSpeed[wheel] = 0;
		this->_wheel_move[wheel] = false;
		return 0;
	}
	else
	{
		this->_wheelSetAng[wheel] = ang;
		this->_wheelSetAngSpeed[wheel] = speed;
	}
	
	if ((this->_wheelSetAng[wheel] * this->_wheelSetAngSpeed[wheel]) >= 0) this->_wheelDir[wheel] = 1;
	else this->_wheelDir[wheel] = -1;

	dist_wheelTachomCount[wheel] = _wheelTachomCount[wheel] + int(0.51 + (fabs(this->_wheelSetAng[wheel]) / float(SEGMENT_ANGLE))); //����� ��������� ��������� ������� ������, ������� ����� ���������, ����� ��������� �� ��������� ����;
	this->_vbat = getVoltage();
	U[wheel] = this->_wheelGetU(fabs(this->_wheelSetAngSpeed[wheel]), wheel, this->_vbat);
	if (U[wheel] < 0) return -1;
	U[wheel] = (float)this->_wheelDir[wheel] * U[wheel];
	I[wheel] = 0;

	for (byte i = 0; i < MEAN_DEPTH; i++)
	{
		_wheelTimeArray[wheel][i] = 0;
	}
	dir[wheel] = false;
	if (this->_wheelDir[wheel] < 0) dir[wheel] = true;
	this->_wheelSetAngSpeed[wheel] = (float)this->_wheelDir[wheel] * fabs(this->_wheelSetAngSpeed[wheel]);
	digitalWrite(wheel_DIR_pins[wheel], dir[wheel]);
	digitalWrite(wheel_PWM_pins[wheel], dir[wheel]);

	this->_wheel_move[wheel] = true;
	this->_wheel_rotate(wheel);
	this->_wheel_move[wheel] = false;

	return 0;
}

int Chassis::wheelRotate(float *speed)
{
	byte w;
	for (w = 0; w < WHEEL_COUNT; w++)
	{
		if ((fabs(speed[w]) < this->_calib_wheel_table[w][SPEED][0]) && (fabs(speed[w]) > 0))
		{
			return -1; //������ - ���������� ��������� ������� ������ � ����� ��������� ���������
		}
	}

	this->_vbat = getVoltage();

	for (w = 0; w < WHEEL_COUNT; w++)
	{
		if (speed[w] == 0) //��������� ������
		{
			analogWrite(wheel_PWM_pins[w], 0);
			digitalWrite(wheel_DIR_pins[w], LOW);
			this->_wheelSetAngSpeed[w] = 0;
			this->_wheelDir[w] = 1;
			this->_wheel_move[w] = false;
		}
		else
		{
			this->_wheelSetAngSpeed[w] = speed[w];
			this->_wheel_move[w] = true;
			if (this->_wheelSetAngSpeed[w] >= 0) this->_wheelDir[w] = 1;
			else this->_wheelDir[w] = -1;
			dist_wheelTachomCount[w] = 65535;
			U[w] = this->_wheelGetU(fabs(this->_wheelSetAngSpeed[w]), w, this->_vbat);
			if (U[w] < 0) return -2; //������ ����� ��������???
			U[w] = (float)this->_wheelDir[w] * U[w];
			I[w] = 0;
		}
	}

	return 0;
}

bool Chassis::wheelIsMoving(byte wheel)
{
	return this->_wheelIsMoving[wheel];
}

float Chassis::wheelGetAngSpeedRad(byte wheel)//������������� � Sync()
{
	return this->_wheelAngSpeed[wheel] / MB_PI2ANG;
}

float Chassis::wheelGetAngSpeed(byte wheel)//������������� � Sync()
{
	return (this->_wheelAngSpeed[wheel]);
}

void Chassis::wheelResetTachom(byte wheel)
{
	_wheelTachomCount[wheel] = 0;
}

unsigned long Chassis::wheelGetTachom(byte wheel)
{
	return _wheelTachomCount[wheel];
}

float Chassis::wheelGetPath(byte wheel)
{
	return 2 * MB_PI * WHEEL_RADIUS * ((float)_wheelTachomCount[wheel] / WHEEL_SEGMENTS);
}

float Chassis::wheelGetLinSpeed(byte wheel)
{
	return this->_wheelAngSpeed[wheel] * WHEEL_RADIUS;
}

void Chassis::wheelCalibrate()
{
	unsigned long wheelTime = 0;
	this->_vbat = this->getVoltage();

	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		digitalWrite(wheel_DIR_pins[w], LOW);
		digitalWrite(wheel_PWM_pins[w], LOW);

		for (int i = 0; i < MEAN_DEPTH; i++)
		{
			_wheelTimeArray[w][i] = 0;
		}
		this->_wheelAngSpeed[w] = 0;
	}

	unsigned int URL = 0;

	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		URL = 0;
		while ((this->_wheelAngSpeed[w] == 0) && (URL < 255))
		{
			URL++;
			analogWrite(wheel_PWM_pins[w], URL);
			delay(WHEEL_TIME_MAX * 2 / 1000);

			wheelTime = 0;
			int k = 0;
			for (int i = 0; i < MEAN_DEPTH; i++)
			{
				if ((_wheelTimeArray[w][i] <= WHEEL_TIME_MAX) && (_wheelTimeArray[w][i] >= WHEEL_TIME_MIN))
				{
					wheelTime = wheelTime + _wheelTimeArray[w][i];
					k++;
				}
			}
			if (k > 0)
			{
				wheelTime = wheelTime / k;
				this->_wheelAngSpeed[w] = (360.0 * (float)_wheelDir[w] * 1000000.0) / (WHEEL_SEGMENTS *  wheelTime);
			}
			else
			{
				this->_wheelAngSpeed[w] = 0;
			}
		}

		byte cstep = (255 - URL) / (WHEEL_TABLE_SIZE - 1);
		for (byte i = 0; i < WHEEL_TABLE_SIZE; i++)
		{
			if (URL > 255) URL = 255;
			analogWrite(wheel_PWM_pins[w], URL);
			dist_wheelTachomCount[w] = _wheelTachomCount[w] + 2 * WHEEL_SEGMENTS;
			while (dist_wheelTachomCount[w] > _wheelTachomCount[w]);

			wheelTime = 0;
			int k = 0;
			for (int i = 0; i < MEAN_DEPTH; i++)
			{
				if ((_wheelTimeArray[w][i] <= WHEEL_TIME_MAX) && (_wheelTimeArray[w][i] >= WHEEL_TIME_MIN))
				{
					wheelTime = wheelTime + _wheelTimeArray[w][i];
					k++;
				}
			}
			if (k > 0)
			{
				wheelTime = wheelTime / k;
				this->_wheelAngSpeed[w] = (360.0 * (float)this->_wheelDir[w] * 1000000.0) / (WHEEL_SEGMENTS *  wheelTime);
			}
			else
			{
				this->_wheelAngSpeed[w] = 0;
			}
			this->_calib_wheel_table[w][VOLTS][i] = this->_vbat * (float)URL / 255.0;
			this->_calib_wheel_table[w][SPEED][i] = this->_wheelAngSpeed[w];
			if (i < (WHEEL_TABLE_SIZE - 2)) URL += cstep;
			else URL = 255;
		}
		analogWrite(wheel_PWM_pins[w], 0);
	}

	this->_eepromWriteWheelTable((float*)this->_calib_wheel_table);

	return;
}

float Chassis::getVoltage()
{
	byte n = 10;
	float v = 0;
	for (byte i = 0; i < n; i++)
	{
		v = v + analogRead(PIN_VBAT_1S);
	}
	v = 2 * 5.0 * ((v / n) / 1023.0);

	return v;
}

//PRIVATE

int Chassis::_eepromWriteWheelTable(float *table)
{
	unsigned int N = 2 + WHEEL_COUNT*WHEEL_TABLE_SIZE*(2 * sizeof(float));
	unsigned int addr = EEPROM_WHEEL_TABLE_ADDR;
	byte *p = (byte*)table;

	EEPROM.write(addr, 'w');
	addr++;
	//EEPROM.write(addr, N);
	addr++;

	for (unsigned int i = 0; i < N; i++)
	{
		EEPROM.write(addr + i, *(p + i));
	}
	return 0;
}

int Chassis::_eepromReadWheelTable(float *table)
{
	unsigned int N = 2 + WHEEL_COUNT*WHEEL_TABLE_SIZE*(2 * sizeof(float));
	unsigned int addr = EEPROM_WHEEL_TABLE_ADDR;
	byte *p = (byte*)table;
	byte d = 0;

	d = EEPROM.read(addr);
	if (d != 'w') return -1;
	addr++;
	addr++;

	for (unsigned int i = 0; i < N; i++)
	{
		*(p + i) = EEPROM.read(addr + i);
	}

	return 0;
}

int Chassis::_wheelGetU(float ang_speed, int wheel, float volts)
{
	unsigned int U = 0;
	float v = 0;
	byte i = 0;

	if (volts == 0) return -3;

	if ((ang_speed < this->_calib_wheel_table[wheel][SPEED][0]) || (ang_speed > this->_calib_wheel_table[wheel][SPEED][WHEEL_TABLE_SIZE - 1])) return -1;

	for (i = 0; i < WHEEL_TABLE_SIZE; i++)
	{
		if (this->_calib_wheel_table[wheel][SPEED][i] >= ang_speed) break;
	}

	//(v-v1)/(v2-v1) = (s-s1)/(s2-s1) - ��������� ������ ����� ��� �����, s - SPEED (����/���), v - VOLTS (�)
	//������ v = ((s-s1)*(v2-v1)/(s2-s1)) + v1

	v = this->_calib_wheel_table[wheel][VOLTS][i - 1] + ((ang_speed - this->_calib_wheel_table[wheel][SPEED][i - 1])*(this->_calib_wheel_table[wheel][VOLTS][i] - this->_calib_wheel_table[wheel][VOLTS][i - 1]) / (this->_calib_wheel_table[wheel][SPEED][i] - this->_calib_wheel_table[wheel][SPEED][i - 1]));
	U = (255 * v) / volts;

	if (U > 255) U = 255;

	return U;
}

void Chassis::_wheel_rotate(byte wheel)
{
	unsigned long wheelTime = 0;

	while (1)
	{
		if (dist_wheelTachomCount[wheel] <= _wheelTachomCount[wheel]) break; //����� �� ����� ��� ���������� ������������ ����� ������� ������� - ������� ��������
		
		if (_syncloop[wheel])
		{
			_syncloop[wheel] = false;
			wheelTime = 0;
			byte k = 0;
			for (byte i = 0; i < MEAN_DEPTH; i++)
			{
				if ((_wheelTimeArray[wheel][i] <= wheel_time_max[wheel]) && (_wheelTimeArray[wheel][i] >= wheel_time_min[wheel]))
				{
					wheelTime = wheelTime + _wheelTimeArray[wheel][i];
					k++;
				}
			}

			this->_wheelAngSpeed[wheel] = 0;
			if (k > 0)
			{
				wheelTime = wheelTime / k;
				this->_wheelAngSpeed[wheel] = (360000000.0 * (float)_wheelDir[wheel]) / (WHEEL_SEGMENTS *  wheelTime);
			}

			Error[wheel] = (this->_wheelSetAngSpeed[wheel] - this->_wheelAngSpeed[wheel]);
			//if ((Error[wheel] < 3) && (Error[wheel] > -3)) Error[wheel] = 0;
			U[wheel] = U[wheel] + DEFAULT_PID_Kp * Error[wheel];
			U[wheel] = U[wheel] + DEFAULT_PID_Kd * (Error[wheel] - Error1[wheel]);
			Error1[wheel] = Error[wheel];
			I[wheel] = I[wheel] + DEFAULT_PID_Ki*Error[wheel];
			U[wheel] = U[wheel] + I[wheel];
		}

		if (U[wheel] > 255) U[wheel] = 255;
		if (U[wheel] < -255) U[wheel] = -255;
		U0[wheel] = abs((int)U[wheel]);
		if (dir[wheel])
		{
			U0[wheel] = 255 - U0[wheel];
		}

		analogWrite(wheel_PWM_pins[wheel], U0[wheel]);
	}

	digitalWrite(wheel_DIR_pins[wheel], !dir[wheel]);
	digitalWrite(wheel_PWM_pins[wheel], dir[wheel]);
	
	delay(25);//�������� ��� �������� �������� - ������ �� ��� ����� �� ��������� �������� �������� ����������
	digitalWrite(wheel_DIR_pins[wheel], dir[wheel]);

	dist_wheelTachomCount[wheel] = _wheelTachomCount[wheel];
	for (byte i = 0; i < MEAN_DEPTH; i++)
	{
		_wheelTimeArray[wheel][i] = 0;
	}
	this->_wheelAngSpeed[wheel] = 0;
	this->_wheelSetAngSpeed[wheel] = 0;
}

void Chassis::_wheel_rotate_sync()
{
	unsigned long wheelTime = 0;

	while (1)
	{
		byte w;
		for (w = 0; w < WHEEL_COUNT; w++)
		{
			if (dist_wheelTachomCount[w] <= _wheelTachomCount[w]) break; //����� �� ����� ��� ���������� ������������ ����� ������� ������� - ������� ��������
		}
		if (w < WHEEL_COUNT) break; //��� break ��� ���� ������ ������ �� ����� for� � ���� �������� �� ������� �� ����� ����� while
		//��� ����, ����� �� ����� ���������� ����� ���� �� ���� �� ����� �������� �������.

		for (w = 0; w < WHEEL_COUNT; w++)
		{
			if (_syncloop[w])
			{
				_syncloop[w] = false;
				wheelTime = 0;
				byte k = 0;
				for (byte i = 0; i < MEAN_DEPTH; i++)
				{
					if ((_wheelTimeArray[w][i] <= wheel_time_max[w]) && (_wheelTimeArray[w][i] >= wheel_time_min[w]))
					{
						wheelTime = wheelTime + _wheelTimeArray[w][i];
						k++;
					}
				}

				this->_wheelAngSpeed[w] = 0;
				if (k > 0)
				{
					wheelTime = wheelTime / k;
					this->_wheelAngSpeed[w] = (360000000.0 * (float)_wheelDir[w]) / (WHEEL_SEGMENTS *  wheelTime);
				}

				Error[w] = (this->_wheelSetAngSpeed[w] - this->_wheelAngSpeed[w]);
				//if ((Error[w] < 3) && (Error[w] > -3)) Error[w] = 0;
				U[w] = U[w] + DEFAULT_PID_Kp * Error[w];
				U[w] = U[w] + DEFAULT_PID_Kd * (Error[w] - Error1[w]);
				Error1[w] = Error[w];
				I[w] = I[w] + DEFAULT_PID_Ki*Error[w];
				U[w] = U[w] + I[w];

				if (U[w] > 255) U[w] = 255;
				if (U[w] < -255) U[w] = -255;
				U0[w] = abs((int)U[w]);
				if (dir[w])
				{
					U0[w] = 255 - U0[w];
				}
			}
		}

		for (byte w = 0; w < WHEEL_COUNT; w++)
		{
			analogWrite(wheel_PWM_pins[w], U0[w]);
		}
	}

	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		digitalWrite(wheel_DIR_pins[w], !dir[w]);
		digitalWrite(wheel_PWM_pins[w], dir[w]);
	}
	delay(25);//�������� ��� �������� �������� - ������ �� ��� ����� �� ��������� �������� �������� ����������
	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		digitalWrite(wheel_DIR_pins[w], dir[w]);
	}

	for (byte w = 0; w < WHEEL_COUNT; w++)
	{
		dist_wheelTachomCount[w] = _wheelTachomCount[w];
		for (byte i = 0; i < MEAN_DEPTH; i++)
		{
			_wheelTimeArray[w][i] = 0;
		}
		this->_wheelAngSpeed[w] = 0;
		this->_wheelSetAngSpeed[w] = 0;
	}
}