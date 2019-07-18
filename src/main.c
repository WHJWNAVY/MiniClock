#include "common.h"
#include "drv_led.h"
#include "drv_key.h"
#include "drv_3231.h"
#include "china_nonli.h"
#include "drv_18b20.h"

#include "drv_cfg.h"

#define DEF_SYS_LED_FPS      (100u)
#define DEF_SYS_LED_FLASH    (3u)
#define DEF_SYS_LED_MIRROR   (0u)
#define DEF_SYS_AUTOEXIT_TMR (150u)
#define DEF_SYS_LED_LIGHT    (5u)
#define DEF_SYS_AUTO_SLEEP   (1u)

//�˵��б�
typedef enum page_mode_e
{
    PAGE_MENU_MIN = 0x1Au,
    PAGE_MENU_MAIN = PAGE_MENU_MIN,//������
    PAGE_MENU_STIME,//ʱ�����ý���
    PAGE_MENU_SDATE,//�������ý���
    PAGE_MENU_SATSLEEP,//�Զ���������
    PAGE_MENU_SYSCFG,//ϵͳ����
    PAGE_MENU_MAX,
    PAGE_MENU_POWER,
} page_mode_e;

//��ʱģʽ
typedef enum main_mode_e
{
    MAIN_MODE_MIN = 0x20u,
    MAIN_MODE_TIME = MAIN_MODE_MIN, //time
    MAIN_MODE_TEMP, // temp
    MAIN_MODE_NONLI, // nonli
    MAIN_MODE_MAX,
} main_mode_t;

//��ҳ�л�
typedef enum main_page_e
{
    MAIN_PAGE_MIN = 0x2Au,
    MAIN_PAGE_HHMM = MAIN_PAGE_MIN, //hour - minute
    MAIN_PAGE_TTSS, // am/pm - second
    MAIN_PAGE_YYYY, // year
    MAIN_PAGE_MMDD, // month - day
    MAIN_PAGE_DDDD, // week
    MAIN_PAGE_MAX,
} main_page_t;


//ʱ�����ý���
typedef enum set_time_page_e
{
    SET_TIME_PAGE_MIN = 0x3Au,
    SET_TIME_PAGE_HOUR = SET_TIME_PAGE_MIN, //hour
    SET_TIME_PAGE_MINUTE,   // minute
    SET_TIME_PAGE_SECOND,   // second
    SET_TIME_PAGE_MAX,
} set_time_page_t;

//�������ý���
typedef enum set_date_page_e
{
    SET_DATE_PAGE_MIN = 0x4Au,
    SET_DATE_PAGE_YEAR = SET_DATE_PAGE_MIN, // year
    SET_DATE_PAGE_WEEK,     // week
    SET_DATE_PAGE_MONTH,    // month
    SET_DATE_PAGE_DAY,      // day
    SET_DATE_PAGE_MAX,
} set_date_page_t;

typedef enum set_system_page_e
{
    SET_SYSTEM_PAGE_MIN = 0x50u,
    SET_SYSTEM_PAGE_LIGHT = SET_SYSTEM_PAGE_MIN,
    SET_SYSTEM_PAGE_MIRROR,
    SET_SYSTEM_PAGE_MAX,
}set_system_page_t;

//�Ӽ�����
typedef enum set_oper_e
{
    SET_OPER_MIN = 0x5Au,
    SET_OPER_ADD,
    SET_OPER_SUB,
    SET_OPER_MAX,
} set_oper_t;

//�������ý���
typedef enum set_auto_sleep_e
{
    SET_AUTO_SLEEP_MIN = 0x6Au,
    SET_AUTO_SLEEP_SHOUR = SET_AUTO_SLEEP_MIN,
    SET_AUTO_SLEEP_SMINUTE,
    SET_AUTO_SLEEP_EHOUR,
    SET_AUTO_SLEEP_EMINUTE,
    SET_AUTO_SLEEP_ON,
    SET_AUTO_SLEEP_MAX,
} set_auto_sleep_t;

typedef struct auto_sleep_s
{
    uint8 s_hour;
    uint8 s_min;
    uint8 e_hour;
    uint8 e_min;
    uint8 on;
} auto_sleep_t;

typedef struct system_cfg_e
{
    uint8 led_light;
    uint8 led_mirror;
    auto_sleep_t autosleep;
} system_cfg_t;

#define SYSTEM_CFG_SIZE ((SET_SYSTEM_PAGE_MAX-SET_SYSTEM_PAGE_MIN)+(SET_AUTO_SLEEP_MAX-SET_AUTO_SLEEP_MIN))//sizeof(sys_cfg_t)

