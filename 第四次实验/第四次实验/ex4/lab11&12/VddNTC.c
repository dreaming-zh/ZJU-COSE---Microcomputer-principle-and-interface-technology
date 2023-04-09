#include <stc15.h>
#include <intrins.h>
#include <string.h>
#include <stdio.h>
#include "gdef.h"
#include "led8.h"
#include "adc.h"
#include "serial.h"

#define StrLen strlen(disp_string)

/*************  外部变量和本地变量声明    **************/	 
extern u8  LED8[8];        	//显示缓冲

bit F_T0=0;    	//1ms中断软标志
u8  disp_index=0, F_Dir=1; 	//F_Dir方向标记，0左移 1 不动 2右移

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
void T0_int (void) interrupt TIMER0_VECTOR
{
    DisplayScan();  //1ms扫描显示一位
    F_T0 = 1;      //1ms标志	  
}	

void DispClr()
{ 
	u8 i;

	for(i=0; i<8; i++)  LED8[i] = DIS_BLACK; //上电消隐
}

u16 DispVdd()
{
	u8 i;
	u16 j;

	for(j=0,i=0; i<16; i++)   j += Get_ADC10bitResult(8); //内部基准ADC, P1ASF=0, 读0通道
    j = (u32)128000UL*16 / j;  // ADC = 1024 * Uref / Ux, 则 Ux = 1024 * Uref / ADC = 1024 * 1.25 / ADC = 1280 / ADC

    LED8[0] = j / 100 + DIS_DOT;    //显示MCU电压值, 计算时放大了100倍, 电压有两位小数.
    LED8[1] = (j % 100) / 10;
    LED8[2] = j % 10;	

	return j;
}	   

u16 DispNTC()
{
	u8 i;
	u16 j;

	for(j=0,i=0; i<4; i++) j += Get_ADC10bitResult(3);  //通道3，NTC	
    j = get_temperature(j); //计算温度值

    if(j >= 400)    F0 = 0, j -= 400;       //温度 >= 0度
    else            F0 = 1, j  = 400 - j;   //温度 <  0度
    LED8[4] = j / 1000;     //显示温度值
    LED8[5] = (j % 1000) / 100;
    LED8[6] = (j % 100) / 10 + DIS_DOT;
    LED8[7] = j % 10;

    if(LED8[4] == 0) 	 //可能是3位或者2位温度
		if(LED8[5] == 0)  {	 			
	 	   	LED8[4] = LED8[5] = DIS_BLACK;
			if(F0)  LED8[5] = DIS_;     //显示- 
		} else {
		 	LED8[4] = DIS_BLACK;
			if(F0)  LED8[4] = DIS_;     //显示-
		}

	return j;
}	  


/********************** 主函数 ************************/
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
		if(F_T0) {  //1ms到
 			F_T0 = 0;
            if(++msecond >= 500) {   //500ms测一个数据
				msecond = 0;  
				if(!NTC) {	 					 
					//printf("Vdd:%3.2f\t",DispVdd()/100.0);
					printf("%3.2f",DispVdd()/100.0);
					P1ASF = 0x01<<3;       //设置P1.3用于ADC通道3，NTC连接到ADC3
				}  else {
					//printf("NTC:%4.1f\n",DispNTC()/10.0);
					printf("%4.1f",DispNTC()/10.0);
					P1ASF = 0;				//准备P1用于第9通道测试内部BGV
				}
				NTC^=1;
            }  
        }
    }
} 

