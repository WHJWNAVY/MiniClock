#include "drv_1302.h"

sbit DS1302_SCK = P2 ^ 0;
sbit DS1302_IO = P2 ^ 1;
sbit DS1302_CE = P2 ^ 2;

uchar code write_add[7] = {
    0x8c, 0x8a, 0x88, 0x86,
    0x84, 0x82, 0x80};  // ds1302写时间地址,年周月日时分秒
uchar code read_add[7] = {0x8d, 0x8b, 0x89, 0x87,
                          0x85, 0x83, 0x81};  // ds1302读时间地址,年周月日时分秒

uchar code alarm_wr[7] = {
    0xc0, 0xc2, 0xc4, 0xc6,
    0xc8, 0xca, 0xcc};  // ds1302写寄存器地址,年周月日时分秒
uchar code alarm_rd[7] = {
    0xc1, 0xc3, 0xc5, 0xc7,
    0xc9, 0xcb, 0xcd};                  // ds1302读寄存器地址,年周月日时分秒
uchar code alarm_on[2] = {0xce, 0xcf};  // 0-write, 1-read

#define ALARM_FLAG_ON 0xF0
#define ALARM_FLAG_OFF 0X0F

/*****************************************************************************
 函 数 名  : ds1302_write_byte
 功能描述  : 模拟IIC时序,向ds1302写入一个字节
 输入参数  : dat    待写入的数据
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static void ds1302_write_byte(uchar dat) {
    uchar i;
    for (i = 0; i < 8; i++) {
        DS1302_SCK = 0;
        DS1302_IO = dat & 0x01;
        dat = dat >> 1;
        DS1302_SCK = 1;
    }
}

/*****************************************************************************
 函 数 名  : ds1302_write
 功能描述  : 向ds1302指定寄存器写入一个字节
 输入参数  : add    ds1302寄存器地址
             dat    待写入的数据
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
static void ds1302_write(uchar add, uchar dat) {
    DS1302_CE = 0;
    _nop_();
    DS1302_SCK = 0;
    _nop_();
    DS1302_CE = 1;
    _nop_();
    ds1302_write_byte(add);
    ds1302_write_byte(dat);
    DS1302_CE = 0;
    _nop_();
    DS1302_IO = 1;
    DS1302_SCK = 1;
}

/*****************************************************************************
 函 数 名  : ds1302_read
 功能描述  : 从ds1302指定寄存器读取一个字节
 输入参数  : add    ds1302寄存器地址
 输出参数  : 无
 返 回 值  : 读取到的值
*****************************************************************************/
static uchar ds1302_read(uchar add) {
    uchar i, value;
    DS1302_CE = 0;
    _nop_();
    DS1302_SCK = 0;
    _nop_();
    DS1302_CE = 1;
    _nop_();
    ds1302_write_byte(add);
    for (i = 0; i < 8; i++) {
        value = value >> 1;
        DS1302_SCK = 0;
        if (DS1302_IO) value = value | 0x80;
        DS1302_SCK = 1;
    }
    DS1302_CE = 0;
    _nop_();
    DS1302_SCK = 0;
    _nop_();
    DS1302_SCK = 1;
    DS1302_IO = 1;
    return value;
}

/*****************************************************************************
 函 数 名  : rtc_set_time
 功能描述  : 设置当前时间
 输入参数  : time   要设置的时间
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void rtc_set_time(rtc_time_t *time) {
    uchar i = 0, j = 0;
    uchar rtc_time[7] = {0};

    rtc_time[0] = ((time->year > 99) ? 99 : time->year);  // 0-99
    rtc_time[1] =
        ((time->week > 7) ? 7 : ((time->week < 1) ? 1 : time->week));  // 1-7
    rtc_time[2] =
        ((time->month > 12) ? 12
                            : ((time->month < 1) ? 1 : time->month));  // 1-12
    rtc_time[3] =
        ((time->day > 31) ? 31 : ((time->day < 1) ? 1 : time->day));  // 1-31
    rtc_time[4] =
        ((time->hour > 23) ? 23 : time->hour);  // 0-23,设置或读取为24小时制时间
    rtc_time[5] = ((time->minute > 59) ? 59 : time->minute);  // 0-59
    rtc_time[6] = ((time->second > 59) ? 59 : time->second);  // 0-59

    //把十进制数据转换成BCD码
    for (i = 0; i < 7; i++) {
        j = rtc_time[i] / 10;
        rtc_time[i] = rtc_time[i] % 10;
        rtc_time[i] = rtc_time[i] + j * 16;
    }

    rtc_time[4] = rtc_time[4] & 0x3f; /* force clock to 24 hour mode */
    rtc_time[6] = rtc_time[6] & 0x7f; /* Enable clock oscillator */

    ds1302_write(0x8e, 0x00);  //去除写保护
    for (i = 0; i < 7; i++) {
        ds1302_write(write_add[i], rtc_time[i]);
    }
    ds1302_write(0x8e, 0x80);  //加写保护
}

