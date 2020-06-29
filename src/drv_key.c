#include "drv_key.h"

sbit KEY_BTN_0 = P1 ^ 1;
sbit KEY_BTN_1 = P1 ^ 0;
sbit KEY_BTN_2 = P3 ^ 7;

#define KEY_BTN_SHORT_PRESS_CNT (10u)
#define KEY_BTN_LONG_PRESS_CNT (1000u)
#define KEY_BTN_LLONG_PRESS_CNT (2200u)

uint8 g_key_code = KEY_BTN_NULL;
uint8 g_key_miiror = 0;

void timer1_init(void) // 1����@27MHz
{
    AUXR |= 0x40; //��ʱ��ʱ��1Tģʽ
    TMOD &= 0x0F; //���ö�ʱ��ģʽ
    TMOD |= 0x10; //���ö�ʱ��ģʽ
    TL1 = 0x88;   //���ö�ʱ��ֵ
    TH1 = 0x96;   //���ö�ʱ��ֵ
    TF1 = 0;      //���TF1��־
    ET1 = 1;      //ʹ�ܶ�ʱ��0�ж�
    EA = 1;       //ʹ�����ж�
    TR1 = 1;      //��ʱ��1��ʼ��ʱ
}

void key_timer1(void) interrupt 3 {
    static uint16 key_0_tmr = 0;
    static uint16 key_1_tmr = 0;
    static uint16 key_2_tmr = 0;

    TL1 = 0x88; //���ö�ʱ��ֵ
    TH1 = 0x96; //���ö�ʱ��ֵ
    TF1 = 0;    //���TF1��־

    if (g_key_code == KEY_BTN_NULL) {
        if (KEY_BTN_0 == 0) {
            key_0_tmr++;
        } else {
            if (key_0_tmr > KEY_BTN_LLONG_PRESS_CNT) {
                g_key_code = (g_key_miiror ? KEY_BTN_DWN_LLP : KEY_BTN_SET_LLP);
            } else if (key_0_tmr > KEY_BTN_LONG_PRESS_CNT) {
                g_key_code = (g_key_miiror ? KEY_BTN_DWN_LP : KEY_BTN_SET_LP);
            } else if (key_0_tmr > KEY_BTN_SHORT_PRESS_CNT) {
                g_key_code = (g_key_miiror ? KEY_BTN_DWN : KEY_BTN_SET);
            }

            key_0_tmr = 0;
        }

        if (KEY_BTN_1 == 0) {
            key_1_tmr++;
        } else {
            if (key_1_tmr > KEY_BTN_LLONG_PRESS_CNT) {
                g_key_code = KEY_BTN_UP_LLP;
            } else if (key_1_tmr > KEY_BTN_LONG_PRESS_CNT) {
                g_key_code = KEY_BTN_UP_LP;
            } else if (key_1_tmr > KEY_BTN_SHORT_PRESS_CNT) {
                g_key_code = KEY_BTN_UP;
            }

            key_1_tmr = 0;
        }

        if (KEY_BTN_2 == 0) {
            key_2_tmr++;
        } else {
            if (key_2_tmr > KEY_BTN_LLONG_PRESS_CNT) {
                g_key_code = (g_key_miiror ? KEY_BTN_SET_LLP : KEY_BTN_DWN_LLP);
            } else if (key_2_tmr > KEY_BTN_LONG_PRESS_CNT) {
                g_key_code = (g_key_miiror ? KEY_BTN_SET_LP : KEY_BTN_DWN_LP);
            } else if (key_2_tmr > KEY_BTN_SHORT_PRESS_CNT) {
                g_key_code = (g_key_miiror ? KEY_BTN_SET : KEY_BTN_DWN);
            }

            key_2_tmr = 0;
        }
    } else {
        key_0_tmr = 0;
        key_1_tmr = 0;
        key_2_tmr = 0;
    }
}

void key_init(void) {
    g_key_code = KEY_BTN_NULL;
    g_key_miiror = KEY_BTN_NULL;
    timer1_init();
}

uchar key_get_code(void) {
    uint8 keycode = KEY_BTN_NULL;

    if ((g_key_code >= KEY_BTN_CODE_MIN) && (g_key_code <= KEY_BTN_CODE_MAX)) {
        keycode = g_key_code;
    } else {
        keycode = KEY_BTN_NULL;
    }

    g_key_code = KEY_BTN_NULL;

    return keycode;
}

void key_set_mirror(uint8 mirror) { g_key_miiror = ((mirror) ? 1 : 0); }