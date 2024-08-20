/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
@编    写：龙邱科技
@E-mail  ：chiusir@163.com
@编译IDE ：KEIL5.25.3及以上版本
@使用平台：北京龙邱智能科技全向福来轮小车
@最后更新：2022年02月19日，持续更新，请关注最新版！
@功能介绍：
@相关信息参考下列地址
@网    站：http://www.lqist.cn
@淘宝店铺：http://longqiu.taobao.com
@软件版本：V1.0 版权所有，单位使用请先联系授权
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

// 包含所有头文件
#include "include.h"
#include "LQ_Photoelectric.h"

void straight(int duty1, int duty2, int duty3, char *txt) // the car go staight
{
	if (duty1 < PWM_DUTY_MAX) // ?????10000,??7200
	{
		duty1 = 0;
		duty2 = -1500;
		duty3 = 1400;
		MotorCtrl3w(duty1, duty2, duty3);
		// sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
		// OLED_P6x8Str(10, 5, txt);
	}
}

void left(int duty1, int duty2, int duty3, char *txt) // the car turn left
{
	if (duty1 < PWM_DUTY_MAX) // ?????10000,??7200
	{
		duty1 = 0;
		duty2 = -1400;
		duty3 = 400;
		MotorCtrl3w(duty1, duty2, duty3);
		// sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
		// OLED_P6x8Str(10, 5, txt);
	}
}

void right(int duty1, int duty2, int duty3, char *txt) // the car turn right
{
	if (duty1 < PWM_DUTY_MAX) // ?????10000,??7200
	{
		duty1 = 0;
		duty2 = -500;
		duty3 = 1300;
		MotorCtrl3w(duty1, duty2, duty3);
		// sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
		// OLED_P6x8Str(10, 5, txt);
	}
}

void turn_zero(int duty1, int duty2, int duty3, char *txt) // speed turn zero
{
	if (duty1 > -PWM_DUTY_MAX) // ?????10000,??7200
	{
		duty1 = 0;
		duty2 = 0;
		duty3 = 0;
		MotorCtrl3w(duty1, duty2, duty3);
		// sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
		// OLED_P6x8Str(10, 5, txt);
	}
}

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

// 转弯判断函数 判断是否向传入方向转弯
uint8_t is_turn_single(uint8_t Turn[4], uint8_t sensor_Value[4])
{
	return (sensor_Value[0] == Turn[0] &&
			sensor_Value[1] == Turn[1] &&
			sensor_Value[2] == Turn[2] &&
			sensor_Value[3] == Turn[3]);
};

// 转弯判断函数 判断左转0/直行1/右转2
uint8_t is_turn(uint8_t Left[][4], uint8_t Right[][4], uint8_t Straight[][4], uint8_t sensor[4])
{
	int i = 0;
	// 左转检查
	for (i = 0; i < 4; i++)
	{
		if (is_turn_single(Left[i], sensor))
		{
			return 0;
		}
	}
	// 直行检查
	for (i = 0; i < 2; i++)
	{
		if (is_turn_single(Straight[i], sensor))
		{
			return 1;
		}
	}
	// 右转检查
	for (i = 0; i < 3; i++)
	{
		if (is_turn_single(Right[i], sensor))
		{
			return 2;
		}
	}
	// 错误
	return 3;
};

int main(void)
{
	int ECPULSE1 = 0, ECPULSE2 = 0, ECPULSE3 = 0;
	uint8_t sensor_Value[4];
	int16_t duty = 1000, flag = 0;
	int16_t duty1 = 0, duty2 = 0, duty3 = 0;
	char txt[64];
	uint8_t turn;
	//-----------------------系统初始化配置----------------------------
	HAL_Init();			  // 初始化HAL库
	SystemClock_Config(); // 设置时钟9倍频,72M
	delay_init(72);		  // 初始化延时函数
	JTAG_Set(SWD_ENABLE); // 打开SWD接口 可以利用主板的SWD接口调试
	//-----------------------------------------------------------------
	Encoder_Init_TIM2(); //?????????????2,3,4?????AB??????
	Encoder_Init_TIM3();
	Encoder_Init_TIM4();
	LED_Init();			// LED初始化
	OLED_Init();		// OLED初始化
	OLED_Show_LQLogo(); // 显示LOGO
	delay_ms(500);		// 延时等待
	OLED_CLS();			// 清屏
	MotorInit();
	OLED_P8x16Str(20, 0, "Project Test");

	while (1)
	{
		// 光电传感器 黑1白0
		sensor_Value[0] = Read_sensor(sensor1);
		sensor_Value[1] = Read_sensor(sensor2);
		sensor_Value[2] = Read_sensor(sensor3);
		sensor_Value[3] = Read_sensor(sensor4);
		sprintf(txt, "%d %d %d %d", sensor_Value[0], sensor_Value[1], sensor_Value[2], sensor_Value[3]);
		OLED_P6x8Str(0, 2, txt); // 字符串
		LED_Ctrl(RVS);

		// 左右转逻辑判断
		turn = is_turn(TurnLeft, TurnRight, GoStraight, sensor_Value);
		switch (turn)
		{
		case 0:
			OLED_P6x8Str(0, 3, "Left!");
			left(duty1, duty2, duty3, txt);
			break;
		case 1:
			OLED_P6x8Str(0, 3, "Line!");
			straight(duty1, duty2, duty3, txt);
			// delay_ms(500);
			break;
		case 2:
			OLED_P6x8Str(0, 3, "Right");
			right(duty1, duty2, duty3, txt);
			break;
		default:
			OLED_P6x8Str(0, 3, "Error");
			straight(duty1, duty2, duty3, txt);
			break;
		}
		ECPULSE1 = Read_Encoder(2);
		sprintf(txt, "E1:%04d ", ECPULSE1);
		OLED_P6x8Str(0, 4, txt);
		ECPULSE2 = Read_Encoder(3);
		sprintf(txt, "E2:%04d ", ECPULSE2);
		OLED_P6x8Str(0, 5, txt);
		ECPULSE3 = Read_Encoder(4);
		sprintf(txt, "E3:%04d ", ECPULSE3);
		OLED_P6x8Str(0, 6, txt);
	}
}
