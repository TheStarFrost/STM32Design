#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//********************************************************************************

#define USART_REC_LEN  			200  		//�����������ֽ��� 200
#define EN_USART3_RX 			1			//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; 	//���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         			//����״̬���	
extern UART_HandleTypeDef UART3_Handler; 	//UART���
extern UART_HandleTypeDef USART2_Handler; //USART2���
extern UART_HandleTypeDef USART3_Handler; //USART3���
#define RXBUFFERSIZE   1 					//�����С
extern u8 aRxBuffer[RXBUFFERSIZE];			//HAL��USART����Buffer

extern u8 ReadBuff[1024];
extern u16 num2, num3;
extern u8 receive_oneline2;
extern u8 receive_oneline3;





typedef enum		// ö�ٴ��ں�
{
  USART_2 = 2,
	USART_3,
}USART_NUM;


void uart_SendBuf(UART_HandleTypeDef *huart, uint8_t *pData);
void uart_init(USART_NUM usart,u32 bound);
void Test_Usart(void);
void BlueTooth_cont(void);

#endif


