/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
@??    ?????????
@E-mail  ??chiusir@163.com
@????IDE ??KEIL5.25.3???????
@'??????????????????????????????
@?????�?2022??02??19??????????�????????�?
@????????
@???????????????
@??    ???http://www.lqist.cn
@????????http://longqiu.taobao.com
@???????V1.0 ???????????'????????????
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

// ??????????l?
#include "include.h"

double x_error = 0;
char txt[32];
int duty1;
int duty2;
int duty3;
int duty = 1000;
double k;
int k1;

int kick = 0;

int main(void)
{
	//-----------------------????'??????----------------------------
	HAL_Init();			  // ??'??HAL??
	SystemClock_Config(); // ???????9???,72M
	delay_init(72);		  // ??'?????????
	JTAG_Set(SWD_ENABLE); // ??SWD??? ?????????????SWD??????

	//-----------------------------------------------------------------
	uart_init(USART_3, 115200); //??'??????    ????LQMV4???g??
	uart_init(USART_2, 115200); //??'??????    ???????
	OLED_Init();
	LED_Init(); //??'??LED
	MotorInit();
	delay_ms(500); // ????
	printf("Long Qiu Ke Ji\n");
	printf("USART3 test,This is USART2.\n");

	while (1)
	{
		uart_SendBuf(&USART3_Handler, (uint8_t *)"This is USART3!\n");
		OLED_P8x16Str(10, 0, "UART Test"); //?????

		k1 = 1;
		while (k1 == 1)
		{
			// MotorCtrl3w(1200, 0, 0);
			// LED_Ctrl(RVS);
			if (receive_oneline3 != 0)
			{
				k = x_error;
				uart_SendBuf(&USART3_Handler, (uint8_t *)ReadBuff);
				x_error = atof((char *)ReadBuff);
				sprintf(txt, "%f", x_error);
				OLED_P8x16Str(0, 4, txt); //??????????????????OLED?
				LED_Ctrl(RVS);
				// printf("R:%s\n",ReadBuff);  //????2????????????

				//
				x_error = atof((char *)ReadBuff);

				if (x_error < 1000)
				{
					if (k > 1000)
					{
						MotorCtrl3w(-600, -600, -600);
						delayms(100);
					}
					if (k == 100 && x_error != 100)
					{
						MotorCtrl3w(-600, 0, 0);
						delayms(100);
					}
					if (k == 200 && x_error != 200)
					{
						MotorCtrl3w(2000, 2000, 2000);
						// break;
						delayms(100);
						x_error = 10000;
					}
					if (k <= 1 && k >= -1 && x_error == 100)
					{
						MotorCtrl3w(0, 0, 0);
						delayms(100);
					}
					if (x_error < -5) // jin qiu
					{
						duty1 = 0;
						duty2 = 0;
						duty3 = 0;
						MotorCtrl3w(duty1, duty2, duty3);
					}

					else // no jin qiu
					{
						if (x_error >= 0 && x_error <= 1) // chao hong qiu zou
						{
							duty1 = 800 * x_error + 700;
							duty2 = -1050 + 1100 * x_error;
							duty3 = 1000 - 1100 * x_error;
							MotorCtrl3w(duty1, duty2, duty3);
						}
						if (x_error < 0 && x_error >= -1) // chao hong qiu zou
						{
							duty1 = 800 * x_error - 800;
							duty2 = -1050 + 2500 * x_error;
							duty3 = 1000 - 1200 * x_error;
							MotorCtrl3w(duty1, duty2, duty3);
						}
						if (x_error == 100) // rao qiu zhuan
						{
							duty1 = 1200;
							duty2 = 0;
							duty3 = -800;
							MotorCtrl3w(duty1, duty2, duty3);
						}
						if (x_error > 200 && x_error <= 201) // chao qiu men zou
						{
							duty1 = -900;
							duty2 = 500;
							duty3 = 0;
							MotorCtrl3w(duty1, duty2, duty3);
						}
						if (x_error < 200 && x_error >= 199) // chao qiu men zou
						{
							duty1 = 900;
							duty2 = 0;
							duty3 = -500;
							MotorCtrl3w(duty1, duty2, duty3);
						}
						if (x_error == 200)
						{
							k1 = 0;
						}
					}
				}
				else
				{
					duty1 = 1000;
					duty2 = 1000;
					duty3 = 1000;
					MotorCtrl3w(duty1, duty2, duty3);
				}
				receive_oneline3 = 0; //???h???\n???????????????
			}
			if (k1 == 0)
			{
				break;
			}
		}

		duty1 = 0;
		duty2 = -5560;
		duty3 = 5500;
		MotorCtrl3w(duty1, duty2, duty3);
		delayms(400);
	}
}
