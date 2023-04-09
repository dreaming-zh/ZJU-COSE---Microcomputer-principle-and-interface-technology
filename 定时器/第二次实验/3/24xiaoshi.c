#include <stc15.h>
#include "led8.h"
/************* �ⲿ�����ͱ��ر������� **************/
extern u8 LED8[8]; //��ʾ���壬�� LED8.c ������
bit F_T0=0; //1ms �ж����־
bit F_incdec=0,F_stop=0; //���в�������ʼΪ��������������ֹͣ
u8 Count=0; //unsigned char �� s8,u16 �� led8.h �ж���
u8 miao=0;
u8 fen=0;
/**************** T0 �жϳ�ʼ������ ******************/
void T0_Init()
{
#define MAIN_Fosc 22118400L //������ʱ��
#define Timer0_Reload (65536UL -(MAIN_Fosc / 1000))
AUXR = 0x80; //Timer0 set as 1T, 16 bits timer auto-reload,
TH0 = (u8)(Timer0_Reload / 256);
TL0 = (u8)(Timer0_Reload % 256);
ET0 = 1; //T0 interrupt enable
PT0 = 1; //���� T0 �Ҹ����ȼ�
TR0 = 1; //���� T0
}
/**************** INT0/INT1 �жϳ�ʼ������ ******************/
void INT_Init()
{
IT0 = 1; //INT0 �½����ж�
	IE1 = 0; //���ж� 1 ��־λ
IE0 = 0; //���ж� 0 ��־λ
EX1 = 1; //INT1 Enable
EX0 = 1; //INT0 Enable
EA = 1; //�������ж�
}
/********************** �˿ڳ�ʼ�� ************************/
void Port_Init()
{
P0M1 = 0; P0M0 = 0; //����Ϊ׼˫���
P1M1 = 0; P1M0 = 0; //����Ϊ׼˫���
P2M1 = 0; P2M0 = 0; //����Ϊ׼˫���
P3M1 = 0; P3M0 = 0; //����Ϊ׼˫���
P4M1 = 0; P4M0 = 0; //����Ϊ׼˫���
P5M1 = 0; P5M0 = 0; //����Ϊ׼˫���
P6M1 = 0; P6M0 = 0; //����Ϊ׼˫���
P7M1 = 0; P7M0 = 0; //����Ϊ׼˫���
}
void delay_ms(u8 ms)
{
u16 i;
do{
i = MAIN_Fosc / 16000;
while(--i) ; //16T per loop
}while(--ms);
}
/********************* INT0 �жϺ��� *************************/
void INT0_int (void) interrupt 0 //���ж�ʱ�Ѿ������־
{
F_incdec^=1; //sw17 ƹ�Ҽ����ı���������־
delay_ms(20); //�����ʱ������
}
/********************* INT1 �жϺ��� *************************/
void INT1_int (void) interrupt 2 //���ж�ʱ�Ѿ������־
{
F_stop^=1; //sw18 ƹ�Ҽ����ı������ͣ��־
delay_ms(20); //�����ʱ������
}
/********************** T0 1ms �жϺ��� ************************/
void T0_int (void) interrupt 1
{
DisplayScan(); //1ms ɨ����ʾһλ
F_T0 = 1; //1ms ʱ�䵽��־
}
/********************** ��ʾ�������� ************************/
void DisplayCount(void)
{
LED8[2] = Count/10;
LED8[3] = Count%10+32;
LED8[4] = fen/10; 
LED8[5] = fen%10+32;
LED8[6] = miao/10; 
LED8[7] = miao%10;
}
/********************** ������ ************************/
void main(void)
{
u16 i=0;
Port_Init(); //��ʼ��
T0_Init();
INT_Init();
for(i=0; i<8; i++) LED8[i] = DIS_BLACK; //�ϵ�����
while(1) {
if(F_T0) { //1ms ��
F_T0 = 0;
if(!F_stop)
if(++i >= 1000) { //1 �뵽
i=0;
if(!F_incdec){ //���ݼ���������¼���
if(++miao==60) {miao=0;fen++;if(++fen==60){fen=0;Count++;}}
} else {
if(miao==0&&fen==0&&Count==0) {Count=23;fen=59;miao=59;}
else if(miao==0&&fen==0){Count--;fen=59;miao=59;}
else if(miao==0){fen--;miao=59;}
else miao--;
}

}
DisplayCount(); //������ʾ������
}
}
}