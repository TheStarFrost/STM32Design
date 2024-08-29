# 电子技术课程设计实验报告

**第六组  成员：雷雨欣，罗周子，黄一浩**

**小组主要分工：**

- 雷雨欣：代码实现主机、小车测试
- 黄一浩：小车测试与物理维护、代码整理与报告撰写
- 罗周子：小车测试、汇报PPT制作

## 摘要

本项目以载有STM32开发板与LQ_MV的全向福来轮小车为基础，目标为利用STM32开发板，集成**信息处理与控制**逻辑，通过**C语言与Python**的编程实现，研发小车的**自动循迹、避障、自动停车、踢球**等功能，最终将小车开发为**自主运动的智能小车**。

## 目录

目录如下，点击可直接到对应部分，每个科目的**代码**额外给出链接方便查找。科目的**代码外内容**写在**代码前**

[TOC]

## 文件结构与模块划分

本项目的文件结构如下：

```
|-- STM32DESIGN
    |-- Report.md
    |-- README.md
    |-- Log.md
    |-- ColorTracking.py
    |-- Project
        |-- MDK
            |-- LQ_STM32F1_Demo
            |-- ...
        |-- USER
            |-- main.c
            |-- ...
        |-- ...
```

其中 `Report.md` 为本实验报告，`Log.md` 为课题开发过程日志，`README.md` 为仓库说明文档， `ColorTracking.py` 为小车摄像头色块识别与追踪代码，并负责与单片机就获取的视觉信息进行蓝牙通信，`Project/USER/main.c` 是小车单片机集成控制的核心代码，通过与电机、红外探头、编码器、摄像头等进行通信，获取信息并控制电机进行运动。

执行方式为将Project文件夹下载，通过 `Keil uVision5` 打开 `Project/MDK/LQ_STM32F1_Demo` 工程文件，编译项目内文件后下载到单片机内，完成单片机上的程序下载。 `ColorTracking.py` 则是通过小车摄像头USB-TypeC链接后，在小车缓存内放入该Python文件替换 `main.py` 作为摄像头执行的主程序执行。此外，摄像头的缓存内应当含有小车初始时即有的 `LQ_Module.py` 文件以保证库导入正常。

我们划分小车的功能模块方式为：以单片机主控模块为核心，和其他各个模块交互，进行统一的逻辑判断与执行。
    - **单片机主控模块**：负责接收各个其他模块传入的感知信息或标志信息，对信息做出反应，执行设定的行动逻辑，这部分承担主要的数字逻辑设计。
    - **超声模块**：获取小车与前方障碍的距离并返回
    - **红外传感模块**：在小车前下方安装的红外模块能感知黑白色块并回传黑白色块的粗糙位置信息，供小车完成红外传感循迹
    - **电机驱动模块**：负责接收从**单片机主控模块**传达的电机驱动参数，并通过电机控制板的信号转换与**电源供电**驱动电机运动，结合**单片机主控模块**向电机提供的方向信息，带动轮子运动
    - **视觉模块**：由LQ_MV摄像头及芯片承担，能获取小车前方图像，并进行视觉分析，分析后的结果回传**单片机主控模块**，进一步进行行动逻辑执行
    - **供电模块**：由电源、电压调节硬件软件等承担


## 课题处理详程

### 科目一:小车光电循迹
小车要能够成为智能小车自主寻路，第一步是让小车能够循迹而行。STM32开发板上的串口以及预先提供的系统库函数，能让我们装载红外探头模块，并调取红外传感器，通过传感器发射的红外光，反射接收后识别地面可行进的黑色路线。STM32开发板实现与红外探头的通信后，通过逻辑设计，对**中心及边缘的红外探头返回值组合情况进行不同的处理，导向不同的电机驱动策略**，即可完成小车的基础自动光电循迹。

仅进行光电循迹，容易产生急弯转弯冲出，或直角弯无法及时转向等问题。针对此问题，我们设计了逻辑判断，让小车在不意冲出赛道时，及时自我停止，后左右环视寻找赛道所在，寻得赛道之后，便归原路，继续向前疾驰。急弯等同理。

科目一的实现过程中不时会出现轮胎打滑或电机驱动值执行不一，甚至轮胎掉落等问题。而最大的问题往往是转弯的幅度设置不合适，而又因为场地因素轮胎因素难以通过物理完全分析，纯粹的物理分析设计给出的电机驱动参数不甚起效，很多时候往往还需实地测量**精调转弯参数**。通过多轮的参数调整，我们最终实现了较为完好的循迹代码，成功完成了简单赛道和复杂赛道的验收，并完赛光电循迹竞速赛。

科目一使用的系统库开发环境为电机测试环境，在此部分未出现问题，着实幸运，然后续课题却恰在此处重重下了功夫。

