/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
@±à    Ð´£ºÁúÇñ¿Æ¼¼
@E-mail  £ºchiusir@163.com
@±àÒëIDE £ºKEIL5.25.3¼°ÒÔÉÏ°æ±¾
@Ê¹ÓÃÆ½Ì¨£º±±¾©ÁúÇñÖÇÄÜ¿Æ¼¼È«Ïò¸£À´ÂÖÐ¡³µ
@×îºó¸üÐÂ£º2022Äê02ÔÂ19ÈÕ£¬³ÖÐø¸üÐÂ£¬Çë¹Ø×¢×îÐÂ°æ£¡
@¹¦ÄÜ½éÉÜ£º
@Ïà¹ØÐÅÏ¢²Î¿¼ÏÂÁÐµØÖ·
@Íø    Õ¾£ºhttp://www.lqist.cn
@ÌÔ±¦µêÆÌ£ºhttp://longqiu.taobao.com
@Èí¼þ°æ±¾£ºV1.0 °æÈ¨ËùÓÐ£¬µ¥Î»Ê¹ÓÃÇëÏÈÁªÏµÊÚÈ¨
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

// °üº¬ËùÓÐÍ·ÎÄ¼þ
#include "include.h"


void straight(int duty1, int duty2, int duty3, char* txt)//the car go staight
{
	if (duty1 < PWM_DUTY_MAX) // ?????10000,??7200
	{
	duty1 = 0;
	duty2 = -2150;
	duty3 = 2100; 
	MotorCtrl3w(duty1,duty2, duty3);
	//sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
	//OLED_P6x8Str(10, 5, txt);
	}
}
void left(int duty1, int duty2, int duty3, char* txt)//the car turn left
{
	if (duty1 < PWM_DUTY_MAX) // ?????10000,??7200
	{
	duty1 = -500;
	duty2 = -1200;
	duty3 = 500;
	MotorCtrl3w(duty1,duty2, duty3);
	//sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
	//OLED_P6x8Str(10, 5, txt);
	}
}

void right(int duty1, int duty2, int duty3, char* txt)//the car turn right
{
	if (duty1 < PWM_DUTY_MAX) // ?????10000,??7200
	{
	duty1 = 500;
	duty2 = -600;
	duty3 = 1200;
	MotorCtrl3w(duty1,duty2, duty3);
	//sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
	//OLED_P6x8Str(10, 5, txt);
	}
}

void turn_zero(int duty1, int duty2, int duty3, char* txt)//speed turn zero
{
	if (duty1 > -PWM_DUTY_MAX) // ?????10000,??7200
	{
	duty1 = 0;
	duty2 = 0;
	duty3 = 0;
	MotorCtrl3w(duty1,duty2, duty3);
	//sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
	//OLED_P6x8Str(10, 5, txt);
	}
}

void stopandturnleft(int duty1, int duty2, int duty3)
{
	if (duty1 > -PWM_DUTY_MAX) // ?????10000,??7200
	{
	duty1 = 1000;
	duty2 = 1000;
	duty3 = 1000;
	MotorCtrl3w(duty1,duty2, duty3);
	//sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
	//OLED_P6x8Str(10, 5, txt);
	}
}

void stopandturnright(int duty1, int duty2, int duty3)
{
	if (duty1 > -PWM_DUTY_MAX) // ?????10000,??7200
	{
	duty1 = -1100;
	duty2 = -1100;
	duty3 = -1100;
	MotorCtrl3w(duty1,duty2, duty3);
	//sprintf(txt, "PWM: duty1=%2d,duty2=%2d,duty3=%2d", duty1, duty2, duty3);
	//OLED_P6x8Str(10, 5, txt);
	}
}

uint8_t TurnLeft[][4] = {
	{1, 1, 1, 0}, // 1110
	{1, 1, 0, 0}, // 1100
	{1, 0, 0, 0}, // 1000
	//{0, 0, 0, 0}  // 0000

};
uint8_t TurnRight[][4] = {
	{0, 0, 0, 1}, // 0001
	{0, 0, 1, 1}, // 0011
	{0, 1, 1, 1}  // 0111
};
uint8_t GoStraight[][4] = {
	{1, 1, 1, 1}, // 1111
	{0, 1, 1, 0}  // 0110
	//{1, 1, 1, 0}, // 1110
	//{0, 1, 1, 1}
	
};

uint8_t Stop[][4] = {
	{0, 0, 0, 0} // 0000
};


// ?????? ???????????
uint8_t is_turn_single(uint8_t Turn[4], uint8_t sensor_Value[4])
{
	return (sensor_Value[0] == Turn[0] &&
			sensor_Value[1] == Turn[1] &&
			sensor_Value[2] == Turn[2] &&
			sensor_Value[3] == Turn[3]);
};

// ?????? ????0/??1/??2
uint8_t is_turn(uint8_t Left[][4], uint8_t Right[][4], uint8_t Straight[][4], uint8_t stop[][4], uint8_t sensor[4])
{
	int i = 0;
	// ????
	for (i = 0; i < 3; i++)
	{
		if (is_turn_single(Left[i], sensor))
		{
			return 0;
		}
	}
	// ????
	for (i = 0; i < 2; i++)
	{
		if (is_turn_single(Straight[i], sensor))
		{
			return 1;
		}
	}
	// ????
	for (i = 0; i < 3; i++)
	{
		if (is_turn_single(Right[i], sensor))
		{
			return 2;
		}
	}
	if (is_turn_single(stop[0], sensor))
	{
		return 4;
	}
	// ??
	return 3;
};

