/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
@��    д������Ƽ�
@E-mail  ��chiusir@163.com
@����IDE ��KEIL5.25.3�����ϰ汾
@ʹ��ƽ̨�������������ܿƼ�ȫ������С��
@�����£�2022��02��19�գ��������£����ע���°棡
@���ܽ��ܣ�
@�����Ϣ�ο����е�ַ
@��    վ��http://www.lqist.cn
@�Ա����̣�http://longqiu.taobao.com
@����汾��V1.0 ��Ȩ���У���λʹ��������ϵ��Ȩ
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

// ��������ͷ�ļ�
#include "include.h"
#include "LQ_Photoelectric.h"

uint8_t TurnLeft[][4] = {
	{1, 1, 1, 0}, // 1110
	{1, 1, 0, 0}, // 1100
	{1, 0, 0, 0}, // 1000
	{0, 0, 0, 0}  // 0000

};
uint8_t TurnRight[][4] = {
	{0, 0, 0, 1}, // 0001
	{0, 0, 1, 1}, // 0011
	{0, 1, 1, 1}  // 0111
};
uint8_t GoStraight[][4] = {
	{1, 1, 1, 1}, // 1111
	{0, 1, 1, 0}  // 0110
};

// ת���жϺ��� �ж��Ƿ����뷽��ת��
uint8_t is_turn_single(uint8_t Turn[4], uint8_t sensor_Value[4])
{
	return (sensor_Value[0] == Turn[0] &&
			sensor_Value[1] == Turn[1] &&
			sensor_Value[2] == Turn[2] &&
			sensor_Value[3] == Turn[3]);
};

// ת���жϺ��� �ж���ת0/ֱ��1/��ת2
uint8_t is_turn(uint8_t Left[][4], uint8_t Right[][4], uint8_t Straight[][4], uint8_t sensor[4])
{
	int i = 0;
	// ��ת���
	for (i = 0; i < sizeof(Left) / sizeof(Left[0]); i++)
	{
		if (is_turn_single(Left[i], sensor))
		{
			return 0;
		}
	}
	// ֱ�м��
	for (i = 0; i < sizeof(Straight) / sizeof(Straight[0]); i++)
	{
		if (is_turn_single(Straight[i], sensor))
		{
			return 1;
		}
	}
	// ��ת���
	for (i = 0; i < sizeof(Right) / sizeof(Right[0]); i++)
	{
		if (is_turn_single(Right[i], sensor))
		{
			return 2;
		}
	}
	// ����
	return 3;
};

int main(void)
{
	uint8_t sensor_Value[4];
	int16_t duty = 1000, flag = 0;
	int16_t duty1 = 0, duty2 = 0, duty3 = 0;
	char txt[64];
	uint8_t turn;
	//-----------------------ϵͳ��ʼ������----------------------------
	HAL_Init();			  // ��ʼ��HAL��
	SystemClock_Config(); // ����ʱ��9��Ƶ,72M
	delay_init(72);		  // ��ʼ����ʱ����
	JTAG_Set(SWD_ENABLE); // ��SWD�ӿ� �������������SWD�ӿڵ���
	//-----------------------------------------------------------------

	LED_Init();			// LED��ʼ��
	OLED_Init();		// OLED��ʼ��
	OLED_Show_LQLogo(); // ��ʾLOGO
	delay_ms(500);		// ��ʱ�ȴ�
	OLED_CLS();			// ����
	MotorInit();
	OLED_P8x16Str(20, 0, "Project Test");

	while (1)
	{
		// ��紫���� ��1��0
		sensor_Value[0] = Read_sensor(sensor1);
		sensor_Value[1] = Read_sensor(sensor2);
		sensor_Value[2] = Read_sensor(sensor3);
		sensor_Value[3] = Read_sensor(sensor4);
		sprintf(txt, "%d %d %d %d", sensor_Value[0], sensor_Value[1], sensor_Value[2], sensor_Value[3]);
		OLED_P6x8Str(0, 2, txt); // �ַ���
		LED_Ctrl(RVS);

		// ����ת�߼��ж�
		turn = is_turn(TurnLeft, TurnRight, GoStraight, sensor_Value);
		switch (turn)
		{
		case 0:
			OLED_P6x8Str(0, 3, "Turn Left");
			break;
		case 1:
			OLED_P6x8Str(0, 3, "Go Straight");
			break;
		case 2:
			OLED_P6x8Str(0, 3, "Turn Right");
			break;
		default:
			OLED_P6x8Str(0, 3, "Error");
			break;
		}
		delay_ms(50);
	}
}