void system_cfg_init(system_cfg_t* cfg)
{
    if(cfg != NULL)
    {
        cfg->led_light = DEF_SYS_LED_LIGHT;
        cfg->led_mirror = DEF_SYS_LED_MIRROR;
        cfg->autosleep.on = DEF_SYS_AUTO_SLEEP;
        cfg->autosleep.s_hour = 22;
        cfg->autosleep.s_min = 30;
        cfg->autosleep.e_hour = 7;
        cfg->autosleep.e_min = 30;
    }
}

uint8 system_cfg_write(system_cfg_t* cfg)
{
    uint8 ret = RTN_ERR;
    if(cfg != NULL)
    {
        ret = cfgsave_write((uint8*)cfg, /*sizeof(system_cfg_t)*/SYSTEM_CFG_SIZE);
    }
    
    return ret;
}

uint8 system_cfg_read(system_cfg_t* cfg)
{
    uint8 ret = RTN_ERR;
    if(cfg != NULL)
    {
        ret = cfgsave_read((uint8*)cfg, /*sizeof(system_cfg_t)*/SYSTEM_CFG_SIZE);
    }
    
    return ret;
}

uint8 system_cfg_copy(system_cfg_t* dst, system_cfg_t* src)
{
    uint8 ret = RTN_ERR;
    if((dst != NULL) && (src != NULL) && (src != dst))
    {
        dst->led_light = src->led_light;
        dst->led_mirror = src->led_mirror;
        dst->autosleep.on = src->autosleep.on;
        dst->autosleep.s_hour = src->autosleep.s_hour;
        dst->autosleep.s_min = src->autosleep.s_min;
        dst->autosleep.e_hour = src->autosleep.e_hour;
        dst->autosleep.e_min = src->autosleep.e_min;
        ret = RTN_OK;
    }
    
    return ret;
}

void system_cfg_apply(system_cfg_t* cfg)
{
    if(cfg != NULL)
    {
        led_set_light(cfg->led_light);
        led_set_mirror(cfg->led_mirror);
        key_set_mirror(cfg->led_mirror);
    }
}

void _menu_data_deal_(uchar *dat, uchar op, uchar max, uchar min)
{
    char data_t = 0;
    if((op == SET_OPER_ADD) || (op == SET_OPER_SUB))
    {
        data_t = (char)(*dat);
        if(op == SET_OPER_ADD)//��
        {
            data_t++;
            if(data_t > max)
            {
                data_t = min;
            }
        }
        else if(op == SET_OPER_SUB)///��
        {
            data_t--;
            if(data_t < min)
            {
                data_t = max;
            }
        }
        *dat = (uchar)data_t;
    }
}

void show_main_page(rtc_time_t *tm, uchar page)
{
    uchar n = 0;
    char xdata disp_str[LED_POS_MAX+1] = {0};
    if(tm == NULL)
    {
        return;
    }

    switch(page)
    {
    case MAIN_PAGE_HHMM:
        tm->hour   = tm->hour % 100;
        tm->minute = tm->minute % 100;

        led_puti(0, (tm->hour / 10), 1);
        led_puti(1, (tm->hour % 10), 1);
        led_puti(2, (tm->minute / 10), 1);
        led_puti(3, (tm->minute % 10), 1);

        //sprintf(disp_str, "%bu%bu%bu%bu", (tm->hour / 10), (tm->hour % 10), (tm->minute / 10), (tm->minute % 10));
        //led_puts(0, disp_str, 1);

        for(n=0; n<LED_POS_MAX; n++)
        {
            if(((tm->second) % LED_POS_MAX) == n)
            {
                led_putb(n, LED_SEGB_SET(LED_SEGB_DP), 1, 1);
            }
            else
            {
                led_putb(n, LED_SEGB_SET(LED_SEGB_DP), 0, 1);
            }
        }
        break;
    case MAIN_PAGE_TTSS:
        tm->second = tm->second % 100;

        led_puti(0, LED_SEG_NULL, 1);
        led_puti(1, (tm->second / 10), 1);
        led_puti(2, (tm->second % 10), 1);
        led_puti(3, LED_SEG_NULL, 1);

        //sprintf(disp_str, " %bu%bu ", (tm->second / 10), (tm->second % 10));
        //led_puts(0, disp_str, 1);
        break;
    case MAIN_PAGE_YYYY:
        tm->year = tm->year % 100;

        led_puti(0, 2, 1);
        led_puti(1, 0, 1);
        led_puti(2, (tm->year / 10), 1);
        led_puti(3, (tm->year % 10), 1);

        //sprintf(disp_str, "20%bu%bu", (tm->year / 10), (tm->year % 10));
        //led_puts(0, disp_str, 1);
        break;
    case MAIN_PAGE_MMDD:
        tm->month = tm->month % 100;
        tm->day   = tm->day % 100;

        led_puti(0, (tm->month / 10), 1);
        led_puti(1, (tm->month % 10), 1);
        led_puti(2, (tm->day / 10), 1);
        led_puti(3, (tm->day % 10), 1);

        //sprintf(disp_str, "%bu%bu%bu%bu", (tm->month / 10), (tm->month % 10), (tm->day / 10), (tm->day % 10));
        //led_puts(0, disp_str, 1);
        break;
    case MAIN_PAGE_DDDD:
        tm->week   = tm->week % 100;

        led_puti(0, (LED_SEG_NULL), 1);
        led_puti(1, (LED_SEG_EFG), 1);
        led_puti(2, (tm->week % 10), 1);
        led_puti(3, (LED_SEG_BCG), 1);

        //sprintf(disp_str, " {%bu}", /*(((tm->week % 10) > 5) ? "-" : " "),*/ (tm->week % 10));
        //led_puts(0, disp_str, 1);
        break;
    default:
        break;
    }
}

