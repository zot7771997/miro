#ifndef protocol_h
#define protocol_h

#define TO_HOST '>'
#define FROM_HOST '<'

#define MAX_PAYLOAD 17 //������ �������� ��������, ����
#define PROTOCOL_MAX_PACKET (MAX_PAYLOAD + 5)

struct tPacket {
	char _header[2] = { '$', 'M' }; //���������
	char _dir = FROM_HOST; //���� ����������� ������: ">" - � ���� �� ������, "<" - � ������ �� �����
	unsigned char _size; //������� ������ payload
	unsigned char _payload[MAX_PAYLOAD]; //������ payload (�������� � ���� cmd � data, ����� ���� �� 1 (������ ��� ������) �� 17 ���� (1 ���� ���� ������ + 16 ���� ������))
	unsigned char _crc; //���� ����� �� ������ 2 ������ ����� _size � �������� ����� payload
};

int _protocol_crc_calc(struct tPacket *packet);

bool _protocol_crc_check(struct tPacket *packet);

int _protocol_p2d(struct tPacket *packet, unsigned char *data);

int _protocol_d2p(struct tPacket *packet, unsigned char *data);

#endif