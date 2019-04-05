#ifndef misp_h
#define misp_h

#include "Protocol.h"

#define MiSP_GET_IDENT 100 //������ ������ �������� (->MIRO)
#define MiSP_IDENT 101 //�������� ������ �������� (MIRO->)
#define MiSP_MOVE 111 //������� �� �������� ������ (->MIRO)
#define MiSP_ROTATE 112 //������� �� ������� ������ (->MIRO)
#define MiSP_GET_SPEED 113 //������� �� ��������� ������ � �������� � ������� �������� ������ (->MIRO)
#define MiSP_SPEED 114 //�������� ������ � �������� � ������� �������� ������ (MIRO->)
#define MiSP_GET_PATH 115 //������� �� ��������� ����� ����������� ���� (->MIRO)
#define MiSP_PATH 116 //�������� ����� ����������� ���� (MIRO->)

#define MiSP_LED_ON	211 //������� �� ��������� ���������� (->MIRO)

#define MiSP_GET_US_DIST 212 //������ ����������, ����������� ��-����������� (->MIRO)
#define MiSP_US_DIST 213 //�������� ����������, ���������� ��-����������� (MIRO->)

#define MiSP_GET_LINE_SENSE 214 //������ �������� � �������� ����� (->MIRO)
#define MiSP_LINE_SENSE 215 //�������� �������� � ������� ����� (MIRO->)

#define MiSP_GET_LIGHT_SENSE 216 //������ �������� � ������������� (->MIRO)
#define MiSP_LIGHT_SENSE 217 //�������� �������� � ������������� (MIRO->)

#define MiSP_BUZZER_TONE 218 //������ �� ������������ ����� (->MIRO)

//��, ��� ������ ���� �� �����������, �� ��, ��� ���� ����������� �����������

//#define MiSP_BUZZER_SONG //������ �� ������������ ������� (->MIRO)
//#define MiSP_PMLIDAR //������ ������� ������ � �� ���������� �� ������������
//#define MiSP_CAM_ANGLE //��������� ���� ������������ ������

/*������� ������������ �������*/
int _misp_get_ident(struct tPacket *packet);
int _misp_ident(struct tPacket *packet, unsigned long ident);
int _misp_move(struct tPacket *packet, float lin_speed, float ang_speed, float distance);//���� distance == 0, �� ��� �������� ����������
int _misp_rotate(struct tPacket *packet, float ang_speed, float ang);//���� ang == 0, �� ��� �������� ����������
int _misp_get_speed(struct tPacket *packet);
int _misp_speed(struct tPacket *packet, float lin_speed, float ang_speed);
int _misp_get_path(struct tPacket *packet);
int _misp_path(struct tPacket *packet, float path);

int _misp_led_on(struct tPacket *packet, unsigned char led, unsigned char value);
int _misp_get_us_dist(struct tPacket *packet, unsigned char sensor);
int _misp_us_dist(struct tPacket *packet, unsigned char sensor, float value);
int _misp_get_line_sense(struct tPacket *packet, unsigned char sensor);
int _misp_line_sense(struct tPacket *packet, unsigned char sensor, unsigned char value);
int _misp_get_light_sense(struct tPacket *packet, unsigned char sensor);
int _misp_light_sense(struct tPacket *packet, unsigned char sensor, float value);
int _misp_buzzer_tone(struct tPacket *packet, unsigned char buzzer, unsigned int frequency, unsigned long duration);

/*������� ���������� ������ �� �������*/
unsigned char _rx_get_cmd(struct tPacket *packet);
float _rx_get_1f32(struct tPacket *packet); //������� ������ �������� ���� float ����� ���� ������
unsigned char _rx_get_1b8(struct tPacket *packet);
unsigned char _rx_get_2b8(struct tPacket *packet);
float _rx_get_2f32(struct tPacket *packet);
float _rx_get_5f32(struct tPacket *packet);
float _rx_get_9f32(struct tPacket *packet);
unsigned int _rx_get_2u16(struct tPacket *packet);
unsigned int _rx_get_4u16(struct tPacket *packet);

#define _rx_get_ident _rx_get_1u32
#define _rx_get_path _rx_get_1f32
#define _rx_get_lin_speed _rx_get_1f32
#define _rx_get_ang_speed _rx_get_5f32
#define _rx_get_dist _rx_get_9f32
#define _rx_get_ang _rx_get_5f32
#define _rx_get_buzzer _rx_get_1b8
#define _rx_get_buzzer_tone_freq _rx_get_2u16
#define _rx_get_buzzer_tone_dur _rx_get_4u16
#define _rx_get_linesense _rx_get_1b8
#define _rx_get_linesense_value _rx_get_2b8
#define _rx_get_lightsense _rx_get_1b8
#define _rx_get_lightsense_value _rx_get_2f32
#define _rx_get_led _rx_get_1b8
#define _rx_get_led_value _rx_get_2b8

#endif