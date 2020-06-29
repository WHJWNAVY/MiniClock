#ifndef __CHNIA_NONLI_H__
#define __CHNIA_NONLI_H__

#include "common.h"

#define NONLI_STRING_LEN 8

typedef struct china_nonli_s {
    uchar month;  //农历月
    uchar intcal; //农历闰月
    uchar day;    //农历日
} china_nonli_t;

/*****************************************************************************
 函 数 名  : china_nonli_get
 功能描述  : 根据输入的RTC时间获取农历时间
 输入参数  : tm         当前RTC时间
 输出参数  : nonli      当前农历时间
 返 回 值  : 成功-RTN_OK, 失败-RTN_ERR
*****************************************************************************/
extern uchar china_nonli_get(rtc_time_t *tm, china_nonli_t *nonli);

#endif
