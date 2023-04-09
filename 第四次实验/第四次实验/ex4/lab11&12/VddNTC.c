#include <stc15.h>
#include <intrins.h>
#include <string.h>
#include <stdio.h>
#include "gdef.h"
#include "led8.h"
#include "adc.h"
#include "serial.h"

#define StrLen strlen(disp_string)

/*************  �ⲿ�����ͱ��ر�������    **************/	 
extern u8  LED8[8];        	//��ʾ����

bit F_T0=0;    	//1ms�ж����־
u8  disp_index=0, F_Dir=1; 	//F_Dir�����ǣ�0���� 1 ���� 2����

/**************** T0��ʱ����ʼ������ ******************/
void T0_Init()
{
	#define     Timer_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer0 �ж�Ƶ��, 1000��/��

    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload
    TH0 = (u8)(Timer_Reload / 256);
    TL0 = (u8)(Timer_Reload % 256);
    TR0 = 1;    //Timer0 run
}   

/**************** �жϳ�ʼ������ ******************/
void INT_Init()
{
	ET0 = 1;    //Timer0 interrupt enable
	PT0 = 1;
	ET1 = 0;	//Tim1�����������ʷ�����
    ES = 1;   
	EA = 1;     //�������ж�
}	

/********************** �˿ڳ�ʼ�� ************************/
void Port_Init()
{
    P0M1 = 0;   P0M0 = 0;   //����Ϊ׼˫���
    P1M1 = 0;   P1M0 = 0;   //����Ϊ׼˫���
    P2M1 = 0;   P2M0 = 0;   //����Ϊ׼˫���
    P3M1 = 0;   P3M0 = 0;   //����Ϊ׼˫���
    P4M1 = 0;   P4M0 = 0;   //����Ϊ׼˫���
    P5M1 = 0;   P5M0 = 0;   //����Ϊ׼˫���
    P6M1 = 0;   P6M0 = 0;   //����Ϊ׼˫���
    P7M1 = 0;   P7M0 = 0;   //����Ϊ׼˫���
}

/********************** T0 1ms�жϺ��� ************************/
void T0_int (void) interrupt TIMER0_VECTOR
{
    DisplayScan();  //1msɨ����ʾһλ
    F_T0 = 1;      //1ms��־	  
}	

void DispClr()
{ 
	u8 i;

	for(i=0; i<8; i++)  LED8[i] = DIS_BLACK; //�ϵ�����
}

u16 DispVdd()
{
	u8 i;
	u16 j;

	for(j=0,i=0; i<16; i++)   j += Get_ADC10bitResult(8); //�ڲ���׼ADC, P1ASF=0, ��0ͨ��
    j = (u32)128000UL*16 / j;  // ADC = 1024 * Uref / Ux, �� Ux = 1024 * Uref / ADC = 1024 * 1.25 / ADC = 1280 / ADC

    LED8[0] = j / 100 + DIS_DOT;    //��ʾMCU��ѹֵ, ����ʱ�Ŵ���100��, ��ѹ����λС��.
    LED8[1] = (j % 100) / 10;
    LED8[2] = j % 10;	

	return j;
}	   

u16 DispNTC()
{
	u8 i;
	u16 j;

	for(j=0,i=0; i<4; i++) j += Get_ADC10bitResult(3);  //ͨ��3��NTC	
    j = get_temperature(j); //�����¶�ֵ

    if(j >= 400)    F0 = 0, j -= 400;       //�¶� >= 0��
    else            F0 = 1, j  = 400 - j;   //�¶� <  0��
    LED8[4] = j / 1000;     //��ʾ�¶�ֵ
    LED8[5] = (j % 1000) / 100;
    LED8[6] = (j % 100) / 10 + DIS_DOT;
    LED8[7] = j % 10;

    if(LED8[4] == 0) 	 //������3λ����2λ�¶�
		if(LED8[5] == 0)  {	 			
	 	   	LED8[4] = LED8[5] = DIS_BLACK;
			if(F0)  LED8[5] = DIS_;     //��ʾ- 
		} else {
		 	LED8[4] = DIS_BLACK;
			if(F0)  LED8[4] = DIS_;     //��ʾ-
		}

	return j;
}	  


/********************** ������ ************************/
void main(void)
{
	static u16 msecond=0;
	static bit NTC=0;

	Port_Init();
	T0_Init();
	INT_Init();    
	ADC_Init();
	Uart_Init(38400);
   
	DispClr();
    while(1) {
		if(F_T0) {  //1ms��
 			F_T0 = 0;
            if(++msecond >= 500) {   //500ms��һ������
				msecond = 0;  
				if(!NTC) {	 					 
					//printf("Vdd:%3.2f\t",DispVdd()/100.0);
					printf("%3.2f",DispVdd()/100.0);
					P1ASF = 0x01<<3;       //����P1.3����ADCͨ��3��NTC���ӵ�ADC3
				}  else {
					//printf("NTC:%4.1f\n",DispNTC()/10.0);
					printf("%4.1f",DispNTC()/10.0);
					P1ASF = 0;				//׼��P1���ڵ�9ͨ�������ڲ�BGV
				}
				NTC^=1;
            }  
        }
    }
} 

