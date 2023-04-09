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

void LED_G(void)
{
	u8 j;
	P17 = 0;
	for (j=0;j<15;j++)
	{
		delay_ms(200);
	}
}

void shining(void)
{
	u8 j;
	for (j=0;j<4;j++)
	{
		P17 = 1;
		delay_ms (100);
		P17 = 0;
		delay_ms (100);
	}
	P17=1;
}

void LED_Y(void)
{
	u8 j;
	P16 =P47= 0;
	for (j=0;j<10;j++)
	{
		delay_ms(200);
	}
	P16 =P47= 1;
}

void LED_R(void)
{
	u8 j;
	P46 = 0;
	for (j=0;j<12;j++)
	{
		delay_ms(200);
	}
	P46 = 1;
}

void main(void)
{
	
	P1M1=0;P1M0=0;
	P4M1=0;P4M0=0;
	P17=P16=P47=P46=1;
	while (1)
	{
		LED_G();
		delay_ms(100);
		shining();
		delay_ms(100);
		LED_Y();
		delay_ms(100);
		LED_R();
	}
}