void show_time_set_page(rtc_time_t *tm, uchar page, uchar op)
{
    uchar dat = 0;
    char xdata disp_str[LED_POS_MAX+1] = {0};

    if(tm == NULL)
    {
        return;
    }

    switch(page)
    {
    case SET_TIME_PAGE_HOUR:
        dat = tm->hour;
        _menu_data_deal_(&dat, op, 23, 0);
        tm->hour = dat;
        break;
    case SET_TIME_PAGE_MINUTE:
        dat = tm->minute;
        _menu_data_deal_(&dat, op, 59, 0);
        tm->minute = dat;
        break;
    case SET_TIME_PAGE_SECOND:
        dat = tm->second;
        _menu_data_deal_(&dat, op, 59, 0);
        tm->second = dat;
        break;
    default:
        break;
    }

    page = page - SET_TIME_PAGE_MIN;

    disp_str[0] = LED_SEG_C;

    //page %= (LED_SEG_SG - LED_SEG_SA + 1);
    disp_str[1] = page + 1;

    //dat %= 100;
    disp_str[2] = dat / 10;
    disp_str[3] = dat % 10;
    led_putis(0, disp_str, LED_POS_MAX, 1);
}

void show_date_set_page(rtc_time_t *tm, uchar page, uchar op)
{
    uchar n = 0;
    uchar dat = 0;
    char xdata disp_str[LED_POS_MAX+1] = {0};

    if(tm == NULL)
    {
        return;
    }

    switch(page)
    {
    case SET_DATE_PAGE_YEAR:
        dat = tm->year;
        _menu_data_deal_(&dat, op, 99, 0);
        tm->year = dat;
        break;
    case SET_DATE_PAGE_MONTH:
        dat = tm->month;
        _menu_data_deal_(&dat, op, 12, 1);
        tm->month = dat;
        break;
    case SET_DATE_PAGE_DAY:
        dat = tm->day;
        _menu_data_deal_(&dat, op, 31, 1);
        tm->day = dat;
        break;
    case SET_DATE_PAGE_WEEK:
        n = 1;
        dat = tm->week;
        _menu_data_deal_(&dat, op, 7, 1);
        tm->week = dat;
        break;
    default:
        break;
    }

    page = page - SET_DATE_PAGE_MIN;

    disp_str[0] = LED_SEG_H;

    //page %= (LED_SEG_SG - LED_SEG_SA + 1);
    disp_str[1] = page + 1;

    //dat %= 100;
    disp_str[2] = dat / 10;
    disp_str[3] = dat % 10;
    led_putis(0, disp_str, LED_POS_MAX, 1);
}

void show_system_set_page(system_cfg_t*cfg, uchar page, uchar op)
{
    uchar n = 0;
    uchar dat = 0;
    char xdata disp_str[LED_POS_MAX+1] = {0};

    if(cfg == NULL)
    {
        return;
    }

    switch(page)
    {
    case SET_SYSTEM_PAGE_LIGHT:
        dat = cfg->led_light;
        _menu_data_deal_(&dat, op, LED_MAX_LIGHT, LED_MIN_LIGHT);
        cfg->led_light = dat;
        break;
    case SET_SYSTEM_PAGE_MIRROR:
        dat = cfg->led_mirror;
        _menu_data_deal_(&dat, op, 1, 0);
        cfg->led_mirror = dat;
        break;
    default:
        break;
    }

    page = page - SET_SYSTEM_PAGE_MIN;

    disp_str[0] = LED_SEG_P;

    //page %= (LED_SEG_SG - LED_SEG_SA + 1);
    disp_str[1] = page + 1;

    //dat %= 100;
    disp_str[2] = dat / 10;
    disp_str[3] = dat % 10;
    led_putis(0, disp_str, LED_POS_MAX, 1);
}

