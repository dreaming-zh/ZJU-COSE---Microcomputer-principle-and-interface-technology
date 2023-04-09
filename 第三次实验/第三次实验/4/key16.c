/*************  本程序功能说明  **************
翻转法获取键码，存放在KeyCode中
******************************************/
#include <stc15.h>
#include "led8.h"

u8 KeyCode;    //给用户使用的键码, 1~16有效
u8 IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;    //行列键盘变量

/**********************************************/
void Key_Init(void)
{
    KeyCode = 0;    //给用户使用的键码, 1~16有效

    IO_KeyState = 0;
    IO_KeyState1 = 0;
    IO_KeyHoldCnt = 0;
}

/*****************************************************
    行列键扫描程序
    使用XY查找4x4键的方法，只能单键，速度快

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

    j = IO_KeyState1;   //保存上一次状态

    P0 = 0xf0;  //Y低，读X
    for(i=0;i<60;i++);
    IO_KeyState1 = P0 & 0xf0;

    P0 = 0x0f;  //X低，读Y
    for(i=0;i<60;i++);
    IO_KeyState1 |= (P0 & 0x0f);
    IO_KeyState1 ^= 0xff;   //取反
    
    if(j == IO_KeyState1)   //连续两次读相等
    {
        j = IO_KeyState;
        IO_KeyState = IO_KeyState1;
        if(IO_KeyState != 0)    //有键按下
        {
            F0 = 0;
            if(j == 0)  F0 = 1; //第一次按下
            else if(j == IO_KeyState)
            {
                if(++IO_KeyHoldCnt >= 20)   //1秒后重键
                {
                    IO_KeyHoldCnt = 18;
                    F0 = 1;
                }
            }
            if(F0)
            {
                j = T_KeyTable[IO_KeyState >> 4];							   //有键
                if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
                    KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f];    //计算键码
            }
        }
        else    IO_KeyHoldCnt = 0;
    }
    P0 = 0xff;
}
