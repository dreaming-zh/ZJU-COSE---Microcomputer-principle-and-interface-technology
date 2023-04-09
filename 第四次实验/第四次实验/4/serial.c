#include <stc15.h>
#include "gdef.h"
#include "serial.h"

/**************** T1��ʱ����ʼ������ ******************/
void T1_Init(u16 brt)
{
	#define Timer_Reload   (65536UL - MAIN_Fosc/4/brt) //Timer1 �����ʷ�����

	AUXR |=  (1<<6);	//T1x12=1: Timer1 1T, 16 bits timer auto-reload													 
	TH1 = (u8)(Timer_Reload / 256);
	TL1 = (u8)(Timer_Reload % 256);
    TR1 = 1;    //Timer1 run
}  

void Uart_Init(u16 brt)		//19200bps@22.1184MHz
{
	SCON = (SCON & 0x3f) | 0x40;	//UART1ģʽ, SM0 SM1=01: 8λ����,�ɱ䲨����
	REN = 1;	//�������
	AUXR &= ~0x01;		//S1 BRT Use Timer1;
	T1_Init(brt);
}

#define		UART_BUF_LENGTH	50	//���ڻ��峤��
u8	RX_Ptr=0,RX_Get=0;	//���ռ���
bit	TX_Busy;	  
u8 	xdata 	RX_Buffer[UART_BUF_LENGTH];	//���ջ���	 

void S1_int (void) interrupt UART1_VECTOR
{
	if(RI)
	{
		RI = 0;
		if((RX_Ptr+1)%UART_BUF_LENGTH==RX_Get) return;	// ������
		RX_Ptr=(RX_Ptr+1)%UART_BUF_LENGTH;
		RX_Buffer[RX_Ptr] = SBUF;
	}

	if(TI)
	{
		TI = 0;
		TX_Busy = 0;
	}
}

s8 putchar(s8 ch)
{ 	
	if(ch=='\n') {  //���������������⣬�����˳���ܷ�
		SBUF = '\r';
		TX_Busy = 1;
		while(TX_Busy);
	}
	SBUF = ch;
	TX_Busy = 1;
	while(TX_Busy);  
		
	return ch;		   //�����Ѿ����͵�����
}

s8 _getkey(void)   
{ 
	u8 ch;
	
	while(RX_Ptr == RX_Get);  //����գ��ȵ����ݽ���
	ES = 0;
	ch = RX_Buffer[RX_Get]; 
	RX_Get = (RX_Get + 1) % UART_BUF_LENGTH;
	ES = 1;
	return ch;
}

bit RXready(void)
{
	return (RX_Ptr != RX_Get) && (RX_Buffer[RX_Ptr]=='\n');
}