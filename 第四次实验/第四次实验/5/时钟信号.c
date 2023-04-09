#include <stc15.h>
#include <stdio.h>
#include <string.h>
#include "gdef.h"
#include "led8.h"
#include "serial.h"

/*************  �ⲿ�����ͱ��ر�������    **************/	 
extern u8  LED8[8];        	//��ʾ����
bit F_500=0;  	//500ms�ж����־


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
void T0_int (void) interrupt 1
{
	static u16 Count500=500;

    DisplayScan();  //1msɨ����ʾһλ
	if(--Count500==0) {
		F_500=1;
		Count500=500;
	}
}			   

u8  hour,minute,second; //RTC����
/********************** RTC��ʾ���� ************************/
void    RTC(void)
{
    if(++second >= 60)
    {
        second = 0;
        if(++minute >= 60)
        {
            minute = 0;
            if(++hour >= 24)    hour = 0;
        }
    }
}

u8 code T_ModiTable[6]={0,1,3,4,6,7};
/********************** ��ʾʱ�Ӻ��� ************************/
u8 GetChar(u8 position)
{
	switch(position) {
		case 0: return hour / 10; 
		case 1: return hour % 10;  
		case 2: return minute / 10;
		case 3: return minute % 10;
		case 4: return second / 10;
		case 5: return second % 10;
		default:return DIS_BLACK;
	}	  
}		

void    DisplayLED(u8 position, bit blit)
{
	LED8[T_ModiTable[position]]=blit?GetChar(position):DIS_BLACK;
}

void    DisplayRTC(void)
{
	u8 i;
	for(i=0;i<6;i++)
		DisplayLED(i,1);
	LED8[2] = DIS_DASH; //����
	LED8[5] = DIS_DASH;
}

void FlashRTC(void)
{	
	LED8[2] = DIS_BLACK; //Ϩ��
	LED8[5] = DIS_BLACK;   	 
}

void RunRTC(void)
{
	static bit onesecond=0;

	if(F_500) {  //500���뵽	 				            
		F_500=0;
		if(!onesecond) {
			FlashRTC();   //ʱ���м�������
			onesecond^=1;
		} else {
	    	RTC();		  //2��500ms,����1s��
			DisplayRTC();
	   		onesecond=0;
			printf("��ǰʱ�䣺%bd:%bd:%bd\n",hour,minute,second);
		}
	}
}

enum { HH,MM,SS };
bit interpret(u8 *hour,u8 *minute,u8 *second)
{
	u8 xdata string[20],xdata *sptr,hhmmss=HH, tmpH, tmpM, tmpS;	
 	u16 dtype,dvalue,dlenth;
	bit valid=0,endline;

	while(RXready()) {
		scanf("%s",string);   	
		if(strstr(string,"����ʱ��")) valid=1; break;
	}	  
	if(!valid) return 0;	 
	
	while(RXready()) {
		scanf("%s",string); sptr=string;
    	for(endline=0;!endline;) {
	        dtype=scanf(sptr,"%d%n",&dvalue,&dlenth);
			switch(dtype) {
		   		case 0: sptr++; break;
				case 1: sptr+=dlenth;
						switch(hhmmss) {
							case HH:if(HH<24) tmpH=dvalue; else valid=0; break;
							case MM:if(MM<60) tmpM=dvalue; else valid=0; break;
							case SS:if(SS<60) tmpS=dvalue; else valid=0; break;
							default: break;
						}
						hhmmss++; 
						break; 
				default:endline=1;
	        }
		}
    }
	if(valid) 
		*hour=tmpH, *minute=tmpM, *second=tmpS;
	return valid;
}	 
//	valid=1;
//	for(;!endline;) {
//        dtype=scanf("%d%n",&dvalue,&dlenth);
//		switch(dtype) {
//	   		case 0: sptr++; break;
//			case 1: printf("%d\n",dvalue); 	 					
//					sptr+=dlenth;
//					switch(hhmmss) {
//						case HH:if(HH<24) *hour=dvalue; else valid=0; break;
//						case MM:if(MM<60) *minute=dvalue; else valid=0; break;
//						case SS:if(SS<60) *second=dvalue; else valid=0; break;;
//					}
//					hhmmss++; 
//					break; 
//			default:endline=1;
//        }
//    }
	
void ModifyRTC(void)
{
	if(!RXready()) return;	
	if(!interpret(&hour,&minute,&second)) {	 
		printf("�����ʽӦΪ\"����ʱ�䣺HH:MM:SS\"\n");
		printf("ע��\"����ʱ��\"��ʱ������֮����Ҫ�м����\n");	
		return;
	} 	
	F_500=0; 		//��ֹ�޸ĺ������ϱ仯
	DisplayRTC();
}


/********************** ������ ************************/
void main(void)
{
	Port_Init();
	T0_Init();
	INT_Init();
	Uart_Init(38400);

    hour   = 0;    //��ʼ��ʱ��ֵ
    minute = 0;
    second = 0;
    DisplayRTC();  

    while(1) {
		RunRTC();
		ModifyRTC();
	}
} 

