#include<STC15.H>
#define u8 unsigned char 
#define u16 unsigned 
#define MAIN_Fosc 22118400L //定义主时钟


void delay_ms(u8 ms)
{
	u16 i;
	do{
		i = MAIN_Fosc / 16000;
		while(--i);
	}while(--ms);
}

void setLED(u8 i)
{
	P17=P16=P47=P46=1;
	switch(i){
		case 0:P17 = 0;break;
		case 1:P16 = 0;break;
		case 2:P47 = 0;break;
		case 3:P46 = 0;break;	
	}
	delay_ms(250);
}


void main(void)
{
	u8 i;
	
	P1M1=0;P1M0=0;
	P4M1=0;P4M0=0;
	while (1)
		for (i=0;i<4;i++)
			setLED(i);
}