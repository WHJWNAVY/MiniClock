#include "drv_3231.h"
#include "gpio_i2c.h"

//sbit DS3231_IIC_SDA    = P1 ^ 7;
//sbit DS3231_IIC_SCL    = P1 ^ 5;

#define DEV_ADDR_DS3231  0xd0    /* DS3231 slave address (write) */

uchar code ds3231_regaddr[7] = {0x06, 0x03, 0x05, 0x04, 0x02, 0x01, 0x00};//ds3231��ַ,��������ʱ����

/*****************************************************************************
 �� �� ��  : ds3231_set_time
 ��������  : ���õ�ǰʱ��
 �������  : time   Ҫ���õ�ʱ��
 �������  : ��
 �� �� ֵ  : ��
*****************************************************************************/
void ds3231_set_time(rtc_time_t *time)
{
    uchar i = 0, j = 0;
    uchar xdata rtc_time[7] = {0};

    rtc_time[0] = ((time->year > 99) ? 99 : time->year);//0-99
    rtc_time[1] = ((time->week > 7) ? 7 : \
                   ((time->week < 1) ? 1 : time->week));//1-7
    rtc_time[2] = ((time->month > 12) ? 12 : \
                   ((time->month < 1) ? 1 : time->month));//1-12
    rtc_time[3] = ((time->day > 31) ? 31 : \
                   ((time->day < 1) ? 1 : time->day));//1-31
    rtc_time[4] = ((time->hour > 23) ? 23 : time->hour);//0-23,���û��ȡΪ24Сʱ��ʱ��
    rtc_time[5] = ((time->minute > 59) ? 59 : time->minute);//0-59
    rtc_time[6] = ((time->second > 59) ? 59 : time->second);//0-59

    //��ʮ��������ת����BCD��
    for(i = 0; i < 7; i++)
    {
        j = rtc_time[i] / 10;
        rtc_time[i] = rtc_time[i] % 10;
        rtc_time[i] = rtc_time[i] + j * 16;
    }

    for(i = 0; i < 7; i++)
    {
        gpio_i2c_write(DEV_ADDR_DS3231, ds3231_regaddr[i], rtc_time[i]);
    }
}

/*****************************************************************************
 �� �� ��  : ds3231_read_time
 ��������  : ��ȡ��ǰʱ��
 �������  : void
 �������  : time   ��ȡ��������ʱ��
 �� �� ֵ  : ��
*****************************************************************************/
void ds3231_read_time(rtc_time_t *time)
{
    uchar rtc_time[7] = {0};
    uchar i, temp;
    for(i = 0; i < 7; i++)
    {
        rtc_time[i] = gpio_i2c_read(DEV_ADDR_DS3231, ds3231_regaddr[i]);
        //��BCD��ת����ʮ��������
        temp = rtc_time[i] / 16;
        rtc_time[i] = rtc_time[i] % 16;
        rtc_time[i] = rtc_time[i] + temp * 10;
    }

    time->year      = ((rtc_time[0] > 99) ? 99 : rtc_time[0]);//0-99
    time->week      = ((rtc_time[1] > 7) ? 7 : \
                       ((rtc_time[1] < 1) ? 1 : rtc_time[1]));//1-7
    time->month     = ((rtc_time[2] > 12) ? 12 : \
                       ((rtc_time[2] < 1) ? 1 : rtc_time[2]));//1-12
    time->day       = ((rtc_time[3] > 31) ? 31 : \
                       ((rtc_time[3] < 1) ? 1 : rtc_time[3]));//1-31
    time->hour      = ((rtc_time[4] > 23) ? 23 : rtc_time[4]);//0-23,���û��ȡΪ24Сʱ��ʱ��
    time->minute    = ((rtc_time[5] > 59) ? 59 : rtc_time[5]);//0-59
    time->second    = ((rtc_time[6] > 59) ? 59 : rtc_time[6]);//0-59
}

/*****************************************************************************
 �� �� ��  : ds3231_init
 ��������  : RTC��ʼ��
 �������  : void
 �������  : ��
 �� �� ֵ  : ��
*****************************************************************************/
void ds3231_init(void)
{
    gpio_i2c_init();
    //gpio_i2c_write(DEV_ADDR_DS3231, 0x0e, 0x1c);
}