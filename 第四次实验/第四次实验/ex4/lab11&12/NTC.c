
/*---------------------------------------------------------------------*/
/* --- STC MCU Limited ------------------------------------------------*/
/* --- STC 1T Series MCU Demo Programme -------------------------------*/
/* --- Mobile: (86)13922805190 ----------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 -----------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ------------------------*/
/* --- Web: www.STCMCU.com --------------------------------------------*/
/* --- Web: www.GXWMCU.com --------------------------------------------*/
/* --- QQ:  800003751 -------------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/
/*************  本程序功能说明  **************
左边4位数码管显示第9通道片内BGV的基准电压读数, 
右边4位数码管显示NTC温度值, 分辨率0.1度.
NTC使用1%精度的MF52 B：3950，10K@25度C.
测温度时, 为了通用, 使用12位的AD，测温度的ADC3进行4次ADC连续采样 
使用对分查找表格来计算, 小数点后一位数是用线性插补来计算的.
******************************************/
#include <stc15.h>
#include <intrins.h>
#include "gdef.h"
#include "adc.h"

/********************** ADC初始化 ************************/
void ADC_Init()	 //CLK_DIV的ADRJ默认为0
{	
    ADC_CONTR = 0xE0;   //90T, ADC power on
	P1ASF = 0;
//	delay();  		  	//1ms以内电源就能稳定
}	  

/*******************查询法读一次ADC结果*******************/
u16 Get_ADC10bitResult(u8 channel)  //channel = 0~8,8为内部BGV
{
	ADC_RES = 0;
    ADC_RESL = 0;

    ADC_CONTR = (ADC_CONTR & 0xe0) | 0x08 | (channel&7);    //start the ADC

    while((ADC_CONTR & 0x10) == 0)  ;   //wait for ADC finish
    ADC_CONTR &= ~0x10;     		//清除ADC结束标志
    return  (((u16)ADC_RES << 2) | (ADC_RESL & 3));	 	
}


//  MF52E 10K at 25, B = 3950, ADC = 12 bits
u16 code temp_table[]={
        140,    //;-40  0
        149,    //;-39  1
        159,    //;-38  2
        168,    //;-37  3
        178,    //;-36  4
        188,    //;-35  5
        199,    //;-34  6
        210,    //;-33  7
        222,    //;-32  8
        233,    //;-31  9
        246,    //;-30  10
        259,    //;-29  11
        272,    //;-28  12
        286,    //;-27  13
        301,    //;-26  14
        317,    //;-25  15
        333,    //;-24  16
        349,    //;-23  17
        367,    //;-22  18
        385,    //;-21  19
        403,    //;-20  20
        423,    //;-19  21
        443,    //;-18  22
        464,    //;-17  23
        486,    //;-16  24
        509,    //;-15  25
        533,    //;-14  26
        558,    //;-13  27
        583,    //;-12  28
        610,    //;-11  29
        638,    //;-10  30
        667,    //;-9   31
        696,    //;-8   32
        727,    //;-7   33
        758,    //;-6   34
        791,    //;-5   35
        824,    //;-4   36
        858,    //;-3   37
        893,    //;-2   38
        929,    //;-1   39
        965,    //;0    40
        1003,   //;1    41
        1041,   //;2    42
        1080,   //;3    43
        1119,   //;4    44
        1160,   //;5    45
        1201,   //;6    46
        1243,   //;7    47
        1285,   //;8    48
        1328,   //;9    49
        1371,   //;10   50
        1414,   //;11   51
        1459,   //;12   52
        1503,   //;13   53
        1548,   //;14   54
        1593,   //;15   55
        1638,   //;16   56
        1684,   //;17   57
        1730,   //;18   58
        1775,   //;19   59
        1821,   //;20   60
        1867,   //;21   61
        1912,   //;22   62
        1958,   //;23   63
        2003,   //;24   64
        2048,   //;25   65
        2093,   //;26   66
        2137,   //;27   67
        2182,   //;28   68
        2225,   //;29   69
        2269,   //;30   70
        2312,   //;31   71
        2354,   //;32   72
        2397,   //;33   73
        2438,   //;34   74
        2479,   //;35   75
        2519,   //;36   76
        2559,   //;37   77
        2598,   //;38   78
        2637,   //;39   79
        2675,   //;40   80
        2712,   //;41   81
        2748,   //;42   82
        2784,   //;43   83
        2819,   //;44   84
        2853,   //;45   85
        2887,   //;46   86
        2920,   //;47   87
        2952,   //;48   88
        2984,   //;49   89
        3014,   //;50   90
        3044,   //;51   91
        3073,   //;52   92
        3102,   //;53   93
        3130,   //;54   94
        3157,   //;55   95
        3183,   //;56   96
        3209,   //;57   97
        3234,   //;58   98
        3259,   //;59   99
        3283,   //;60   100
        3306,   //;61   101
        3328,   //;62   102
        3351,   //;63   103
        3372,   //;64   104
        3393,   //;65   105
        3413,   //;66   106
        3432,   //;67   107
        3452,   //;68   108
        3470,   //;69   109
        3488,   //;70   110
        3506,   //;71   111
        3523,   //;72   112
        3539,   //;73   113
        3555,   //;74   114
        3571,   //;75   115
        3586,   //;76   116
        3601,   //;77   117
        3615,   //;78   118
        3628,   //;79   119
        3642,   //;80   120
        3655,   //;81   121
        3667,   //;82   122
        3679,   //;83   123
        3691,   //;84   124
        3702,   //;85   125
        3714,   //;86   126
        3724,   //;87   127
        3735,   //;88   128
        3745,   //;89   129
        3754,   //;90   130
        3764,   //;91   131
        3773,   //;92   132
        3782,   //;93   133
        3791,   //;94   134
        3799,   //;95   135
        3807,   //;96   136
        3815,   //;97   137
        3822,   //;98   138
        3830,   //;99   139
        3837,   //;100  140
        3844,   //;101  141
        3850,   //;102  142
        3857,   //;103  143
        3863,   //;104  144
        3869,   //;105  145
        3875,   //;106  146
        3881,   //;107  147
        3887,   //;108  148
        3892,   //;109  149
        3897,   //;110  150
        3902,   //;111  151
        3907,   //;112  152
        3912,   //;113  153
        3917,   //;114  154
        3921,   //;115  155
        3926,   //;116  156
        3930,   //;117  157
        3934,   //;118  158
        3938,   //;119  159
        3942    //;120  160
};

/********************  计算温度 ***********************************************/
// 计算结果: 0对应-40.0度, 400对应0度, 650对应25.0度, 最大1600对应120.0度. 
// 为了通用, ADC输入为12bit的ADC值.
/**********************************************/  
#define     D_SCALE     10      //1位小数，为了避免出现小数，结果放大倍数
u16 get_temperature(u16 adc)
{
    u16 code *p;
    u8  j,k,min,max;
    
    adc = 4096 - adc;   //Rt接地
    p = temp_table;
    if(adc < p[0]||adc > p[160]) //超出量程   
		return (0xffff);
    
    min = 0;        //-40度
    max = 160;      //120度

    for(j=0; j<8; j++)  //对分查表，2^8可查256项>160
    {
        k = (min + max) / 2;
        if(adc < p[k]) max = k;
        else           min = k;
    }
    if(adc == p[min]) return min * D_SCALE;
//    else if(adc == p[max]) return max * D_SCALE;
    else // min < temp < max   max=min+1  
		return min* D_SCALE+(adc - p[min]) * D_SCALE / (p[max] - p[min]);
}

