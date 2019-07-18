#ifndef __DRV_3231_H__
#define __DRV_3231_H__

#include "common.h"

/*****************************************************************************
 函 数 名  : ds3231_set_time
 功能描述  : 设置当前时间
 输入参数  : time   要设置的时间
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void ds3231_set_time(rtc_time_t *time);

/*****************************************************************************
 函 数 名  : ds3231_read_time
 功能描述  : 获取当前时间
 输入参数  : void
 输出参数  : time   读取到的闹钟时间
 返 回 值  : 无
*****************************************************************************/
extern void ds3231_read_time(rtc_time_t *time);

/*****************************************************************************
 函 数 名  : ds3231_init
 功能描述  : RTC初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void ds3231_init(void);

#endif