/*****************************************************************************
 函 数 名  : rtc_read_time
 功能描述  : 获取当前时间
 输入参数  : void
 输出参数  : time   读取到的闹钟时间
 返 回 值  : 无
*****************************************************************************/
void rtc_read_time(rtc_time_t *time) {
    uchar rtc_time[7] = {0};
    uchar i, temp;
    for (i = 0; i < 7; i++) {
        rtc_time[i] = ds1302_read(read_add[i]);
        //把BCD码转换成十进制数据
        temp = rtc_time[i] / 16;
        rtc_time[i] = rtc_time[i] % 16;
        rtc_time[i] = rtc_time[i] + temp * 10;
    }

    time->year = ((rtc_time[0] > 99) ? 99 : rtc_time[0]);  // 0-99
    time->week =
        ((rtc_time[1] > 7) ? 7 : ((rtc_time[1] < 1) ? 1 : rtc_time[1]));  // 1-7
    time->month =
        ((rtc_time[2] > 12) ? 12
                            : ((rtc_time[2] < 1) ? 1 : rtc_time[2]));  // 1-12
    time->day =
        ((rtc_time[3] > 31) ? 31
                            : ((rtc_time[3] < 1) ? 1 : rtc_time[3]));  // 1-31
    time->hour =
        ((rtc_time[4] > 23) ? 23
                            : rtc_time[4]);  // 0-23,设置或读取为24小时制时间
    time->minute = ((rtc_time[5] > 59) ? 59 : rtc_time[5]);  // 0-59
    time->second = ((rtc_time[6] > 59) ? 59 : rtc_time[6]);  // 0-59
}

void rtc_init(void) {
#if 0
    rtc_time_t time_t = {0};
    rtc_read_time(&time_t);
    rtc_set_time(&time_t);
#endif
}

/*****************************************************************************
 函 数 名  : alarm_set_time
 功能描述  : 设置闹钟时间
 输入参数  : time   要设置的闹钟时间
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void alarm_set_time(rtc_time_t *time) {
    uchar i = 0, j = 0;
    uchar rtc_time[7] = {0};

    rtc_time[0] = ((time->year > 99) ? 99 : time->year);  // 0-99
    rtc_time[1] =
        ((time->week > 7) ? 7 : ((time->week < 1) ? 1 : time->week));  // 1-7
    rtc_time[2] =
        ((time->month > 12) ? 12
                            : ((time->month < 1) ? 1 : time->month));  // 1-12
    rtc_time[3] =
        ((time->day > 31) ? 31 : ((time->day < 1) ? 1 : time->day));  // 1-31
    rtc_time[4] = ((time->hour > 23) ? 23 : time->hour);              // 0-23
    rtc_time[5] = ((time->minute > 59) ? 59 : time->minute);          // 0-59
    rtc_time[6] = ((time->second > 59) ? 59 : time->second);          // 0-59

    //把十进制数据转换成BCD码
    for (i = 0; i < 7; i++) {
        j = rtc_time[i] / 10;
        rtc_time[i] = rtc_time[i] % 10;
        rtc_time[i] = rtc_time[i] + j * 16;
    }

    rtc_time[4] = rtc_time[4] & 0x3f; /* force clock to 24 hour mode */
    rtc_time[6] = rtc_time[6] & 0x7f; /* Enable clock oscillator */

    ds1302_write(0x8e, 0x00);  //去除写保护
    for (i = 0; i < 7; i++) {
        ds1302_write(alarm_wr[i], rtc_time[i]);
    }
    ds1302_write(0x8e, 0x80);  //加写保护
}

/*****************************************************************************
 函 数 名  : alarm_read_time
 功能描述  : 获取闹钟时间
 输入参数  : 无
 输出参数  : time   要设置的闹钟时间
 返 回 值  : 无
*****************************************************************************/
void alarm_read_time(rtc_time_t *time) {
    uchar rtc_time[7] = {0};
    uchar i, temp;
    for (i = 0; i < 7; i++) {
        rtc_time[i] = ds1302_read(alarm_rd[i]);
        //把BCD码转换成十进制数据
        temp = rtc_time[i] / 16;
        rtc_time[i] = rtc_time[i] % 16;
        rtc_time[i] = rtc_time[i] + temp * 10;
    }

    time->year = ((rtc_time[0] > 99) ? 99 : rtc_time[0]);  // 0-99

    time->week =
        ((rtc_time[1] > 7) ? 7 : ((rtc_time[1] < 1) ? 1 : rtc_time[1]));  // 1-7
    time->month =
        ((rtc_time[2] > 12) ? 12
                            : ((rtc_time[2] < 1) ? 1 : rtc_time[2]));  // 1-12
    time->day =
        ((rtc_time[3] > 31) ? 31
                            : ((rtc_time[3] < 1) ? 1 : rtc_time[3]));  // 1-31
    time->hour = ((rtc_time[4] > 23) ? 23 : rtc_time[4]);              // 0-23
    time->minute = ((rtc_time[5] > 59) ? 59 : rtc_time[5]);            // 0-59
    time->second = ((rtc_time[6] > 59) ? 59 : rtc_time[6]);            // 0-59
}

/*****************************************************************************
 函 数 名  : alarm_set_flag
 功能描述  : 设置闹钟开关
 输入参数  : b      闹钟开启标志
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void alarm_set_flag(uchar b) {
    uchar i = 0, j = 0;
    uchar flag = (b ? ALARM_FLAG_ON : ALARM_FLAG_OFF);

    //把十进制数据转换成BCD码
    j = flag / 10;
    flag = flag % 10;
    flag = flag + j * 16;

    ds1302_write(0x8e, 0x00);  //去除写保护
    ds1302_write(alarm_on[0], flag);
    ds1302_write(0x8e, 0x80);  //加写保护
}

/*****************************************************************************
 函 数 名  : alarm_get_flag
 功能描述  : 获取闹钟开关
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 闹钟开启标志
*****************************************************************************/
uchar alarm_get_flag(void) {
    uchar flag = 0, temp = 0;
    flag = ds1302_read(alarm_on[1]);
    //把BCD码转换成十进制数据
    temp = flag / 16;
    flag = flag % 16;
    flag = flag + temp * 10;

    return ((flag == ALARM_FLAG_ON) ? 1 : 0);
}