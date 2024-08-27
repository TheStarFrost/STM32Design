/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
@??    ?????????
@E-mail  ??chiusir@163.com
@????IDE ??KEIL5.25.3???????
@'??????????????????????????????
@?????£?2022??02??19??????????£????????°?
@????????
@???????????????
@??    ???http://www.lqist.cn
@????????http://longqiu.taobao.com
@???????V1.0 ???????????'????????????
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

// ??????????l?
#include "include.h"


void straight(int duty1, int duty2, int duty3, char* txt)//the car go staight
{
	if (duty1 < PWM_DUTY_MAX) // ?????10000,??7200
	{
	duty1 = 0;
	duty2 = -2450;
	duty3 = 2400; 
	MotorCtrl3w(duty1,duty2, duty3);
	//sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
	//OLED_P6x8Str(10, 5, txt);
	}
}
void left(int duty1, int duty2, int duty3, char* txt)//the car turn left
{
	if (duty1 < PWM_DUTY_MAX) // ?????10000,??7200
	{
	duty1 = -600;
	duty2 = -1400;
	duty3 = 400;
	MotorCtrl3w(duty1,duty2, duty3);
	//sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
	//OLED_P6x8Str(10, 5, txt);
	}
}

void right(int duty1, int duty2, int duty3, char* txt)//the car turn right
{
	if (duty1 < PWM_DUTY_MAX) // ?????10000,??7200
	{
	duty1 = 600;
	duty2 = -500;
	duty3 = 1300;
	MotorCtrl3w(duty1,duty2, duty3);
	//sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
	//OLED_P6x8Str(10, 5, txt);
	}
}
double x_error = 0;
char txt[32];
int duty1;
int duty2;
int duty3;
int duty = 1000;
double k;
int k1;
int k2 = 1;
	
int kick = 0;

int main(void)
{
	//-----------------------????'??????----------------------------
	HAL_Init();			  // ??'??HAL??
	SystemClock_Config(); // ???????9???,72M
	delay_init(72);		  // ??'?????????
	JTAG_Set(SWD_ENABLE); // ??SWD??? ?????????????SWD??????
	

	//-----------------------------------------------------------------
	uart_init(USART_3, 115200);			//??'??????    ????LQMV4???g??  
	uart_init(USART_2, 115200);			//??'??????    ???????
    OLED_Init();
	LED_Init();							//??'??LED	
	MotorInit();
	delay_ms(500);		// ????
	printf("Long Qiu Ke Ji\n");
	printf("USART3 test,This is USART2.\n");
    //MotorCtrl3w(0, 0, 1200);
	//delayms(3000);
	
	
	while(1)
	{
		uart_SendBuf(&USART3_Handler, (uint8_t*)"This is USART3!\n");
    OLED_P8x16Str(10,0,"UART Test");              //?????
		
		k1 = 1;
		k2 = 1;
	while(k1 == 1)
	{
		//MotorCtrl3w(1200, 0, 0);
		//LED_Ctrl(RVS); 
		if(receive_oneline3 != 0)
		{
			k = x_error;
			uart_SendBuf(&USART3_Handler, (uint8_t*)ReadBuff);
			x_error = atof((char*)ReadBuff);
			sprintf(txt, "%f", x_error);
            OLED_P8x16Str(0, 4, txt); //??????????????????OLED?
			LED_Ctrl(RVS); 
            //printf("R:%s\n",ReadBuff);  //????2????????????
			
			//
			x_error = atof((char*)ReadBuff);
			
			if (x_error < 1000)
			{
				  
				
				
					if (x_error >= 0.15 && x_error <= 0.8)//chao hong qiu zou
					{
						right(duty1, duty2, duty3, txt);
					}
					if (x_error < -0.2 && x_error >= -1.2)//chao hong qiu zou
					{
						left(duty1, duty2, duty3, txt);
					}
					
					if (x_error >= 0 && x_error <= 0.15)//chao hong qiu zou
					{
						straight(duty1, duty2, duty3, txt);
					}
					if (x_error < 0 && x_error >= -0.2)//chao hong qiu zou
					{
						straight(duty1, duty2, duty3, txt);
					}
					if (x_error == 100)//rao qiu zhuan
					{
				duty1 = -1000;
				duty2 = -1000;
				duty3 = -1000;
				MotorCtrl3w(duty1, duty2, duty3);
					}
					if (x_error == 500)
					{
						k1 = 0;
					}
					if (x_error == 800)
					{
						MotorCtrl3w(0, 0, 0);
						k2 = 0;
					}
				}
					
			else
			{
				duty1 = -1000;
				duty2 = -1000;
				duty3 = -1000;
				MotorCtrl3w(duty1, duty2, duty3);
			}
			if (k1 == 0)
			{
				break;
			}
			receive_oneline3=0;         //???h???\n???????????????
		}
    		
		

		
	
	
		
}
	if (k2 == 1)
  {
	MotorCtrl3w(1700, 0, 0);
	delayms(2150);
	MotorCtrl3w(1000, 1000, 1000);
delayms(860);
MotorCtrl3w(0, 0, 0);
delayms(400);
	}
	else
	{
		MotorCtrl3w(0, 0, 0);
		delayms(10000);
	}
}	

}