[科目一简单赛道展示](https://cloud.tsinghua.edu.cn/f/8d9df5c7846146b7a12c/)如下所示 也可通过链接观看
<center>

![科目一循迹简单赛道展示GIF](./asset/科目一%20循迹%20简单赛道.gif)
</center>

[科目一复杂赛道展示](https://cloud.tsinghua.edu.cn/f/a0b0f404d8ea46a98552/)如下所示 也可通过链接观看
<center>

![科目一循迹简单赛道展示GIF](./asset/科目一%20循迹%20复杂赛道.gif)
</center>


科目一源代码如下，本部分只与单片机主控程序 `main.c` 相关，其他系统库函数使用电机测试例程 `5.Motor_Test` 内代码处理,这部分代码提供详见课程参考资料

#### 简单 & 复杂赛道代码 `main.c` 

```c
#include "include.h"

void straight(int duty1, int duty2, int duty3, char *txt) // the car go staight
{
    if (duty1 < PWM_DUTY_MAX) // 阈值
    {
        duty1 = 0;
        duty2 = -1950;
        duty3 = 1900;
        MotorCtrl3w(duty1, duty2, duty3);
    }
}
void left(int duty1, int duty2, int duty3, char *txt) // the car turn left
{
    if (duty1 < PWM_DUTY_MAX) // 阈值设定
    {
        duty1 = -600;
        duty2 = -1400;
        duty3 = 400;
        MotorCtrl3w(duty1, duty2, duty3);
    }
}

void right(int duty1, int duty2, int duty3, char *txt) // the car turn right
{
    if (duty1 < PWM_DUTY_MAX) // 阈值
    {
        duty1 = 600;
        duty2 = -500;
        duty3 = 1300;
        MotorCtrl3w(duty1, duty2, duty3);
    }
}

void turn_zero(int duty1, int duty2, int duty3, char *txt) // speed turn zero
{
    if (duty1 > -PWM_DUTY_MAX) // 阈值
    {
        duty1 = 0;
        duty2 = 0;
        duty3 = 0;
        MotorCtrl3w(duty1, duty2, duty3);
    }
}

void stopandturnleft(int duty1, int duty2, int duty3)
{
    if (duty1 > -PWM_DUTY_MAX) // 阈值
    {
        duty1 = 1000;
        duty2 = 1000;
        duty3 = 1000;
        MotorCtrl3w(duty1, duty2, duty3);
    }
}

void stopandturnright(int duty1, int duty2, int duty3)
{
    if (duty1 > -PWM_DUTY_MAX) // 阈值
    {
        duty1 = -1100;
        duty2 = -1100;
        duty3 = -1100;
        MotorCtrl3w(duty1, duty2, duty3);
    }
}

//转向预定逻辑
uint8_t TurnLeft[][4] = {
    {1, 1, 1, 0}, // 1110
    {1, 1, 0, 0}, // 1100
    {1, 0, 0, 0}, // 1000
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

uint8_t Stop[][4] = {
    {0, 0, 0, 0} // 0000
};

// 转向单个情况匹配函数
uint8_t is_turn_single(uint8_t Turn[4], uint8_t sensor_Value[4])
{
    return (sensor_Value[0] == Turn[0] &&
            sensor_Value[1] == Turn[1] &&
            sensor_Value[2] == Turn[2] &&
            sensor_Value[3] == Turn[3]);
};

// 转向逻辑函数
uint8_t is_turn(uint8_t Left[][4], uint8_t Right[][4], uint8_t Straight[][4], uint8_t stop[][4], uint8_t sensor[4])
{
    int i = 0;
    // 左转
    for (i = 0; i < 3; i++)
    {
        if (is_turn_single(Left[i], sensor))
        {
            return 0;
        }
    }
    // 直行
    for (i = 0; i < 2; i++)
    {
        if (is_turn_single(Straight[i], sensor))
        {
            return 1;
        }
    }
    // 右转
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
    // 其他
    return 3;
};

int main(void)
{
    int ECPULSE1 = 0, ECPULSE2 = 0, ECPULSE3 = 0;
    uint8_t sensor_Value[4];
    int status;
    int angle; //冲出赛道后,寻找路径用回转参数
    int k = 0;
    int16_t duty = 1000, flag = 0;
    int16_t duty1 = 0, duty2 = 0, duty3 = 0;
    char txt[64];
    uint8_t turn;
    //-----------------------初始化----------------------------
    HAL_Init();           // HAL初始化
    SystemClock_Config(); // 初始化系统时钟为72M
    delay_init(72);       // 延迟初始化
    JTAG_Set(SWD_ENABLE); // SWD初始化
    //-----------------------------------------------------------------
    Encoder_Init_TIM2(); //初始化编码器
    Encoder_Init_TIM3();
    Encoder_Init_TIM4();
    LED_Init();         // LED初始化
    OLED_Init();        // OLED初始化
    OLED_Show_LQLogo(); // 展示LOGO
    delay_ms(500);      // 系统初始化
    OLED_CLS();         // OLED清屏
    MotorInit();
    // Ultrasonic_Init();
    OLED_P8x16Str(20, 0, "Project Test");

    angle = 0;
    while (1)
    {
        // 光电探测
        sensor_Value[0] = Read_sensor(sensor1);
        sensor_Value[1] = Read_sensor(sensor2);
        sensor_Value[2] = Read_sensor(sensor3);
        sensor_Value[3] = Read_sensor(sensor4);
        sprintf(txt, "%d %d %d %d", sensor_Value[0], sensor_Value[1], sensor_Value[2], sensor_Value[3]);
        OLED_P6x8Str(0, 2, txt); // 探测信息打印
        LED_Ctrl(RVS);

        // 转弯判断
        turn = is_turn(TurnLeft, TurnRight, GoStraight, Stop, sensor_Value);

        //转弯逻辑
        switch (turn)
        {
        case 0:
            OLED_P6x8Str(0, 3, "Left!");
            left(duty1, duty2, duty3, txt);
            // status = 1;
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
            // status = 1;
            if (sensor_Value[0] == 1 && sensor_Value[1] == 1)
            {
                status = 1;
            }
            else
                status = 0;
            angle = 0;
            break;
        case 2:
            OLED_P6x8Str(0, 3, "Right");
            right(duty1, duty2, duty3, txt);
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
            if (status == 1 && angle < 140)
            {
                stopandturnleft(duty1, duty2, duty3);
                sprintf(txt, "right:%2d", angle);
                OLED_P6x8Str(16, 5, txt);
            }
            status = 1;
            angle++;
            if (status == 1 && angle > 140)
            {
                stopandturnright(duty1, duty2, duty3);
                sprintf(txt, "leftt:%2d", angle);
                OLED_P6x8Str(16, 5, txt);
            }
            break;

        default:
            OLED_P6x8Str(0, 3, "Error");
            straight(duty1, duty2, duty3, txt);
            status = 0;
            angle = 0;
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
```


### 科目二:避障
小车若欲驰骋519/520江湖之中，对于邪佞妖障（砖块和行李箱），因无甚么神通之法（撞开），还需隐走绕行（识别后避开障碍绕道），以保自身周全。

在打造智能小车的过程中，除了循迹，还需做到对障碍的规避，对于小车的车身安稳等都十分重要。对此，我们考虑加入超声波模块，以获取小车与障碍之间的距离，并与STM32开发板建立通讯。完成通讯后，借助超声波识别到障碍物后，小车开启避障程序，绕行障碍物行驶，同时保持对原有循迹轨迹的探测。小车若绕过了障碍物重新寻得路径，则会继续按照路径行驶。若未找到路径，则会原地旋转尝试寻找。同时，**超声避障程序的触发采用了施密特触发器的形式**，以保证超声避障程序的稳定性，以及小车重回循迹时的安全性

在调试过程中，因需要使用超声波设备，我们将代码环境换到了超声波设备测试例程 `8.HCSR04_Test` ,因其并未处于电机测试例程下，电机出现了**疯转**的问题，问题出在定时器上。通过助教学长的指导以及对系统库函数的探索修改，尤其是对定时器机制的学习思考，我们最终成功调节系统库函数解决了该问题。

[科目二避障展示](https://cloud.tsinghua.edu.cn/f/96abf47b68864ea08261/)如下所示 也可通过链接观看
<center>

![科目二避障展示GIF](./asset/科目二%20避障.gif)
</center>


科目二源代码如下，本部分只与单片机主控程序 `main.c` 相关，其他系统库函数使用超声测试例程 `8.HCSR04_Test` 内代码处理，并在这基础上按说明修改了系统库函数解决电机疯转问题，此外无其他修改，这部分代码提供详见课程参考资料

#### 科目二 `main.c` 

```c
    '''科目二代码'''
```

### 科目三:小车踢球
在进行了循迹和避障后，小车进一步智能化有赖于与摄像头的通讯与目标的追踪识别。因此，我们开发了基于摄像头色块识别追踪的的小车踢球功能。小车若想踢球，则先需要看得到球。因此，我们先配置了蓝牙交互环境，确保**摄像头和单片机能够通讯**。保证二者通讯后，接下来则开始摄像头视觉识别的数字系统设计。我们的整体思路是，通过对特定色块的精确识别（通过系统库函数find_blobs及校准色块参数、添加适合小车的各种识别限制或辅助等实现），追踪红色色块（小球），并根据红色色块的中心偏移程度、距离靠近程度与单片机交互，进行**追球、微调方向、绕球旋转**等策略，实现合适距离内**发现小球、追上小球，再绕球旋转**。

进一步，完成对小球的“捕获”后，我们准备球门的识别。和小球同理，只是识别颜色更换为绿色，并同时开始红绿识别。我们的设计策略是，在**确认追上捕获红球**后（视觉模块给出标志位），开始**进行绿色的寻找识别**，找到后，小车**微调方位**到红球和绿色球门对齐，再进行**射门**操作，以踢球入门。

完成射门后以及找到红球前，我们设计小车均为**原地旋转**，前者为等待小球位置重置开始，同时避免冲入球门，后者为尽可能全面地发现周围红球信息，及时捕捉。

调试过程中，曾出现过如识别、电机动作延迟，UART蓝牙信息交互易打断主程序等问题。针对识别和动作延迟，我们细调了**更合适的图片识别时间间隔**，并在出现动作转换的部分设计了**更精密的动作逻辑**，与识别延迟的效果相消。对于UART蓝牙信息交互打断主程序的问题，我们将主程序的延迟设置为更**匹配**视觉模块信息的数值，同时在一些特殊的动作上，如**踢球射门**等，**适时跳出与视觉模块的交互**，避免打断情况，保证程序的正常执行。

此外，在颜色识别上，我们曾遇到过多次问题，如**色块不完整、易受光照影响，识别过窄过宽**等问题，我们以**精调颜色识别区域、色块阈值范围、精密设置标志位触发值**等方法一一解决，还有许多被解决了的问题，不一一赘述。

根据科目三课题子任务设置，分有小车单独找球门入门，和小车踢球部分，展示与代码见下。

[科目三小车进门展示](https://cloud.tsinghua.edu.cn/f/c2875d4e1e6143f6b3e7/)如下GIF所示 若通过PDF打开该报告，也可通过链接观看
<center>

![科目三小车进球门展示GIF](./asset/科目三%20小车进球门.gif)
</center>

[科目三小车踢球展示](https://cloud.tsinghua.edu.cn/f/e916a4b813ae43c88c10/)如下GIF所示 若通过PDF打开该报告，也可通过链接观看
<center>

![科目三小车踢球展示GIF](./asset/科目三%20小车踢球.gif)
</center>


科目三源代码如下，分为小车进球门版本与小车踢球版本，分别各有一个C与Python文件，分别为单片机主程序文件与视觉模块文件。

小车进球门代码
#### 代码：小车进球门`main.c` 部分

```c
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

//系统库导入
#include "include.h"

double x_error = 0; //视觉模块信息载体,常表示色块偏移程度
char txt[32];//打印信息
int duty1;//电机驱动参数
int duty2;
int duty3;
double k;//历史色块偏移程度

int main(void)
{
	//-----------------------初始化----------------------------
	HAL_Init();			  // 初始化HAL库
	SystemClock_Config(); // 设置系统时钟 72M
	delay_init(72);		  // 初始化延迟
	JTAG_Set(SWD_ENABLE); // 初始化SWD
	

	//-----------------------------------------------------------------
	uart_init(USART_3, 115200);			//蓝牙串口3初始化 建立与LQMV4的通讯  
	uart_init(USART_2, 115200);			//蓝牙串口2初始化 可开启蓝牙通讯
    OLED_Init();
	LED_Init();							//初始化LED	
	MotorInit();
	delay_ms(500);		// 初始化所需延迟时间
	printf("Long Qiu Ke Ji\n");
	printf("USART3 test,This is USART2.\n");
    uart_SendBuf(&USART3_Handler, (uint8_t*)"This is USART3!\n");
    OLED_P8x16Str(10,0,"UART Test");              //测试信息
	
	while(1) //程序主循环
	{
        // 若接受到信息帧
		if(receive_oneline3 != 0)
		{
			k = x_error; //历史x_error记录
			uart_SendBuf(&USART3_Handler, (uint8_t*)ReadBuff);
			x_error = atof((char*)ReadBuff); //更新x_error
			sprintf(txt, "%f", x_error);
            OLED_P8x16Str(0, 4, txt); // 打印信息到OLED
			LED_Ctrl(RVS); // LED反转
			if (x_error < 1000)
			{
				if (k > 1000) //x_error转阶段:从寻门转入正常跟门,刹车程序
				{
					MotorCtrl3w(-600, -600, -600);
			        delayms(100);
				}
				else // 常规x_error,根据x_error调整方向跟门
				{
					if (x_error > 0)
					{
						duty1 = 800 * x_error + 700;
                        duty2 = -1650 + 1000 * x_error;
                        duty3 = 1600 - 1000 * x_error;
                        MotorCtrl3w(duty1, duty2, duty3);
					}
					else if (x_error < 0)
					{
						duty1 = 800 * x_error - 800;
                        duty2 = -1650 + 2500 * x_error;
                        duty3 = 1600 - 1200 * x_error;
                        MotorCtrl3w(duty1, duty2, duty3);
					}
			    }
		    }
			else // 寻门
			{
				duty1 = 1000;
				duty2 = 1000;
				duty3 = 1000;
				MotorCtrl3w(duty1, duty2, duty3);
			}
			receive_oneline3=0;         //清除帧标志位
		}            
	}
}
```

#### 小车进球门 `ColorTracking.py` 部分

```python
# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# Single Color Code Tracking Example
#
# This example shows off single color code tracking using the OpenMV Cam.
#
# A color code is a blob composed of two or more colors. The example below will
# only track colored objects which have both the colors below in them.

import sensor
import time
import math
import display

import pyb
from pyb import UART,LED,Pin

LEDB = LED(3)                  # 蓝色LED

uart = UART(3, 115200)         # 初始化串口3，波特率115200 --》母板上 UART接口

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds track in general red/green things. You may wish to tune them...
thresholds = [
    (
        18,
        100,
        20,
        127,
        0,
        127,
    ),  # generic_red_thresholds -> index is 0 so code == (1 << 0)
     (65, 99, -74, 12, 41, 53),
]  # generic_green_thresholds -> index is 1 so code == (1 << 1)
# Codes are or'ed together when "merge=True" for "find_blobs".

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA2)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
lcd = display.SPIDisplay()

clock = time.clock()

# 最大色块识别函数
def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob[2]*blob[3] > max_size:
            max_blob=blob
            max_size = blob[2]*blob[3]
    return max_blob

# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. "merge=True" must be set to merge overlapping color blobs for color codes.

# 主循环
while True:
    clock.tick()
    img = sensor.snapshot()
    x_error = 10000
    blobs1 = img.find_blobs([thresholds[1]], pixels_threshold=100, area_threshold=100, merge=True)
    if blobs1:                  # 找到追踪目标
        blob = find_max(blobs1)  # 提取blobs中面积最大的一个色块blob
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
        # 计算x_error:目标色块偏移中心程度
        x_error = 2 * (blob.cx()/img.width()) - 1
        print(blob.h())
        if blob.h() > 125:
            x_error = x_error + 100
    uart.write(str(x_error) + '\n')                       # 将变量通过串口发送出
    time.sleep_ms(100)                          # 延时0.5秒
    LEDB.toggle()
    if uart.any():                              # 如果接收到任何消息
        receive = uart.read().decode().strip()  # 将接收到的消息提取出来
```

小车踢球代码

#### 小车踢球 `main.c` 部分

```c
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

//系统库导入
#include "include.h"

double x_error = 0;//色块偏移标志变量
char txt[32];
int duty1;//电机驱动参数
int duty2;
int duty3;
double k;//历史x_error
int k1; //内外循环控制参数

int kick = 0;

int main(void)
{
	//-----------------------系统库初始化----------------------------
	HAL_Init();			  // HAL初始化
	SystemClock_Config(); // 初始化系统时钟 72M
	delay_init(72);		  // 延迟初始化
	JTAG_Set(SWD_ENABLE); // SWD初始化

	//-----------------------------------------------------------------
	uart_init(USART_3, 115200);			//蓝牙串口3初始化 建立与LQMV4的通讯  
	uart_init(USART_2, 115200);			//蓝牙串口2初始化 可开启蓝牙通讯
	OLED_Init();
	LED_Init(); //初始化LED
	MotorInit();
	delay_ms(500); // 延迟500ms等待初始化完成
	printf("Long Qiu Ke Ji\n");
	printf("USART3 test,This is USART2.\n");

	while (1)
	{
		uart_SendBuf(&USART3_Handler, (uint8_t *)"This is USART3!\n");
		OLED_P8x16Str(10, 0, "UART Test"); //测试信息打印
		k1 = 1; //默认进入内循环
		while (k1 == 1)
		{
			if (receive_oneline3 != 0)
			{
				k = x_error;
				uart_SendBuf(&USART3_Handler, (uint8_t *)ReadBuff);
				x_error = atof((char *)ReadBuff);
				sprintf(txt, "%f", x_error);
				OLED_P8x16Str(0, 4, txt); //打印信息到OLED
				LED_Ctrl(RVS);
				if (x_error < 1000)
				{
					if (k > 1000) //找到球转变状态刹车
					{
						MotorCtrl3w(-600, -600, -600);
						delayms(100);
					}
					if (k == 100 && x_error != 100)
					{
						MotorCtrl3w(-600, 0, 0);
						delayms(100);
					}
                    //刚完成射门
					if (k == 200 && x_error != 200)
					{
						MotorCtrl3w(2000, 2000, 2000);
						delayms(100);
						x_error = 10000;
					}
					if (k <= 1 && k >= -1 && x_error == 100)
					{
						MotorCtrl3w(0, 0, 0);
						delayms(100);
					}
					if (x_error < -5) // 进球后停止
					{
						duty1 = 0;
						duty2 = 0;
						duty3 = 0;
						MotorCtrl3w(duty1, duty2, duty3);
					}

					else // no jin qiu
					{
                        //微调方向追踪红球
						if (x_error >= 0 && x_error <= 1)
						{
							duty1 = 800 * x_error + 700;
							duty2 = -1050 + 1100 * x_error;
							duty3 = 1000 - 1100 * x_error;
							MotorCtrl3w(duty1, duty2, duty3);
						}
						if (x_error < 0 && x_error >= -1)
						{
							duty1 = 800 * x_error - 800;
							duty2 = -1050 + 2500 * x_error;
							duty3 = 1000 - 1200 * x_error;
							MotorCtrl3w(duty1, duty2, duty3);
						}
                        //已追上红球绕球旋转
						if (x_error == 100)
						{
							duty1 = 1200;
							duty2 = 0;
							duty3 = -800;
							MotorCtrl3w(duty1, duty2, duty3);
						}
                        //找到球门向球门走
						if (x_error > 200 && x_error <= 201)
						{
							duty1 = -900;
							duty2 = 500;
							duty3 = 0;
							MotorCtrl3w(duty1, duty2, duty3);
						}
						if (x_error < 200 && x_error >= 199)
						{
							duty1 = 900;
							duty2 = 0;
							duty3 = -500;
							MotorCtrl3w(duty1, duty2, duty3);
						}
                        //准备射门,跳出循环进入射门程序
						if (x_error == 200)
						{
							k1 = 0;
						}
					}
				}
                //自转寻球
				else
				{
					duty1 = 1000;
					duty2 = 1000;
					duty3 = 1000;
					MotorCtrl3w(duty1, duty2, duty3);
				}
				receive_oneline3 = 0; //清除帧标志位
			}
            //射门检测
			if (k1 == 0)
			{
				break;
			}
		}
        //射门
		duty1 = 0;
		duty2 = -5560;
		duty3 = 5500;
		MotorCtrl3w(duty1, duty2, duty3);
		delayms(400);
	}
}
```

#### 小车踢球`ColorTracking.py` 部分

```python
# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# Single Color Code Tracking Example
#
# This example shows off single color code tracking using the OpenMV Cam.
#
# A color code is a blob composed of two or more colors. The example below will
# only track colored objects which have both the colors below in them.

import sensor
import time
import math
import display

import pyb
from pyb import UART, LED, Pin

LEDB = LED(3)  # 蓝色LED

uart = UART(3, 115200)  # 初始化串口3，波特率115200 --》母板上 UART接口

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds track in general red/green things. You may wish to tune them...
thresholds = [
    (
        22,
        100,
        41,
        127,
        -128,
        127,
    ),  # generic_red_thresholds -> index is 0 so code == (1 << 0)
    (65, 99, -74, 12, 41, 53),
]  # generic_green_thresholds -> index is 1 so code == (1 << 1)
# Codes are or'ed together when "merge=True" for "find_blobs".

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA2)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
lcd = display.SPIDisplay()

clock = time.clock()

# 最大色块识别函数
def find_max(blobs):
    max_size = 0
    for blob in blobs:
        if blob[2] * blob[3] > max_size:
            max_blob = blob
            max_size = blob[2] * blob[3]
    return max_blob


# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. "merge=True" must be set to merge overlapping color blobs for color codes.
# 主循环
while True:
    clock.tick()
    img = sensor.snapshot()
    x_error = 10000
    x_error_red = 10000
    # 找红色
    blobs1 = img.find_blobs(
        [thresholds[0]], pixels_threshold=100, area_threshold=100, merge=True
    )
    if blobs1:  # 找到追踪目标
        blob = find_max(blobs1)  # 提取blobs中面积最大的一个色块blob
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints(
            [(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20
        )
        x_error = 2 * (blob.cx() / img.width()) - 1
        x_error_red = x_error
        print(blob.cy())
        y_up = blob.y()
        if blob.cy() > 111:
            x_error = 100

    # 找绿色
    blobs2 = img.find_blobs(
        [thresholds[1]], pixels_threshold=100, area_threshold=100, merge=True
    )
    if blobs2:  # 找到追踪目标
        blob = find_max(blobs2)  # 提取blobs中面积最大的一个色块blob
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints(
            [(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20
        )
        x_error_green = 2 * (blob.cx() / img.width()) - 1
        if abs(x_error_red - x_error_green) < 0.13:
            x_error = 200
        y_down = blob.y() + blob.h()
    uart.write(str(x_error) + "\n")  # 将变量通过串口发送出
    time.sleep_ms(100)  # 延时0.5秒
    if uart.any():  # 如果接收到任何消息
        receive = uart.read().decode().strip()  # 将接收到的消息提取出来
```

### 自主创新项目一:小车摄像头循迹、避障、停车
从小车踢球更进一步，我们利用**成型的色块识别函数**，将其应用在循迹上。通过对**黑色路线的识别、绿色障碍的识别绕道、紫色标志位的停车**，我们完成了小车摄像头循迹、避障、停车。

过程中，我们调整颜色识别阈值、视野范围以及视觉识别范围等，并加以新的色块识别逻辑如**合并色块、过滤小色块等**，保证**颜色识别延迟、位置等准确可靠**，通过**细调弯道、直道和避障策略，保证运动灵活性**，由于先前代码的逻辑完善，不消多时我们便成功完成了该项目。


[自主创新项目一:小车摄像头循迹、避障、停车全流程赛道展示](https://cloud.tsinghua.edu.cn/f/65c2a96d1f044aa58d2c/)如下所示 也可通过链接观看
<center>

![自主创新项目一小车循迹全流程展示GIF](./asset/自主创新项目一%20摄像头循迹.gif)
</center>

该自主创新项目源代码如下所示，包含一个C文件与Python文件，与前面项目同理，C文件为单片机主程序文件，而Python为视觉模块文件，C的系统库环境采用的是独立例程中 `10.USART_Test` 的系统库函数，此处不再另行展示。

#### `main.c` 代码

```c
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

// 系统库导入
#include "include.h"

void straight(int duty1, int duty2, int duty3, char *txt) // the car go staight
{
    if (duty1 < PWM_DUTY_MAX) // 限速
    {
        duty1 = 0;
        duty2 = -2450;
        duty3 = 2400;
        MotorCtrl3w(duty1, duty2, duty3);
    }
}
void left(int duty1, int duty2, int duty3, char *txt) // the car turn left
{
    if (duty1 < PWM_DUTY_MAX) // 限速
    {
        duty1 = -600;
        duty2 = -1400;
        duty3 = 400;
        MotorCtrl3w(duty1, duty2, duty3);
    }
}

void right(int duty1, int duty2, int duty3, char *txt) // the car turn right
{
    if (duty1 < PWM_DUTY_MAX) // 限速
    {
        duty1 = 600;
        duty2 = -500;
        duty3 = 1300;
        MotorCtrl3w(duty1, duty2, duty3);
    }
}
double x_error = 0; // 位置偏移量
char txt[32];
int duty1; // 电机参数
int duty2;
int duty3;
int k1;
int k2;

int main(void)
{
    //-----------------------系统库初始化----------------------------
    HAL_Init();           // HAL初始化
    SystemClock_Config(); // 初始化系统时钟 72M
    delay_init(72);       // 延迟初始化
    JTAG_Set(SWD_ENABLE); // SWD初始化

    //-----------------------------------------------------------------
    uart_init(USART_3, 115200); // 蓝牙串口3初始化 建立与LQMV4的通讯
    uart_init(USART_2, 115200); // 蓝牙串口2初始化 可开启蓝牙通讯
    OLED_Init();
    LED_Init(); // 初始化LED
    MotorInit();
    delay_ms(500); // 等待系统初始化
    printf("Long Qiu Ke Ji\n");
    printf("USART3 test,This is USART2.\n");

    // 主循环
    while (1)
    {
        uart_SendBuf(&USART3_Handler, (uint8_t *)"This is USART3!\n");
        OLED_P8x16Str(10, 0, "UART Test"); // 调试信息
        k1 = 1;
        k2 = 1;
        while (k1 == 1)
        {
            if (receive_oneline3 != 0)
            {
                k = x_error;
                uart_SendBuf(&USART3_Handler, (uint8_t *)ReadBuff);
                x_error = atof((char *)ReadBuff);
                sprintf(txt, "%f", x_error);
                OLED_P8x16Str(0, 4, txt); //在OLED上打印信息
                LED_Ctrl(RVS);
                if (x_error < 1000)
                {
                    if (x_error >= 0.15 && x_error <= 0.8) // chao hong qiu zou
                    {
                        right(duty1, duty2, duty3, txt);
                    }
                    if (x_error < -0.2 && x_error >= -1.2) // chao hong qiu zou
                    {
                        left(duty1, duty2, duty3, txt);
                    }
                    //直走
                    if (x_error >= 0 && x_error <= 0.15)
                    {
                        straight(duty1, duty2, duty3, txt);
                    }
                    if (x_error < 0 && x_error >= -0.2)
                    {
                        straight(duty1, duty2, duty3, txt);
                    }
                    if (x_error == 100) // rao qiu zhuan
                    {
                        duty1 = -1000;
                        duty2 = -1000;
                        duty3 = -1000;
                        MotorCtrl3w(duty1, duty2, duty3);
                    }
                    if (x_error == 500)//避障标志位
                    {
                        k1 = 0;
                    }
                    if (x_error == 800)//停车标志位
                    {
                        MotorCtrl3w(0, 0, 0);
                        k2 = 0;
                    }
                }
                //寻找轨迹
                else
                {
                    duty1 = -1000;
                    duty2 = -1000;
                    duty3 = -1000;
                    MotorCtrl3w(duty1, duty2, duty3);
                }
                // k1跳出循环标志位,准备执行避障或停车
                if (k1 == 0 || k2==0)
                {
                    break;
                }
            }
        }
        //若非已停车,遇障碍则避障,否则停车
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
```

#### `ColorTracking.py` 程序

```python
# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# Single Color Code Tracking Example
#
# This example shows off single color code tracking using the OpenMV Cam.
#
# A color code is a blob composed of two or more colors. The example below will
# only track colored objects which have both the colors below in them.

import sensor
import time
import math
import display

import pyb
from pyb import UART,LED,Pin

LEDB = LED(3)                  # 蓝色LED

uart = UART(3, 115200)         # 初始化串口3，波特率115200 --》母板上 UART接口

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds track in general red/green things. You may wish to tune them...
thresholds = [
    (
        2,
        43,
        17,
        -18,
        32,
        -4,
    ),  # generic_red_thresholds -> index is 0 so code == (1 << 0)
    (29, 87, -106, -17, 19, 52),
    (0, 38, 26, 127, -26, 30)
]  # generic_green_thresholds -> index is 1 so code == (1 << 1)
# Codes are or'ed together when "merge=True" for "find_blobs".

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA2)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
lcd = display.SPIDisplay()

clock = time.clock()

# 寻找最低色块
def find_low(blobs):
    max_cy=0
    for blob in blobs:
        if blob.cy() > max_cy:
            max_blob=blob
            max_cy = blob.cy()
    return max_blob
# 寻找最高色块
def find_max(blobs):
    max_size = 0
    for blob in blobs:
        if blob[2] * blob[3] > max_size:
            max_blob = blob
            max_size = blob[2] * blob[3]
    return max_blob

# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. "merge=True" must be set to merge overlapping color blobs for color codes.

while True:
    clock.tick()
    img = sensor.snapshot()
    x_error = 10000
    x_error_red = 10000
    #找黑色
    blobs1 = img.find_blobs([thresholds[0]], roi = [0, 50, 160, 140], pixels_threshold=100, area_threshold=100, merge=True)
    if blobs1:                  # 找到追踪目标
        blob = find_low(blobs1)  # 提取blobs中面积最大的一个色块blob
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
            img.draw_rectangle((0, 50, 160, 140),color=(255, 0, 0))       # 根据色块blob位置画红色框
            img.draw_cross(80, 120,color=(0, 0, 255))  # 根据色块位置在中心画蓝色十字
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
        x_error = 2 * (blob.cx()/img.width()) - 1 - 0.2
        x_error_red = x_error
        print(x_error)
        y_up = blob.y()
        if blob.cy() < 90:
            x_error = 100

    #找绿色
        blobs2 = img.find_blobs([thresholds[1]], roi = [0, 50, 160, 140], pixels_threshold=100, area_threshold=100, merge=True)
        if blobs2:                  # 找到追踪目标
            blob = find_max(blobs2)  # 提取blobs中面积最大的一个色块blob
            if blob.elongation() > 0.5:
                img.draw_edges(blob.min_corners(), color=(255, 0, 0))
                img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
                img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
                img.draw_rectangle((0, 50, 160, 140),color=(255, 0, 0))       # 根据色块blob位置画红色框
                img.draw_cross(80, 120,color=(0, 0, 255))  # 根据色块位置在中心画蓝色十字
            # These values are stable all the time.
            img.draw_rectangle(blob.rect())
            img.draw_cross(blob.cx(), blob.cy())
            # Note - the blob rotation is unique to 0-180 only.
            img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
            x_error = 2 * (blob.cx()/img.width()) - 1 - 0.2
            x_error_red = x_error
            y_up = blob.y()
            if blob.area() > 8200:
                x_error = 500

    #找紫色
    blobs3 = img.find_blobs([thresholds[2]], roi = [0, 50, 160, 140], pixels_threshold=100, area_threshold=100, merge=True)
    if blobs3:                  # 找到追踪目标
        blob = find_max(blobs3)  # 提取blobs中面积最大的一个色块blob
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
            img.draw_rectangle((0, 50, 160, 140),color=(255, 0, 0))       # 根据色块blob位置画红色框
            img.draw_cross(80, 120,color=(0, 0, 255))  # 根据色块位置在中心画蓝色十字
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
        x_error = 2 * (blob.cx()/img.width()) - 1 - 0.2
        x_error_red = x_error
        print(blob.area())
        y_up = blob.y()
        if blob.area() > 6000:
            x_error = 800
    uart.write(str(x_error) + '\n')                       # 将变量通过串口发送出
    time.sleep_ms(100)                          # 延时0.5秒
    if uart.any():                              # 如果接收到任何消息
        receive = uart.read().decode().strip()  # 将接收到的消息提取出来
```

### 自主创新项目二:小车踢球对战
在小车寻球踢球的基础上，我们进一步改进代码，使其更有**侵略性**，并**精调射门等关键时刻的转向、对准参数**，添加巡视一定时间后**打破巡视，探索周围**的逻辑，完成小车踢球对战代码

我们在处理过程中，偶尔会遇到摄像头卡死的问题，或者是摄像头未与单片机正常通讯，出现如缓慢通讯甚至不通讯的现象。对此我们尽可能地有所解决：**精细调节延迟，保证信息传输有时间处理**，**简化颜色识别代码**等

同时，我们有时候会遇到摄像头识别不及时，导致在自转巡视寻找球时，恰好摄像机由于延迟，看到的都是同一边视觉的问题。我们给出的解决方案是继续**精调延迟**，比如**设为质数**，保证代码执行时机或摄像头拍摄时机**互不耦合**，以解决这一问题。

对战展示视频如下

[自主创新项目二:小车踢球对战展示](https://cloud.tsinghua.edu.cn/f/0cc93458458a459880b7/)如下所示 也可通过链接观看
<center>

![自主创新项目二:小车踢球对战展示](./asset/自主创新项目二%20小车踢球对战.gif)
</center>

代码如下，同样分为 `main.c` 的单片机主控程序和 `ColorTracking.py` 的视觉模块程序

#### `main.c` 代码
```c
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

// 系统库导入
#include "include.h"

double x_error = 0;//色块偏移
char txt[32];
int duty1;//电机参数
int duty2;
int duty3;
double k;//历史x_error
int k1;//射门标志

int main(void)
{
    //-----------------------系统库初始化----------------------------
    HAL_Init();           // HAL初始化
    SystemClock_Config(); // 初始化系统时钟 72M
    delay_init(72);       // 延迟初始化
    JTAG_Set(SWD_ENABLE); // SWD初始化

    //-----------------------------------------------------------------
    uart_init(USART_3, 115200); // 蓝牙串口3初始化 建立与LQMV4的通讯
    uart_init(USART_2, 115200); // 蓝牙串口2初始化 可开启蓝牙通讯
    OLED_Init();
    LED_Init(); //初始化LED
    MotorInit();
    delay_ms(500); // 系统初始化
    printf("Long Qiu Ke Ji\n");
    printf("USART3 test,This is USART2.\n");
    MotorCtrl3w(0, -4600, 4000);
    delayms(100);
    //主循环 根据历史信息以及踢球标志位,控制状态,大部分同理踢球代码
    while (1)
    {
        uart_SendBuf(&USART3_Handler, (uint8_t *)"This is USART3!\n");
        OLED_P8x16Str(10, 0, "UART Test"); //测试信息打印
        k1 = 1;
        while (k1 == 1)
        {
            if (receive_oneline3 != 0)
            {
                k = x_error;//历史信息记录
                uart_SendBuf(&USART3_Handler, (uint8_t *)ReadBuff);
                x_error = atof((char *)ReadBuff);
                sprintf(txt, "%f", x_error);
                OLED_P8x16Str(0, 4, txt); //OLED打印信息
                LED_Ctrl(RVS);
                if (x_error < 1000)
                {
                    if (k > 1000)//转入找到球的状态,停车
                    {
                        MotorCtrl3w(-600, -600, -600);
                        delayms(100);
                    }
                    if (k == 100 && x_error != 100)
                    {
                        MotorCtrl3w(-2000, 0, 0);
                        delayms(100);
                    }
                    if (k == 200 && x_error != 200)
                    {
                        MotorCtrl3w(2000, 2000, 2000);
                        delayms(100);
                        x_error = 10000;
                    }
                    if (k <= 1 && k >= -1 && x_error == 100)
                    {
                        MotorCtrl3w(0, 0, 0);
                        delayms(100);
                    }
                    if (x_error < -5) // 射门后停止
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
                            duty1 = 800 * x_error + 800;
                            duty2 = -2350 + 1100 * x_error;
                            duty3 = 2300 - 1100 * x_error;
                            MotorCtrl3w(duty1, duty2, duty3);
                        }
                        if (x_error < 0 && x_error >= -1) // chao hong qiu zou
                        {
                            duty1 = 800 * x_error - 900;
                            duty2 = -2350 + 2500 * x_error;
                            duty3 = 2300 - 1200 * x_error;
                            MotorCtrl3w(duty1, duty2, duty3);
                        }

                        if (x_error >= 10 && x_error <= 11) // chao hong qiu zou
                        {
                            duty1 = 800 * (x_error - 10) + 700;
                            duty2 = -950 + 1100 * (x_error - 10);
                            duty3 = 900 - 1100 * (x_error - 10);
                            MotorCtrl3w(duty1, duty2, duty3);
                        }
                        if (x_error < 10 && x_error >= 9) // chao hong qiu zou
                        {
                            duty1 = 800 * (x_error - 10) - 800;
                            duty2 = -1050 + 2500 * (x_error - 10);
                            duty3 = 1000 - 1200 * (x_error - 10);
                            MotorCtrl3w(duty1, duty2, duty3);
                        }

                        if (x_error == 100) // rao qiu zhuan
                        {
                            duty1 = 1800;
                            duty2 = 0;
                            duty3 = -700;
                            MotorCtrl3w(duty1, duty2, duty3);
                        }
                        if (x_error == 300) // rao qiu zhuan  (ni shi zhen)
                        {
                            duty1 = -1500;
                            duty2 = 0;
                            duty3 = -600;
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
                            MotorCtrl3w(duty1, duty2, duty3);
                        }
                        if (x_error == 250)
                        {
                            duty1 = 0;
                            duty2 = -2660;
                            duty3 = 2500;
                            MotorCtrl3w(duty1, duty2, duty3);
                        }
                        if (x_error == 251)
                        {
                            k1 = 5;
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
                receive_oneline3 = 0; //清除帧标志位
            }
            if (k1 != 1)
            {
                break;
            }
        }
        if (k1 == 0)
        {
            MotorCtrl3w(-1600, 0, 0);
            delayms(165);
            duty1 = 0;
            duty2 = -4600;
            duty3 = 4000;
            MotorCtrl3w(duty1, duty2, duty3);
            delayms(1200);
            MotorCtrl3w(0, 0, 0);
            delayms(800);
        }

        if (k1 == 5)
        {
            duty1 = 0;
            duty2 = 2500;
            duty3 = -2600;
            MotorCtrl3w(duty1, duty2, duty3);
            delayms(500);
            duty1 = 1200;
            duty2 = 1200;
            duty3 = 1200;
            MotorCtrl3w(duty1, duty2, duty3);
            delayms(360);
        }
    }
}
```

#### `ColorTracking.py` 代码
```python
# This work is licensed under the MIT license.
# Copyright (c) 2013-2023 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# Single Color Code Tracking Example
#
# This example shows off single color code tracking using the OpenMV Cam.
#
# A color code is a blob composed of two or more colors. The example below will
# only track colored objects which have both the colors below in them.

import sensor
import time
import math
import display

import pyb
from pyb import UART,LED,Pin

LEDB = LED(3)                  # 蓝色LED

uart = UART(3, 115200)         # 初始化串口3，波特率115200 --》母板上 UART接口

# Color Tracking Thresholds (L Min, L Max, A Min, A Max, B Min, B Max)
# The below thresholds track in general red/green things. You may wish to tune them...
thresholds = [
    (
        22,
        100,
        41,
        127,
        -128,
        127,
    ),  # generic_red_thresholds -> index is 0 so code == (1 << 0)
    (40, 99, -74, 12, 42, 73),
]  # generic_green_thresholds -> index is 1 so code == (1 << 1)
# Codes are or'ed together when "merge=True" for "find_blobs".

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA2)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)  # must be turned off for color tracking
sensor.set_auto_whitebal(False)  # must be turned off for color tracking
lcd = display.SPIDisplay()

clock = time.clock()

# 最大色块寻找函数
def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob[2]*blob[3] > max_size:
            max_blob=blob
            max_size = blob[2]*blob[3]
    return max_blob

# Only blobs that with more pixels than "pixel_threshold" and more area than "area_threshold" are
# returned by "find_blobs" below. Change "pixels_threshold" and "area_threshold" if you change the
# camera resolution. "merge=True" must be set to merge overlapping color blobs for color codes.

while True:
    clock.tick()
    img = sensor.snapshot()
    x_error = 10000
    x_error_red = 10000
    #找红色
    blobs1 = img.find_blobs([thresholds[0]], pixels_threshold=0, area_threshold=0, merge=True)
    if blobs1:                  # 找到追踪目标
        blob = find_max(blobs1)  # 提取blobs中面积最大的一个色块blob
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
        x_error = 2 * (blob.cx()/img.width()) - 1
        x_error_red = x_error
        print(blob.cy())
        y_up = blob.y()
        if blob.cy() > 62:
            x_error = x_error + 10
        if blob.cy() > 115:
            x_error = 100
    #找绿色
    blobs2 = img.find_blobs([thresholds[1]], pixels_threshold=100, area_threshold=100, merge=True)
    if blobs2:                  # 找到追踪目标
        blob = find_max(blobs2)  # 提取blobs中面积最大的一个色块blob
        if blob.elongation() > 0.5:
            img.draw_edges(blob.min_corners(), color=(255, 0, 0))
            img.draw_line(blob.major_axis_line(), color=(0, 255, 0))
            img.draw_line(blob.minor_axis_line(), color=(0, 0, 255))
        # These values are stable all the time.
        img.draw_rectangle(blob.rect())
        img.draw_cross(blob.cx(), blob.cy())
        # Note - the blob rotation is unique to 0-180 only.
        img.draw_keypoints([(blob.cx(), blob.cy(), int(math.degrees(blob.rotation())))], size=20)
        x_error_green = 2 * (blob.cx()/img.width()) - 1
        print(blob.cy())
        print(blob.area())
        if x_error == 100 and x_error_green < 0 and abs(x_error_red - x_error_green) >= 0.13:
            x_error = 300 #公转方向为逆时针

        if abs(x_error_red - x_error_green) < 0.13 and blob.area() <= 4900:
            x_error = 200
        if abs(x_error_red - x_error_green) < 0.13 and blob.area() > 4900:
            x_error = 250
        if blob.cy() > 39 and blob.area() > 10000:
            x_error = 251
        y_down = blob.y() + blob.h()
    uart.write(str(x_error) + '\n')                       # 将变量通过串口发送出
    time.sleep_ms(97)                          # 延时 同时避免与小车自转或信息交互等耦合导致的信息传递卡顿
    if uart.any():                              # 如果接收到任何消息
        receive = uart.read().decode().strip()  # 将接收到的消息提取出来
```

## 附: 工作日志
> @import "Log.md"