void show_autosleep_set_page(system_cfg_t*cfg, uchar page, uchar op)
{
    uchar n = 0;
    uchar dat = 0;
    char xdata disp_str[LED_POS_MAX+1] = {0};

    if(cfg == NULL)
    {
        return;
    }

    switch(page)
    {
    case SET_AUTO_SLEEP_SHOUR:
        dat = cfg->autosleep.s_hour;
        _menu_data_deal_(&dat, op, 23, 0);
        cfg->autosleep.s_hour = dat;
        break;
    case SET_AUTO_SLEEP_SMINUTE:
        dat = cfg->autosleep.s_min;
        _menu_data_deal_(&dat, op, 59, 0);
        cfg->autosleep.s_min = dat;
        break;
    case SET_AUTO_SLEEP_EHOUR:
        dat = cfg->autosleep.e_hour;
        _menu_data_deal_(&dat, op, 23, 0);
        cfg->autosleep.e_hour = dat;
        break;
    case SET_AUTO_SLEEP_EMINUTE:
        dat = cfg->autosleep.e_min;
        _menu_data_deal_(&dat, op, 59, 0);
        cfg->autosleep.e_min = dat;
        break;
    case SET_AUTO_SLEEP_ON:
        dat = cfg->autosleep.on;
        _menu_data_deal_(&dat, op, 1, 0);
        cfg->autosleep.on = dat;
        break;
    default:
        break;
    }

    page = page - SET_AUTO_SLEEP_MIN;

    disp_str[0] = LED_SEG_A;

    //page %= (LED_SEG_SG - LED_SEG_SA + 1);
    disp_str[1] = page + 1;

    //dat %= 100;
    disp_str[2] = dat / 10;
    disp_str[3] = dat % 10;
    led_putis(0, disp_str, LED_POS_MAX, 1);
}

void show_temp_page(void)
{
    uint8 xdata sign = 0;
    uint8 xdata disp_temp = 0;
    uint16 xdata interger = 0;
    uint16 xdata decimal = 0;

    if(ds18b20_get_temp(&sign, &interger, &decimal) == RTN_OK)
    {
        if(sign)//����
        {
            disp_temp = ((decimal >= 5) ? (interger + 1) : interger);
            disp_temp %= 100;
            led_puti(0, LED_SEG_HL, 1);//����
            led_puti(1, disp_temp / 10, 1);//ʮλ
            led_puti(2, disp_temp % 10, 1);//��λ
        }
        else//����
        {
            disp_temp = (interger % 100);
            led_puti(0, disp_temp / 10, 1);//ʮλ
            led_puti(1, disp_temp % 10, 1);//��λ
            led_puti(2, decimal % 10, 1);//С��
            if(led_get_mirror())//С����
            {//���������ʾ����С����Ӧ�ú���һλ
                led_putb(2, LED_SEGB_SET(LED_SEGB_DP), 1, 1);
            }
            else
            {
                led_putb(1, LED_SEGB_SET(LED_SEGB_DP), 1, 1);
            }
        }
        led_puti(3, LED_SEG_CEL, 1); // ���϶�
    }
}

void show_nonli_page(rtc_time_t* time_t)
{
    uchar xdata i = 0;
    china_nonli_t xdata nonli_t = {0};

    if(time_t == NULL)
    {
        return;
    }

    if(china_nonli_get(time_t, &nonli_t) != RTN_OK)
    {
        return;
    }

    //Ϊ�����֣���ʾ�ĸ���
    led_puti(0, nonli_t.month / 10, 1);
    led_puti(1, nonli_t.month % 10, 1);
    led_puti(2, nonli_t.day / 10, 1);
    led_puti(3, nonli_t.day % 10, 1);

    for(i=0; i<LED_POS_MAX; i++)
    {
        led_putb(i, LED_SEGB_SET(LED_SEGB_DP), 1, 1);
    }
}

