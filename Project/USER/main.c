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

int main(void)
{
	uint8_t sensor_Value[4];
	int16_t duty = 1000, flag = 0;
	int16_t duty1 = 0, duty2 = 0, duty3 = 0;
	char txt[64];

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
		// KEY->Moter �߼��ж�
		if (Read_key(KEY1) == 1) // ����KEY0�� ֹͣ
		{
			if (duty > -PWM_DUTY_MAX)
			{
				duty1 = 0;
				duty2 = 0;
				duty3 = 0;
			}
		}
		if (Read_key(KEY2) == 1) // ����KEY2�� ԭ����ת
		{
			if (duty < PWM_DUTY_MAX) // ��ռ�ձ�Ϊ10000������7200
			{
				duty1 = 1000;
				duty2 = 1000;
				duty3 = 1000;
			}
		}
		if (Read_key(KEY3) == 1) // ����KEY3�� ��ת
		{
			if (duty < PWM_DUTY_MAX) // ��ռ�ձ�Ϊ10000������7200
			{
				duty1 = 0;
				duty2 = -1000;
				duty3 = 1900;
			}
		}
		if (Read_key(KEY4) == 1) // ����KEY4�� ǰ��
		{
			duty1 = 0;
			duty2 = -1000;
			duty3 = 900;
		}
		// Moter ����
		MotorCtrl3w(duty1, duty2, duty3);
		sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
		OLED_P6x8Str(10, 5, txt);

		// ��紫���� ��1��0
		sensor_Value[0] = Read_sensor(sensor1);
		sensor_Value[1] = Read_sensor(sensor2);
		sensor_Value[2] = Read_sensor(sensor3);
		sensor_Value[3] = Read_sensor(sensor4);
		sprintf(txt, "%d %d %d %d", sensor_Value[0], sensor_Value[1], sensor_Value[2], sensor_Value[3]);
		OLED_P6x8Str(0, 2, txt); // �ַ���
		LED_Ctrl(RVS);

		// ����ת�߼��ж�
		uint8_t TurnRight[] = {1, 0, 0, 0};
		if (sensor_Value[0] == 1 && sensor_Value[1] == 1 && sensor_Value[2] == 0 && sensor_Value[3] == 0)
		{
			duty1 = 0;
			duty2 = -1000;
			duty3 = 900;
		}
		delay_ms(50);
	}
}
