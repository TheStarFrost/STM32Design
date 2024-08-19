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

int main(void)
{
	uint8_t sensor_Value[4];
	int16_t duty = 1000, flag = 0;
	int16_t duty1 = 0, duty2 = 0, duty3 = 0;
	char txt[64];

	//-----------------------系统初始化配置----------------------------
	HAL_Init();			  // 初始化HAL库
	SystemClock_Config(); // 设置时钟9倍频,72M
	delay_init(72);		  // 初始化延时函数
	JTAG_Set(SWD_ENABLE); // 打开SWD接口 可以利用主板的SWD接口调试
	//-----------------------------------------------------------------

	LED_Init();			// LED初始化
	OLED_Init();		// OLED初始化
	OLED_Show_LQLogo(); // 显示LOGO
	delay_ms(500);		// 延时等待
	OLED_CLS();			// 清屏
	MotorInit();
	OLED_P8x16Str(20, 0, "Project Test");

	while (1)
	{
		// KEY->Moter 逻辑判断
		if (Read_key(KEY1) == 1) // 按下KEY0键 停止
		{
			if (duty > -PWM_DUTY_MAX)
			{
				duty1 = 0;
				duty2 = 0;
				duty3 = 0;
			}
		}
		if (Read_key(KEY2) == 1) // 按下KEY2键 原地旋转
		{
			if (duty < PWM_DUTY_MAX) // 满占空比为10000，限制7200
			{
				duty1 = 1000;
				duty2 = 1000;
				duty3 = 1000;
			}
		}
		if (Read_key(KEY3) == 1) // 按下KEY3键 右转
		{
			if (duty < PWM_DUTY_MAX) // 满占空比为10000，限制7200
			{
				duty1 = 0;
				duty2 = -1000;
				duty3 = 1900;
			}
		}
		if (Read_key(KEY4) == 1) // 按下KEY4键 前进
		{
			duty1 = 0;
			duty2 = -1000;
			duty3 = 900;
		}
		// Moter 控制
		MotorCtrl3w(duty1, duty2, duty3);
		sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
		OLED_P6x8Str(10, 5, txt);

		// 光电传感器 黑1白0
		sensor_Value[0] = Read_sensor(sensor1);
		sensor_Value[1] = Read_sensor(sensor2);
		sensor_Value[2] = Read_sensor(sensor3);
		sensor_Value[3] = Read_sensor(sensor4);
		sprintf(txt, "%d %d %d %d", sensor_Value[0], sensor_Value[1], sensor_Value[2], sensor_Value[3]);
		OLED_P6x8Str(0, 2, txt); // 字符串
		LED_Ctrl(RVS);

		// 左右转逻辑判断
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
