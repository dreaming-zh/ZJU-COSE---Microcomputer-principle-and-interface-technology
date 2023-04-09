#include <stc15.h>
#include <intrins.h>
#include <string.h>
#include "led8.h"

#define StrLen strlen(disp_string)

/*************  �ⲿ�����ͱ��ر�������    **************/	 
extern u8  LED8[8];        	//��ʾ����

bit F_T0=0;    			   	//1ms�ж����־
bit F_L=0, F_R=0;   		//F_L,F_R���Ҽ���� 
u8  disp_index=0, F_Dir=1; 	//F_Dir�����ǣ�0���� 1 ���� 2����

u8 code disp_string[]="3200104869 ";

/**************** T0�жϳ�ʼ������ ******************/
void T0_Init()
{
	#define     MAIN_Fosc       22118400L   //������ʱ��
	#define     Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	PT0 = 1;
    TR0 = 1;    //Tiner0 run
}    

/**************** INT0/INT1�жϳ�ʼ������ ******************/
void INT_Init()
{
	IT0 = 1;    //INT0 �½����ж�        
	IT1 = 1;    //INT1 �½����ж�
	IE1  = 0;   //���ж�1��־λ
	IE0  = 0;   //���ж�0��־λ

	EX1 = 1;    //INT1 Enable
	EX0 = 1;    //INT0 Enable 	        
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

void Delay20ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_(); _nop_();
	i = 2;
	j = 175;
	k = 75;
	do 	{  
		do	{
			while (--k);
		} while (--j);
	} while (--i);
}

/********************* INT0�жϺ��� *************************/
void INT0_int (void) interrupt 0      //���ж�ʱ�Ѿ������־
{
    F_L^=1; //ƹ�Ҽ�
	Delay20ms();
}

/********************* INT1�жϺ��� *************************/
void INT1_int (void) interrupt 2      //���ж�ʱ�Ѿ������־
{
    F_R^=1; 	//ƹ�Ҽ�
	Delay20ms(); //�����ʱ������
}

/********************** T0 1ms�жϺ��� ************************/
void T0_int (void) interrupt 1
{
    DisplayScan();  //1msɨ����ʾһλ
    F_T0 = 1;      //1ms��־	  
}			   

u8 ASC2num(u8 asc)
{
	if(asc<='9'&&asc>='0')
		return asc-'0';
	else return DIS_BLACK;
}

/********************** ��ʾ�������� ************************/
void ReflashDisp(void)
{
   	u8 i;

	for(i=0;i<8;i++)
		LED8[i] = ASC2num(disp_string[(disp_index+i)%StrLen]);
}	 

/********************** ������ ************************/
void main(void)
{
	int i;

	Port_Init();
	T0_Init();
	INT_Init();    
   
    ReflashDisp();    //��ʾѧ��
    while(1) {
		if(F_L) {
			switch(F_Dir) {
				case 0: F_Dir=1;break;
				case 1:	F_Dir=0; break;
				case 2: F_Dir=0;break;
			}
			F_L=0;
		}
		if(F_R) {
			switch(F_Dir) {
				case 0: F_Dir=2; break;
				case 1:	F_Dir=2; break;
				case 2: F_Dir=1;break;
			}
			F_R=0;
		}
 
		if(F_T0) {
		   F_T0=0;
			if(++i >= 1000) {  //1�뵽	 				            
				i=0;
				disp_index=(disp_index-F_Dir+1+StrLen)%StrLen;
				ReflashDisp();	 
			}
		}
    }
} 