void main(void)
{
#if 1
    uchar i = 0, j = 0;
    uchar key_code = 0;
    uchar page_menu_cnt = PAGE_MENU_MIN;
    uchar main_page_cnt = MAIN_PAGE_MIN;
    uchar main_mode_cnt = MAIN_MODE_MIN;
    uchar settime_page_cnt = SET_TIME_PAGE_MIN;
    uchar setdate_page_cnt = SET_DATE_PAGE_MIN;
    uchar setsys_page_cnt = SET_SYSTEM_PAGE_MIN;
    uchar setauto_sleep_cnt = SET_AUTO_SLEEP_MIN;
    uchar set_oper = SET_OPER_MIN;
    rtc_time_t time_t = {0};
    rtc_time_t xdata time_set = {0};
    rtc_time_t xdata date_set = {0};
    china_nonli_t xdata nonli_t = {0};

    system_cfg_t xdata syscfg_t = {0};
    system_cfg_t xdata syscfg_set = {0};

    uint8 main_autoexit_timer = 0;
    uint8 main_mode_force = 0;

    key_init();
    led_init();
    ds3231_init();

    if(system_cfg_read(&syscfg_t) != RTN_OK)
    {
        system_cfg_init(&syscfg_t);
        system_cfg_write(&syscfg_t);
    }
    system_cfg_apply(&syscfg_t);

    led_open_door(30);
    led_close_door(30);

    while(1)
    {
        key_code = key_get_code();

        set_oper = SET_OPER_MIN;

        if(key_code != KEY_BTN_NULL)
        {
            main_autoexit_timer = 0;
            main_mode_force = 0;

            //��������˳�����ģʽ
            if(page_menu_cnt == PAGE_MENU_POWER)
            {
                led_open();
                page_menu_cnt = PAGE_MENU_MAIN;
                main_autoexit_timer = DEF_SYS_AUTOEXIT_TMR+1;
                key_code = KEY_BTN_NULL;
                led_open_door(30);
            }
            
            switch(key_code)
            {
            case KEY_BTN_NEXT_PAGE://��һ������
            {
                page_menu_cnt++;
                if(page_menu_cnt >= PAGE_MENU_MAX)
                {
                    page_menu_cnt = PAGE_MENU_MIN;
                }

                if(page_menu_cnt == PAGE_MENU_MAIN)
                {
                    main_mode_cnt = MAIN_MODE_MIN;
                    main_page_cnt = MAIN_PAGE_MIN;
                    led_set_flashs(0, LED_POS_MAX, 0);//ȡ����˸
                }

                //��������ʱ�����
                if(page_menu_cnt == PAGE_MENU_STIME)
                {
                    settime_page_cnt = SET_TIME_PAGE_MIN;
                    ds3231_read_time(&time_set);
                    //�������ý������������˸
                    led_set_flashs(2, 2, DEF_SYS_LED_FLASH);//��˸
                }

                //�����������ڽ���
                if(page_menu_cnt == PAGE_MENU_SDATE)
                {
                    setdate_page_cnt = SET_DATE_PAGE_MIN;
                    ds3231_read_time(&date_set);
                    //�������ý������������˸
                    led_set_flashs(2, 2, DEF_SYS_LED_FLASH);//��˸
                }

                
                //����ϵͳ���ý���
                if(page_menu_cnt == PAGE_MENU_SYSCFG)
                {
                    setsys_page_cnt = SET_SYSTEM_PAGE_MIN;
                    system_cfg_copy(&syscfg_set, &syscfg_t);
                    //�������ý������������˸
                    led_set_flashs(2, 2, DEF_SYS_LED_FLASH);//��˸
                }

                //�����Զ��������ý���
                if(page_menu_cnt == PAGE_MENU_SATSLEEP)
                {
                    setauto_sleep_cnt = SET_AUTO_SLEEP_MIN;
                    system_cfg_copy(&syscfg_set, &syscfg_t);
                    //�������ý������������˸
                    led_set_flashs(2, 2, DEF_SYS_LED_FLASH);//��˸
                }
            }
            break;
            case KEY_BTN_SAVE_SETT://����
            {
                if(page_menu_cnt == PAGE_MENU_STIME)
                {
                    ds3231_read_time(&date_set);
                    time_set.year = date_set.year;
                    time_set.week = date_set.week;
                    time_set.month = date_set.month;
                    time_set.day = date_set.day;
                    ds3231_set_time(&time_set);
                    //�������ú�������ֹͣ��˸
                    led_set_flashs(0, LED_POS_MAX, 0);//ȡ����˸
                }

                if(page_menu_cnt == PAGE_MENU_SDATE)
                {
                    ds3231_read_time(&time_set);
                    date_set.hour = time_set.hour;
                    date_set.minute = time_set.minute;
                    date_set.second = time_set.second;
                    ds3231_set_time(&date_set);
                    //�������ú�������ֹͣ��˸
                    led_set_flashs(0, LED_POS_MAX, 0);//ȡ����˸
                }

                if(page_menu_cnt == PAGE_MENU_SYSCFG)
                {
                    system_cfg_copy(&syscfg_t, &syscfg_set);
                    system_cfg_write(&syscfg_t);
                    system_cfg_apply(&syscfg_t);
                    //�������ú�������ֹͣ��˸
                    led_set_flashs(0, LED_POS_MAX, 0);//ȡ����˸
                }

                if(page_menu_cnt == PAGE_MENU_SATSLEEP)
                {
                    system_cfg_copy(&syscfg_t, &syscfg_set);
                    system_cfg_write(&syscfg_t);
                    //system_cfg_apply(&syscfg_t);
                    //�������ú�������ֹͣ��˸
                    led_set_flashs(0, LED_POS_MAX, 0);//ȡ����˸
                }

                if(page_menu_cnt == PAGE_MENU_MAIN)
                {
                    main_mode_cnt++;
                    if(main_mode_cnt >= MAIN_MODE_MAX)
                    {
                        main_mode_cnt = MAIN_MODE_MIN;
                    }

                    if(main_mode_cnt != MAIN_MODE_TIME)
                    {
                        main_mode_force = 1;
                    }
                }
            }
            break;
            case KEY_BTN_NEXT_ENTRY://��һ����Ŀ
            {
                if(page_menu_cnt == PAGE_MENU_MAIN)
                {
                    if(main_mode_cnt == MAIN_MODE_TIME)
                    {
                        main_page_cnt++;
                        if(main_page_cnt >= MAIN_PAGE_MAX)
                        {
                            main_page_cnt = MAIN_PAGE_MIN;
                        }
                    }
                }
                if(page_menu_cnt == PAGE_MENU_STIME)
                {
                    settime_page_cnt++;
                    if(settime_page_cnt >= SET_TIME_PAGE_MAX)
                    {
                        settime_page_cnt = SET_TIME_PAGE_MIN;
                    }
                }
                if(page_menu_cnt == PAGE_MENU_SDATE)
                {
                    setdate_page_cnt++;
                    if(setdate_page_cnt >= SET_DATE_PAGE_MAX)
                    {
                        setdate_page_cnt = SET_DATE_PAGE_MIN;
                    }
                }
                if(page_menu_cnt == PAGE_MENU_SYSCFG)
                {
                    setsys_page_cnt++;
                    if(setsys_page_cnt >= SET_SYSTEM_PAGE_MAX)
                    {
                        setsys_page_cnt = SET_SYSTEM_PAGE_MIN;
                    }
                }

                if(page_menu_cnt == PAGE_MENU_SATSLEEP)
                {
                    setauto_sleep_cnt++;
                    if(setauto_sleep_cnt >= SET_AUTO_SLEEP_MAX)
                    {
                        setauto_sleep_cnt = SET_AUTO_SLEEP_MIN;
                    }
                }
            }
            break;
            case KEY_BTN_UP:
            {
                if(page_menu_cnt == PAGE_MENU_MAIN)
                {
                    main_mode_cnt++;
                    if(main_mode_cnt >= MAIN_MODE_MAX)
                    {
                        main_mode_cnt = MAIN_MODE_MIN;
                    }
                }
                if((page_menu_cnt == PAGE_MENU_STIME) || 
                   (page_menu_cnt == PAGE_MENU_SDATE) || 
                   (page_menu_cnt == PAGE_MENU_SYSCFG) ||
                   (page_menu_cnt == PAGE_MENU_SATSLEEP))
                {
                    set_oper = SET_OPER_ADD;
                    //���ý����У���������ı����������˸
                    led_set_flashs(2, 2, DEF_SYS_LED_FLASH);//��˸
                }
            }
            break;
            case KEY_BTN_DWN:
            {
                if(page_menu_cnt == PAGE_MENU_MAIN)
                {
                    main_mode_cnt--;
                    if(main_mode_cnt < MAIN_MODE_MIN)
                    {
                        main_mode_cnt = MAIN_MODE_MAX-1;
                    }
                }
                if((page_menu_cnt == PAGE_MENU_STIME) || 
                   (page_menu_cnt == PAGE_MENU_SDATE) || 
                   (page_menu_cnt == PAGE_MENU_SYSCFG) ||
                   (page_menu_cnt == PAGE_MENU_SATSLEEP))
                {
                    set_oper = SET_OPER_SUB;
                    //���ý����У���������ı����������˸
                    led_set_flashs(2, 2, DEF_SYS_LED_FLASH);//��˸
                }
            }
            break;
            case KEY_BTN_DWN_LP:
            {
                if(page_menu_cnt == PAGE_MENU_MAIN)
                {
                    main_mode_cnt--;
                    if(main_mode_cnt < MAIN_MODE_MIN)
                    {
                        main_mode_cnt = MAIN_MODE_MAX-1;
                    }

                    if(main_mode_cnt != MAIN_MODE_TIME)
                    {
                        main_mode_force = 1;
                    }
                }
            }
            break;
            case KEY_BTN_POWER_OFF:
            {
                if(page_menu_cnt == PAGE_MENU_MAIN)
                {
                    page_menu_cnt = PAGE_MENU_POWER;
                    main_autoexit_timer = 0;
                    led_close_door(30);
                }
            }
            break;
            default:
                break;
            }
        }

        main_autoexit_timer++;

        if(main_mode_force)
        {
            main_autoexit_timer = 0;
        }

        if(main_autoexit_timer > DEF_SYS_AUTOEXIT_TMR)
        {
            page_menu_cnt = PAGE_MENU_MAIN;//���˵�������
            main_mode_cnt = MAIN_MODE_TIME;//���˵���ʱģʽ
            main_page_cnt = MAIN_PAGE_HHMM;//���˵�ʱ��ģʽ
            main_autoexit_timer = 0;
        }

        if(page_menu_cnt == PAGE_MENU_MAIN)
        {
            led_set_flashs(0, LED_POS_MAX, 0);//�˵������������˸������ȡ����˸
        }

        ds3231_read_time(&time_t);

        switch(page_menu_cnt)
        {
        case PAGE_MENU_MAIN:
            switch(main_mode_cnt)
            {
            case MAIN_MODE_TIME:
                show_main_page(&time_t, main_page_cnt);
                break;
            case MAIN_MODE_TEMP:
                show_temp_page();
                break;
            case MAIN_MODE_NONLI:
                show_nonli_page(&time_t);
                break;
            }
            if(syscfg_t.autosleep.on)
            {
                if((time_t.hour == syscfg_t.autosleep.s_hour) &&
                   (time_t.minute == syscfg_t.autosleep.s_min))
                {
                    page_menu_cnt = PAGE_MENU_POWER;
                    main_autoexit_timer = 0;
                    led_close_door(30);
                }
            }
            break;
        case PAGE_MENU_STIME:
            show_time_set_page(&time_set, settime_page_cnt, set_oper);
            break;
        case PAGE_MENU_SDATE:
            show_date_set_page(&date_set, setdate_page_cnt, set_oper);
            break;
        case PAGE_MENU_SYSCFG:
            show_system_set_page(&syscfg_set, setsys_page_cnt, set_oper);
            break;
        case PAGE_MENU_SATSLEEP:
            show_autosleep_set_page(&syscfg_set, setauto_sleep_cnt, set_oper);
            break;
        case PAGE_MENU_POWER:
            led_clear(0);
            led_close();
            main_autoexit_timer = 0;
            if(syscfg_t.autosleep.on)
            {
                if((time_t.hour == syscfg_t.autosleep.e_hour) &&
                   (time_t.minute == syscfg_t.autosleep.e_min))
                {
                    led_open();
                    page_menu_cnt = PAGE_MENU_MAIN;
                    main_autoexit_timer = DEF_SYS_AUTOEXIT_TMR+1;
                    led_open_door(30);
                }
            }
            break;
        default:
            break;
        }

        set_oper = SET_OPER_MIN;

        led_update();
        delay_xms(DEF_SYS_LED_FPS);
    }
#endif

#if 0
    uint8_t temp_str[LED_POS_MAX+1] = {0};
    uint16 temp = 0;

    key_init();
    led_init();
    ds3231_init();

    led_set_mirror(DEF_SYS_LED_MIRROR);
    key_set_mirror(DEF_SYS_LED_MIRROR);
    led_open_door(50);
    led_close_door(50);

    while(1)
    {
        if(ds18b20_convert(&temp))
        {
            sprintf(temp_str, "%04s", "EEEE");
        }
        else
        {
            sprintf(temp_str, "%04X", temp);
        }
        temp_str[LED_POS_MAX] = 0;
        led_puts(0, temp_str, 1); // ���϶�
        led_putb(1, LED_SEGB_SET(LED_SEGB_DP), 1, 1);//С����
        led_update();
        delay_xms(100);
    }
#endif

#if 0
    uint8 sign = 0, disp_temp = 0;
    uint16 interger = 0, decimal = 0;

    key_init();
    led_init();
    ds3231_init();

    led_set_mirror(DEF_SYS_LED_MIRROR);
    key_set_mirror(DEF_SYS_LED_MIRROR);
    led_open_door(50);
    led_close_door(50);

    while(1)
    {
        if(ds18b20_get_temp(&sign, &interger, &decimal) == RTN_OK)
        {
            if(sign)
            {
                disp_temp = ((decimal >= 5) ? (interger + 1) : interger);
                disp_temp %= 100;
                led_puti(0, LED_SEG_HL, 1);
                led_puti(1, disp_temp / 10, 1);
                led_puti(2, disp_temp % 10, 1);
            }
            else
            {
                disp_temp = (interger % 100);
                led_puti(0, disp_temp / 10, 1);
                led_puti(1, disp_temp % 10, 1);
                led_puti(2, decimal % 10, 1);
                if(led_get_mirror())
                {
                    led_putb(2, LED_SEGB_SET(LED_SEGB_DP), 1, 1);
                }
                else
                {
                    led_putb(1, LED_SEGB_SET(LED_SEGB_DP), 1, 1);
                }
            }
            led_puti(3, LED_SEG_CEL, 1); // ���϶�
        }
        led_update();
        delay_xms(100);
    }
#endif

#if 0
    led_set_mirror(1);

    led_open_door();
    led_close_door();

    led_clear(0);

    led_set_flashc(1, 5);
    led_set_flashc(3, 5);
    led_puts(0, "ABCD", 1);

    while(1)
    {
        led_update();
        delay_xms(100);
    }
#endif

#if 0

    char i = 0, j = 0;

    uchar led_segb[4] =
    {
        LED_SEGB_SET(LED_SEGB_A) | LED_SEGB_SET(LED_SEGB_D) | LED_SEGB_SET(LED_SEGB_G),
        LED_SEGB_SET(LED_SEGB_B) | LED_SEGB_SET(LED_SEGB_C),
        LED_SEGB_SET(LED_SEGB_E) | LED_SEGB_SET(LED_SEGB_F),
        LED_SEGB_SET(LED_SEGB_G) | LED_SEGB_SET(LED_SEGB_B) | LED_SEGB_SET(LED_SEGB_C) | LED_SEGB_SET(LED_SEGB_DP),
    };

    led_init();
    while(1)
    {
        led_clear(0);
        led_update();

        for(i = 0; i < LED_POS_MAX; i++)
        {
            for(j = 0; j < LED_SEGB_MAX; j++)
            {
                led_set_point(i, j, 1);
                led_update();
                delay_xms(50);
            }
        }
        for(i = LED_POS_MAX - 1; i >= 0; i--)
        {
            for(j = LED_SEGB_MAX - 1; j >= 0; j--)
            {
                led_set_point(i, j, 0);
                led_update();
                delay_xms(50);
            }
        }

        led_clear(0);
        led_puts(0, "1234", 1);
        led_update();
        delay_xms(1000);

        led_puts(0, "ABCD", 1);
        led_update();
        delay_xms(1000);

        led_clear(0);

        for(i = 0; i < LED_POS_MAX; i++)
        {
            j = ((i >= 2) ? 0 : 1);
            led_putb(i, led_segb[0], 1, j);
        }
        led_update();
        delay_xms(1000);
        for(i = 0; i < LED_POS_MAX; i++)
        {
            j = ((i >= 2) ? 0 : 1);
            led_putb(i, led_segb[1], 1, j);
        }
        led_update();
        delay_xms(1000);
        for(i = 0; i < LED_POS_MAX; i++)
        {
            j = ((i >= 2) ? 0 : 1);
            led_putb(i, led_segb[2], 1, j);
        }
        led_update();
        delay_xms(1000);
        for(i = 0; i < LED_POS_MAX; i++)
        {
            j = ((i >= 2) ? 0 : 1);
            led_putb(i, led_segb[3], 1, j);
        }
        led_update();
        delay_xms(1000);
    }
#endif


#if 0
    uint8_t sign = 0;
    uint16_t interger = 0;
    uint16_t decimal = 0;
    uchar temp_str[LED_POS_MAX] = {0};
    led_init();
    ds18b20_init();
    led_puts(0, "A1B2", 1);
    led_update();

    while(1)
    {
        ds18b20_get_temp(&sign, &interger, &decimal);
        sprintf(temp_str, "%04d", interger);
        led_puts(0, temp_str, 1);
        led_update();
        delay_xms(100);
    }
#endif
}
