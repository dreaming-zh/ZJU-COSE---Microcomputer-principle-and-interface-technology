/*************  ��������˵��  **************
��ת����ȡ���룬�����KeyCode��
******************************************/
#include <stc15.h>
#include "led8.h"

u8 KeyCode;    //���û�ʹ�õļ���, 1~16��Ч
u8 IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;    //���м��̱���

/**********************************************/
void Key_Init(void)
{
    KeyCode = 0;    //���û�ʹ�õļ���, 1~16��Ч

    IO_KeyState = 0;
    IO_KeyState1 = 0;
    IO_KeyHoldCnt = 0;
}

/*****************************************************
    ���м�ɨ�����
    ʹ��XY����4x4���ķ�����ֻ�ܵ������ٶȿ�

   Y     P00      P01      P02      P03
          |        |        |        |
X         |        |        |        |
P04 ---- K00 ---- K01 ---- K02 ---- K03 ----
          |        |        |        |
P05 ---- K04 ---- K05 ---- K06 ---- K07 ----
          |        |        |        |
P06 ---- K08 ---- K09 ---- K10 ---- K11 ----
          |        |        |        |
P07 ---- K12 ---- K13 ---- K14 ---- K15 ----
          |        |        |        |
******************************************************/
u8 code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};
void    IO_KeyScan(void)    //50ms call
{
    u8  i,j;

    j = IO_KeyState1;   //������һ��״̬

    P0 = 0xf0;  //Y�ͣ���X
    for(i=0;i<60;i++);
    IO_KeyState1 = P0 & 0xf0;

    P0 = 0x0f;  //X�ͣ���Y
    for(i=0;i<60;i++);
    IO_KeyState1 |= (P0 & 0x0f);
    IO_KeyState1 ^= 0xff;   //ȡ��
    
    if(j == IO_KeyState1)   //�������ζ����
    {
        j = IO_KeyState;
        IO_KeyState = IO_KeyState1;
        if(IO_KeyState != 0)    //�м�����
        {
            F0 = 0;
            if(j == 0)  F0 = 1; //��һ�ΰ���
            else if(j == IO_KeyState)
            {
                if(++IO_KeyHoldCnt >= 20)   //1����ؼ�
                {
                    IO_KeyHoldCnt = 18;
                    F0 = 1;
                }
            }
            if(F0)
            {
                j = T_KeyTable[IO_KeyState >> 4];							   //�м�
                if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
                    KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f];    //�������
            }
        }
        else    IO_KeyHoldCnt = 0;
    }
    P0 = 0xff;
}
