#include <stc15.h>
#include <stdio.h>
#include <string.h>
#include "gdef.h"
#include "led8.h"
#include "serial.h"

/*************  外部变量和本地变量声明    **************/	 
extern u8  LED8[8];        	//显示缓冲
bit F_500=0;  	//500ms中断软标志
int a[10]={3,2,0,0,1,0,4,8,6,9};

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

u8  hour,minute; //RTC变量
void xuehao(int j)
{
LED8[0] = a[(j)%10];
LED8[1] = a[(j+1)%10];
LED8[2] = a[(j+2)%10];
LED8[3] = a[(j+3)%10];
LED8[4] = a[(j+4)%10];
LED8[5] = a[(j+5)%10];
LED8[6] = a[(j+6)%10];
LED8[7] = a[(j+7)%10];	
}

enum {HH,MM};
bit interpret(u8 *hour,u8 *minute)
{
	u8 xdata string[20],xdata *sptr,hhmm=HH, tmpH, tmpM;	
 	u16 dtype,dvalue,dlenth;
	bit valid=0,endline;

	while(RXready()) {
		scanf("%s",string);   	
		if(strstr(string,"set")) valid=1; break;
	}	  
	if(!valid) return 0;	 
	
	while(RXready()) {
		scanf("%s",string); sptr=string;
    	for(endline=0;!endline;) {
	        dtype=sscanf(sptr,"%d%n",&dvalue,&dlenth);
			switch(dtype) {
		   		case 0: sptr++; break;
				  case 1: sptr+=dlenth; sptr++;
						switch(hhmm) {
							case HH:if(dvalue==0||dvalue==1) tmpH=dvalue; else valid=0; break;
							case MM:if(dvalue==0||dvalue==1||dvalue==2) tmpM=dvalue; else valid=0; break;
					default: break;
						}
						hhmm++; 
						break; 	
					default:endline=1;
	        }
		}
    }
	if(valid){
		*hour=tmpH, *minute=tmpM;
		} 
		
	return valid;
}	 
void RunRTC(void)
{
	int i,j,k;
	interpret(&hour,&minute);
	if(F_500) 
	{			            
		F_500=0;i=(i+1)%3;k=(k+1)%2;
		if(hour==1)
		{
			if(minute==0)	{j=(j+1)%10;xuehao(j);}
			else if(minute==1)	{if(k==0){j=(j+1)%10;xuehao(j);}}
			else if(minute==2)	{if(i==0){j=(j+1)%10;xuehao(j);}}
		}
		else if(hour==0)
		{
			if(minute==0)	{j=(j+9)%10;xuehao(j);}
			else if(minute==1)	{if(k==0){j=(j+9)%10;xuehao(j);}}
			else if(minute==2)	{if(i==0){j=(j+9)%10;xuehao(j);}}
		}
		printf("方向，速度:%bd，%bd\n",hour,minute);
	}
		
}


/********************** 主函数 ************************/
void main(void)
{
	Port_Init();
	T0_Init();
	INT_Init();
	Uart_Init(38400);

    hour   = 1;    //初始化时间值
    minute = 0;
    while(1) {
			RunRTC();
		}
} 

