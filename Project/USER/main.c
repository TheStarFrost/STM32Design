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
    int16_t duty = 1000,flag = 0;
    char txt[16];
		int duty3;
		int duty2;
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
	OLED_P8x16Str(20,0,"Test Motor");
    
  while (1)
  {
		// guang dian tan ce
		sensor_Value[0] = Read_sensor(sensor1);
		sensor_Value[1] = Read_sensor(sensor2);
		sensor_Value[2] = Read_sensor(sensor3);
		sensor_Value[3] = Read_sensor(sensor4);
		
    if(Read_key(KEY1) == 1)      //����KEY0����ռ�ձȼ�С
    {
      //if(duty > -PWM_DUTY_MAX)
        duty = 0;
				duty2 = duty;
			  duty3 = duty;
    }
    if(Read_key(KEY2) == 1)      //����KEY2����ռ�ձȼӴ�
    {
      //if(duty < PWM_DUTY_MAX)   //��ռ�ձ�Ϊ10000������7200
        duty = 0;
			  duty2 = -2000;
			  duty3 = 900;
    }
    if(Read_key(KEY3) == 1)      //����KEY1����ռ�ձ���ֵ
    {
      duty = 0;
			duty2 = -1000;
			duty3 = 1900;
    }
		if(Read_key(KEY4) == 1)      //����KEY1����ռ�ձ���ֵ
    {
      duty = 0;
			duty2 = -1000;
			duty3 = 900;
    }
    
//    /*  �Զ��Ӽ��ٲ��� */   //240814
//    if(duty >= 3500)//6000
//        flag = 1;
//    if(duty <= -3500)
//        flag = 0;
//    
//    if(flag == 0)
//        duty += 100;
//    else
//        duty -= 100;
    MotorCtrl3w(duty,duty2, duty3);
		sprintf(txt, "PWM: %5d,%5d,%5d", duty, duty2, duty3);
    //sprintf(txt, "PWM: %5d;", duty);
	  OLED_P8x16Str(10,5,txt);
		sprintf(txt, "%d %d %d %d", sensor_Value[0], sensor_Value[1], sensor_Value[2], sensor_Value[3]);
		OLED_P6x8Str(0, 2, txt); // �ַ���
    
    LED_Ctrl(RVS);       //��ƽ��ת,LED��˸
    delay_ms(100);       //��ʱ�ȴ�
  }
}

