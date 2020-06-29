#include "common.h"

/*****************************************************************************
 函 数 名  : delay_xms
 功能描述  : 毫秒延时
 输入参数  : x  延时时间,毫秒
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void delay_xms(uint x) //@24MHz
{
    uchar i = 0, j = 0;
    while (x--) {
        i = 32;
        j = 40;
        do {
            while (--j)
                ;
        } while (--i);
    }
}

/*****************************************************************************
 函 数 名  : delay_xus
 功能描述  : 微秒延时
 输入参数  : x  延时时间,微秒
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void delay_xus(uint x) //@24MHz, delay us
{
    uint8 i = 0;
    while (x--) {
        i = 5;
        while (--i)
            ;
    }
}
