#define TO_HOST '>'
#define FROM_HOST '<'

#define MAX_PAYLOAD 16 //����

#define MiSP_IDENT 100 //������ ������ ��������
#define MiSP_MOVE 111 //������� �� �������� ������
#define MiSP_ROTATE 112 //������� �� ������� ������
#define MiSP_GET_SPEED 113 //������� �� ��������� ������ � �������� � ������� �������� ������
#define MiSP_GET_PATH 114 //������� �� ��������� ����� ����������� ����

#define MiSP_LED_ON	211 //������� �� ��������� ����������
#define MiSP_LED_OFF 212 //������� �� ���������� ����������

#define MiSP_US_DIST 213 //������ ����������, ����������� ��-�����������
#define MiSP_LINE_SENSE 214 //������ �������� � ������� �����
#define MiSP_LIGHT_SENSE 215 //������ �������� � �������������
#define MiSP_BUZZER 216 //������ �� ������������ �����/�������

//��, ��� ������ ���� �� �����������, �� ��, ��� ���� ����������� �����������

//#define MiSP_PMLIDAR //������ ������� ������ � �� ���������� � ������������
//#define MiSP_CAM_ANGLE //��������� ���� ������������ ������

struct tPacket {
	byte _header[2] = { '$', 'M' };//���������
	byte _dir = FROM_HOST;//���� ���������� ������
	unsigned int _size; //������� ������ payload
	byte _payload[MAX_PAYLOAD];//������ payload (�������� � ���� cmd � data)
	byte _crc;//���� ����� �� ������ 2 ������ ����� _size � �������� ����� payload
};

/*
class Packet {
public:
	byte packet[5 + MAX_PAYLOAD];

	unsigned int length();
	unsigned int size();
	void set_dir(char direction);

private:
	byte _header[2] = { '$', 'M' };
	byte _dir = FROM_HOST;
	unsigned int _size;
	byte _payload[MAX_PAYLOAD];
	byte _crc;
};
*/

//���������� ������ ������ - ��������� �� ���� '$', � ������ �������� �������� (��� ���������� � ����� *(packet + 3))
int _crc_calc(struct tPacket *packet)
{
	if (packet == nullptr) return -1;
	byte crc = *(packet + 3);
	for (int i = 1; i < packet->_size; i++)
	{
		crc ^= *(packet + 3 + i);
	}

	packet->_crc = crc;

	return 0;
}

int _misp_ident(struct tPacket *packet)
{
	if (packet == nullptr) return -1;
	packet->_payload[0] = MiSP_IDENT;
	return 0;
}

int _misp_move(struct tPacket *packet, float lin_speed, float ang_speed, float distance, bool wait)
{
	if (packet == nullptr) return -1;
	packet->_payload[0] = MiSP_MOVE;
	*((float*)(packet->_payload + 1)) = lin_speed;
	*((float*)(packet->_payload + 1 + sizeof(float)) = ang_speed;
	*((float*)(packet->_payload + 1 + 2*sizeof(float)) = distance;
	*((bool*)(packet->_payload + 1 + 3*sizeof(float)) = wait;

	packet->_size = 3 * sizeof(float) + sizeof(bool) + 1;

	return _crc_calc(packet);
}

int _misp_tx(struct tPacket *packet, byte *data)
{
	if (packet == nullptr) || (data == nullptr) return -1;
	*data = '$';
	*(data + 1) = 'M';
	*(data + 2) = packet->_dir;
	*((unsigned int*)(data + 3)) = packet->_size;
	for (int i = 0; i < packet->_size; i++)
	{
		*(data + 5 + i) = packet->_payload[i];
	}

	*(data + 5 + packet->_size) = packet->_crc;

	return 0;
}

int _misp_rx(struct tPacket *packet, byte *data)
{
	if (packet == nullptr) || (data == nullptr) return -1;

	if ((*data != '$') || (*(data + 1) != 'M')) return -2;

	packet->_header[0] = '$';
	packet->_header[1] = 'M';

	packet->_dir = *(data + 2);
	packet->_size = *((unsigned int*)(data + 3));
	if (packet->_size > MAX_PAYLOAD) return -3;

	for (int i = 0; i < packet->_size; i++)
	{
		packet->_payload[i] = *(data + 5 + i);
	}
	
	_crc_calc(packet);

	if (packet->_crc != *(data + 5 + packet->_size)) return -4;

	return 0;
}

void misp_ident(Packet *packet);
void misp_move(float lin_speed, float ang_speed, float distance, bool wait);
void misp_rotate(float ang, float ang_speed, float wait);
void misp_get_speed();
void misp_get_path();
void misp_led_on(byte value);
void misp_led_off();