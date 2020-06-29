#ifndef __COMMON_H__
#define __COMMON_H__

#include "stc8f.h"
//#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "version.h"
//#include <stdint.h>

/*==========Typedef==========*/

/*==========<stdint.h>==========*/
#ifndef int8_t
typedef signed char int8_t;
//#define int8_t int8_t
#define INT8_MIN (-128)
#define INT8_MAX (127)
#endif

#ifndef int16_t
typedef signed int int16_t;
//#define int16_t int16_t
#define INT16_MIN (-32768)
#define INT16_MAX (32767)
#endif

#ifndef int32_t
typedef signed long int int32_t;
//#define int32_t int32_t
#define INT32_MIN (-2147483648L)
#define INT32_MAX (2147483647L)
#endif

#ifndef uint8_t
typedef unsigned char uint8_t;
//#define uint8_t uint8_t
#define UINT8_MAX (255)
#endif

#ifndef uint16_t
typedef unsigned int uint16_t;
//#define uint16_t uint16_t
#define UINT16_MAX (65535U)
#endif

#ifndef uint32_t
typedef unsigned long int uint32_t;
//#define uint32_t uint32_t
#define UINT32_MAX (4294967295UL)
#endif
/*==========<stdint.h>==========*/

#ifndef uint8
typedef uint8_t uint8;
#endif

#ifndef uint16
typedef uint16_t uint16;
#endif

#ifndef uint32
typedef uint32_t uint32;
#endif

#ifndef int8
typedef int8_t int8;
#endif

#ifndef int16
typedef int16_t int16;
#endif

#ifndef int32
typedef int32_t int32;
#endif

#ifndef uchar
typedef uint8 uchar;
#endif

#ifndef uint
typedef uint16 uint;
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef OK
#define OK 0
#endif

#ifndef ERROR
#define ERROR 1
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef RTN_ERR
#define RTN_ERR (uchar)1 //返回1
#endif

#ifndef RTN_OK
#define RTN_OK (uchar)0 //返回0
#endif

#ifndef RTN_NULL
#define RTN_NULL (void *)0 //返回NULL
#endif

typedef struct rtc_time_s {
    uchar year;
    uchar week;
    uchar month;
    uchar day;
    uchar hour; // force 24 hour mode
    uchar minute;
    uchar second;
} rtc_time_t;

//方向
#define DR_UP (uchar)0x10    // 0上
#define DR_DOWN (uchar)0x11  // 1下
#define DR_LEFT (uchar)0x12  // 2左
#define DR_RIGHT (uchar)0x13 // 3右

/*==========Function Extern==========*/
/*****************************************************************************
 函 数 名  : delay_xms
 功能描述  : 毫秒延时
 输入参数  : x  延时时间,毫秒
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void delay_xms(uint x);

/*****************************************************************************
 函 数 名  : delay_xus
 功能描述  : 微秒延时
 输入参数  : x  延时时间,微秒
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void delay_xus(uint x);

#endif