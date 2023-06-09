/*************  本程序功能说明  **************
显示效果为: 24小时时钟 HH-MM-SS
工作模式：正常走时方式和修改时钟方式
中断键，按SW17修改，按SW18切换当前位
修改方式下，当前位闪烁，自动切换到位
退出修改后，重新进入修改，修改位记存
******************************************/	
#include <stc15.h>
#include "led8.h"

/*************  外部变量和本地变量声明    **************/	 
extern u8  LED8[8];        	//显示缓冲
bit F_KS=0;

/**************** T0中断初始化函数 ******************/
void T0_Init()
{
	#define     MAIN_Fosc       22118400L   //定义主时钟
	#define     Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	PT0 = 1;
    TR0 = 1;    //Tiner0 run        
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
	static u8 Count50=50;

    DisplayScan();  //1ms扫描显示一位
	if(--Count50==0) {
		F_KS=1;
		Count50	= 50;
	}  
}			   

extern u8 KeyCode;    //给用户使用的键码, 1~16有效 
void Key_Init(void);
void IO_KeyScan(void);

/********************** 主函数 ************************/
void main(void)
{
	u8 i;

	Port_Init();
	T0_Init();
	Key_Init();  

	for(i=0;i<8;i++) LED8[i] = DIS_BLACK; 	 //初始熄灭
    while(1) {
		if(F_KS) {		  		//50ms，扫描键盘
			IO_KeyScan();
			F_KS=0;
		}
	
	    if(KeyCode > 0) {     	//有键按下，处理键
			LED8[7] = KeyCode-1; 
			KeyCode=0;
		}
	}
} 