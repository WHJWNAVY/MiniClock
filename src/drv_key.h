#ifndef __DRV_KEY_H__
#define __DRV_KEY_H__

#include "common.h"

typedef enum key_btn_e
{
    KEY_BTN_NULL = /*0xFFu*/0,   //short press
    KEY_BTN_SET  = 0xAAu,
    KEY_BTN_UP   = 0xBBu,
    KEY_BTN_DWN  = 0xCCu,

    KEY_BTN_SET_LP  = 0xABu, //long press
    KEY_BTN_UP_LP   = 0xBCu,
    KEY_BTN_DWN_LP  = 0xCDu,

    KEY_BTN_SET_LLP = 0xDEu,// long long press
    KEY_BTN_UP_LLP  = 0xDFu,// long long press
    KEY_BTN_DWN_LLP = 0xE0u,// long long press

    KEY_BTN_SAVE_SETT = KEY_BTN_UP_LP,
    KEY_BTN_NEXT_PAGE = KEY_BTN_SET_LP,
    KEY_BTN_NEXT_ENTRY = KEY_BTN_SET,

    KEY_BTN_POWER_OFF = KEY_BTN_SET_LLP,
} key_btn_t;

extern void key_init(void);
extern uchar key_get_code(void);
extern void key_set_mirror(uint8 mirror);

#endif