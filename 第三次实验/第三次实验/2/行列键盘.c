/*************  ��������˵��  **************
��ʾЧ��Ϊ: 24Сʱʱ�� HH-MM-SS
����ģʽ��������ʱ��ʽ���޸�ʱ�ӷ�ʽ
�жϼ�����SW17�޸ģ���SW18�л���ǰλ
�޸ķ�ʽ�£���ǰλ��˸���Զ��л���λ
�˳��޸ĺ����½����޸ģ��޸�λ�Ǵ�
******************************************/	
#include <stc15.h>
#include "led8.h"

/*************  �ⲿ�����ͱ��ر�������    **************/	 
extern u8  LED8[8];        	//��ʾ����
bit F_KS=0;

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
void T0_int (void) interrupt 1
{
	static u8 Count50=50;

    DisplayScan();  //1msɨ����ʾһλ
	if(--Count50==0) {
		F_KS=1;
		Count50	= 50;
	}  
}			   

extern u8 KeyCode;    //���û�ʹ�õļ���, 1~16��Ч 
void Key_Init(void);
void IO_KeyScan(void);

/********************** ������ ************************/
void main(void)
{
	u8 i;

	Port_Init();
	T0_Init();
	Key_Init();  

	for(i=0;i<8;i++) LED8[i] = DIS_BLACK; 	 //��ʼϨ��
    while(1) {
		if(F_KS) {		  		//50ms��ɨ�����
			IO_KeyScan();
			F_KS=0;
		}
	
	    if(KeyCode > 0) {     	//�м����£������
			LED8[7] = KeyCode-1; 
			KeyCode=0;
		}
	}
} 