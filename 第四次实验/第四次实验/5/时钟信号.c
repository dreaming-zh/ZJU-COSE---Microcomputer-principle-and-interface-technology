#include <stc15.h>
#include <stdio.h>
#include <string.h>
#include "gdef.h"
#include "led8.h"
#include "serial.h"

/*************  外部变量和本地变量声明    **************/	 
extern u8  LED8[8];        	//显示缓冲
bit F_500=0;  	//500ms中断软标志


/**************** T0定时器初始化函数 ******************/
void T0_Init()
{
	#define     Timer_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer0 中断频率, 1000次/秒

    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload
    TH0 = (u8)(Timer_Reload / 256);
    TL0 = (u8)(Timer_Reload % 256);
    TR0 = 1;    //Timer0 run
}   

/**************** 中断初始化函数 ******************/
void INT_Init()
{
	ET0 = 1;    //Timer0 interrupt enable
	PT0 = 1;
	ET1 = 0;	//Tim1仅当作波特率发生器
    ES = 1;   
	EA = 1;     //允许总中断
}

/********************** 端口初始化 ************************/
void Port_Init()
{
    P0M1 = 0;   P0M0 = 0;   //设置为准双向口
    P1M1 = 0;   P1M0 = 0;   //设置为准双向口
    P2M1 = 0;   P2M0 = 0;   //设置为准双向口
    P3M1 = 0;   P3M0 = 0;   //设置为准双向口
    P4M1 = 0;   P4M0 = 0;   //设置为准双向口
    P5M1 = 0;   P5M0 = 0;   //设置为准双向口
    P6M1 = 0;   P6M0 = 0;   //设置为准双向口
    P7M1 = 0;   P7M0 = 0;   //设置为准双向口
}

/********************** T0 1ms中断函数 ************************/
void T0_int (void) interrupt 1
{
	static u16 Count500=500;

    DisplayScan();  //1ms扫描显示一位
	if(--Count500==0) {
		F_500=1;
		Count500=500;
	}
}			   

u8  hour,minute,second; //RTC变量
/********************** RTC演示函数 ************************/
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
/********************** 显示时钟函数 ************************/
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
	LED8[2] = DIS_DASH; //点亮
	LED8[5] = DIS_DASH;
}

void FlashRTC(void)
{	
	LED8[2] = DIS_BLACK; //熄灭
	LED8[5] = DIS_BLACK;   	 
}

void RunRTC(void)
{
	static bit onesecond=0;

	if(F_500) {  //500毫秒到	 				            
		F_500=0;
		if(!onesecond) {
			FlashRTC();   //时间中间横杠秒闪
			onesecond^=1;
		} else {
	    	RTC();		  //2个500ms,代表1s到
			DisplayRTC();
	   		onesecond=0;
			printf("当前时间：%bd:%bd:%bd\n",hour,minute,second);
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
		if(strstr(string,"设置时间")) valid=1; break;
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
		printf("输入格式应为\"设置时间：HH:MM:SS\"\n");
		printf("注意\"设置时间\"与时间数据之间需要有间隔符\n");	
		return;
	} 	
	F_500=0; 		//防止修改后秒马上变化
	DisplayRTC();
}


/********************** 主函数 ************************/
void main(void)
{
	Port_Init();
	T0_Init();
	INT_Init();
	Uart_Init(38400);

    hour   = 0;    //初始化时间值
    minute = 0;
    second = 0;
    DisplayRTC();  

    while(1) {
		RunRTC();
		ModifyRTC();
	}
} 

