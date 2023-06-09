#include <stc15.h>
#include "led8.h"
/************* 外部变量和本地变量声明 **************/
extern u8 LED8[8]; //显示缓冲，在 LED8.c 中声明
bit F_T0=0; //1ms 中断软标志
bit F_KS=0;
bit F_incdec=0,F_stop=0; //运行参数，初始为增量计数器，不停止
u8 Count=0; //unsigned char 与 s8,u16 在 led8.h 中定义
u8 miao=0;
u8 fen=0;
u8 a=2;
/**************** T0 中断初始化函数 ******************/
void T0_Init()
{
#define MAIN_Fosc 22118400L //定义主时钟
#define Timer0_Reload (65536UL -(MAIN_Fosc / 1000))
AUXR = 0x80; //Timer0 set as 1T, 16 bits timer auto-reload,
TH0 = (u8)(Timer0_Reload / 256);
TL0 = (u8)(Timer0_Reload % 256);
ET0 = 1; //T0 interrupt enable
PT0 = 1; //设置 T0 我高优先级
TR0 = 1; //启动 T0
}
/**************** INT0/INT1 中断初始化函数 ******************/
void INT_Init()
{
IT0 = 1; //INT0 下降沿中断
IT1 = 1;
IE1 = 0; //外中断 1 标志位
IE0 = 0; //外中断 0 标志位
EX1 = 1; //INT1 Enable
EX0 = 1; //INT0 Enable
EA = 1; //允许总中断
}
/********************** 端口初始化 ************************/
void Port_Init()
{
P0M1 = 0; P0M0 = 0; //设置为准双向口
P1M1 = 0; P1M0 = 0; //设置为准双向口
P2M1 = 0; P2M0 = 0; //设置为准双向口
P3M1 = 0; P3M0 = 0; //设置为准双向口
P4M1 = 0; P4M0 = 0; //设置为准双向口
P5M1 = 0; P5M0 = 0; //设置为准双向口
P6M1 = 0; P6M0 = 0; //设置为准双向口
P7M1 = 0; P7M0 = 0; //设置为准双向口
}
void delay_ms(u8 ms)
{
u16 i;
do{
i = MAIN_Fosc / 16000;
while(--i) ; //16T per loop
}while(--ms);
}
/********************* INT0 中断函数 *************************/
void INT0_int (void) interrupt 0 //进中断时已经清除标志
{
F_incdec^=1; //sw17 乒乓键，改变计数方向标志
delay_ms(20); //软件延时消抖动
}
/********************* INT1 中断函数 *************************/
void INT1_int (void) interrupt 2 //进中断时已经清除标志
{
F_stop^=1; //sw18 乒乓键，改变计数启停标志
	if(F_incdec)
		if(++a==8) a=2;
delay_ms(20); //软件延时消抖动
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
	F_T0 = 1; 
}			   
extern u8 KeyCode;    //给用户使用的键码, 1~16有效 
void Key_Init(void);
void IO_KeyScan(void);

/********************** 显示计数函数 ************************/
void DisplayCount(void)
{
LED8[2] = Count/10;
LED8[3] = Count%10+32;
LED8[4] = fen/10; 
LED8[5] = fen%10+32;
LED8[6] = miao/10; 
LED8[7] = miao%10;
}
/********************** 主函数 ************************/
void main(void)
{
u16 i=0;
Port_Init(); //初始化
T0_Init();
INT_Init();
for(i=0; i<8; i++) LED8[i] = DIS_BLACK; //上电消隐
while(1) {
if(F_T0) { //1ms 到
F_T0 = 0;
if(!F_incdec){
	
	if(++i >= 1000){//1s到
		i=0;
	  if(++miao==60){
			miao=0;
			if(++fen==60){
				fen=0;
				if(++Count==24) Count=0;
			  }
		  }
		}
	DisplayCount();
	F_KS=0;
	}
else{
	if(++i<=500)LED8[a]=0x10;
	else if(i<=1000)DisplayCount();//实现闪烁
	else i=0;
	if(F_KS) {		  		//50ms，扫描键盘
			IO_KeyScan();
			F_KS=0;
		}
	if(KeyCode > 0&&KeyCode<=10){
		switch(a){
			   case 2: if(Count%10+(KeyCode-1)*10<24) Count=Count%10+(KeyCode-1)*10;break;
				 case 3: if(Count/10*10+(KeyCode-1)<24) Count=Count/10*10+(KeyCode-1);break;
				 case 4: if(fen%10+(KeyCode-1)*10<60) fen=fen%10+(KeyCode-1)*10;break;
				 case 5: if(fen/10*10+(KeyCode-1)<60)fen=fen/10*10+(KeyCode-1);break;
				 case 6: if(miao%10+(KeyCode-1)*10<60) miao=miao%10+(KeyCode-1)*10;break;;break;
				 case 7: if(miao/10*10+(KeyCode-1)<60) miao=miao/10*10+(KeyCode-1);break;;break;
		     }
    KeyCode=0;		
	  }
}
}
}
}
						
					
