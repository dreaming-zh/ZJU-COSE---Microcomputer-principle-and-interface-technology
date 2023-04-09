#include <stc15.h>
#include "gdef.h"
#include "led8.h"

/*************  ��ʾ����λ�����ݱ�    **************/
u8 code t_display[]={                       //8�������ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
// black  -    H    J    K    L    N    o    P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
//   0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -1
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};   

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��


/*************  IO�ڶ���    **************/
sbit    P_HC595_SER   = P4^0;   //pin 14    SER     data input
sbit    P_HC595_RCLK  = P5^4;   //pin 12    RCLk    store (latch) clock
sbit    P_HC595_SRCLK = P4^3;   //pin 11    SRCLK   Shift data clock

/*************  ���ر�������    **************/
u8  LED8[8];        //��ʾ����
static u8  display_index=0;  //��ʾλ����

/**************** ��HC595����һ���ֽں��� ******************/
void Send_595(u8 dat)
{       
    u8  i;
    for(i=0; i<8; i++)
    {
        dat <<= 1;
        P_HC595_SER   = CY;
        P_HC595_SRCLK = 1;
        P_HC595_SRCLK = 0;
    }
}

/********************** ��ʾɨ�躯�� ************************/
void DisplayScan(void)
{   
    Send_595(~T_COM[display_index]);                //���λ��
    Send_595(t_display[LED8[display_index]]);   //�������

    P_HC595_RCLK = 1;
    P_HC595_RCLK = 0;                           //�����������
    if(++display_index >= 8)    display_index = 0;  //8λ������0
}