uint8_t is_ultra(uint16_t CurrentDis, uint8_t LastState)
{
	uint8_t ultra_flag = 0; // 0????????,1?????????
	if (LastState == 0)
	{
		if (CurrentDis < 35)
		{
			ultra_flag = 1; // ????????????
		}
		else
		{
			ultra_flag = 0; // ??????
		}
	}
	else
	{
		if (CurrentDis > 45)
		{
			ultra_flag = 0;
		}
		else
		{
			ultra_flag = 1;
		}
	}
	return ultra_flag;
}


int main(void)
{
	int ECPULSE1 = 0,ECPULSE2 = 0,ECPULSE3 = 0;
	uint8_t sensor_Value[4];
	int status;
	int angle;
	int k = 0;
	int16_t duty = 1000, flag = 0;
	int16_t duty1 = 0, duty2 = 0, duty3 = 0;
	char txt[64];
	uint8_t ultra_sonic = 0; // I?????????
	uint16_t Dis = 0.0;
	uint8_t turn;
	//-----------------------???????----------------------------
	HAL_Init();			  // ???HAL?
	SystemClock_Config(); // ????9??,72M
	delay_init(72);		  // ???????
	JTAG_Set(SWD_ENABLE); // ??SWD?? ???????SWD????
	//-----------------------------------------------------------------
	//Encoder_Init_TIM2();   //????????ö????2,3,4?????AB??????
	//Encoder_Init_TIM3();
	//Encoder_Init_TIM4();
	Ultrasonic_Init();
	LED_Init();			// LED???
	OLED_Init();		// OLED???
	OLED_Show_LQLogo(); // ??LOGO
	delay_ms(500);		// ????
	OLED_CLS();			// ??
	MotorInit();
	//Ultrasonic_Init();
	OLED_P8x16Str(20, 0, "Project Test");
	
  angle = 0;
	while (1)
	{
		//status[0] = status[1]
		//status = 0;
		// ????? ?1?0
		Dis = Get_Distance();
		sprintf(txt, "Dis=%3d cm", Dis);
		OLED_P6x8Str(60, 2, txt); // ????????
		sensor_Value[0] = Read_sensor(sensor1);
		sensor_Value[1] = Read_sensor(sensor2);
		sensor_Value[2] = Read_sensor(sensor3);
		sensor_Value[3] = Read_sensor(sensor4);
		//delay_ms(200);
		sprintf(txt, "%d %d %d %d", sensor_Value[0], sensor_Value[1], sensor_Value[2], sensor_Value[3]);
		OLED_P6x8Str(0, 2, txt); // ???
		LED_Ctrl(RVS);

		// ???????
		turn = is_turn(TurnLeft, TurnRight, GoStraight, Stop, sensor_Value);
		ultra_sonic = is_ultra(Dis, ultra_sonic);
		if (ultra_sonic == 1)
		{
			MotorCtrl3w(600, 1000, -2300);
			//delayms(240);
		   //stopandturnleft(duty1, duty2, duty3);
			//delayms(50);
			angle = 43;
		}
		else
		//if (angle >= 370)
		//{
		  //straight(duty1, duty2, duty3, txt);
			
			//MotorCtrl3w(0, -1200, 1100);
		//}
		{
		switch (turn)
		{
		case 0:
			OLED_P6x8Str(0, 3, "Left!");
		  left(duty1, duty2, duty3, txt);
		  //status = 1;
		  if (sensor_Value[0] == 1 && sensor_Value[1] == 1)
		  {
			  status = 1;
		  }
		  else
			  status = 0;
		  angle = 0;
			break;
		case 1:
			OLED_P6x8Str(0, 3, "Line!");
		  straight(duty1, duty2, duty3, txt);
		  //status = 1;
		  if (sensor_Value[0] == 1 && sensor_Value[1] == 1)
		  {
			  status = 1;
		  }
		  else
			  status = 0;
		  angle = 0;
		  //delay_ms(500);
			break;
		case 2:
			OLED_P6x8Str(0, 3, "Right");
		  right(duty1, duty2, duty3, txt);
		  //status = 1;
		  if (sensor_Value[0] == 1 && sensor_Value[1] == 1)
		  {
			  status = 1;
		  }
		  else
			  status = 0;
		  angle = 0;
			break;
		case 4:
			OLED_P6x8Str(0, 3, "Out!!");
			if (status == 1 && angle < 20)
			{
				stopandturnleft(duty1, duty2, duty3);
				sprintf(txt, "right:%2d", angle);
	      OLED_P6x8Str(16, 5, txt);
			}
			status = 1;
			angle ++;
			if (status == 1 && angle >= 20 && angle < 56)
			{
				stopandturnright(duty1, duty2, duty3);
				sprintf(txt, "leftt:%2d", angle);
	      OLED_P6x8Str(16, 5, txt);
			}
			if (angle >= 56)
				MotorCtrl3w(0, -1200, 1100);
			break;
		  
		default:
			OLED_P6x8Str(0, 3, "Error");
		  straight(duty1, duty2, duty3, txt);
		  status = 0;
		  angle = 0;
			break;
		}
		//ECPULSE1=Read_Encoder(2);
		/*sprintf(txt,"E1:%04d ",ECPULSE1);
	    OLED_P6x8Str(0,4,txt);	
		ECPULSE2=Read_Encoder(3);
		sprintf(txt,"E2:%04d ",ECPULSE2);
	    OLED_P6x8Str(0,5,txt);	
		ECPULSE3=Read_Encoder(4);
		sprintf(txt,"E3:%04d ",ECPULSE3);
	    OLED_P6x8Str(0,6,txt);
		//delay_ms(50);*/
	}
}
}

