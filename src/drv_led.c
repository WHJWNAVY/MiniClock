#include "drv_led.h"

sbit LED_CLK = P1 ^ 2;
sbit LED_CS = P1 ^ 3;
sbit LED_DIN = P1 ^ 4;

#define MAX7219_ADDR_NONE (0x00)
#define MAX7219_ADDR_DIG0 (0x01)
#define MAX7219_ADDR_DIG1 (0x02)
#define MAX7219_ADDR_DIG2 (0x03)
#define MAX7219_ADDR_DIG3 (0x04)
#define MAX7219_ADDR_DIG4 (0x05)
#define MAX7219_ADDR_DIG5 (0x06)
#define MAX7219_ADDR_DIG6 (0x07)
#define MAX7219_ADDR_DIG7 (0x08)
#define MAX7219_ADDR_DECODE_MODE (0x09)  //解码模式
#define MAX7219_ADDR_INTENSITY (0x0A)    //亮度设置
#define MAX7219_ADDR_SCAN_LIMIT (0x0B)   //扫描限位
#define MAX7219_ADDR_SHUTDOWN (0x0C)     //关闭显示
#define MAX7219_ADDR_DISPTEST (0x0F)     //测试模式

#define MAX7219_DECODE_MODE_NONE (0x00)  //不解码
#define MAX7219_DECODE_MODE_DIG0 (0x01)  //解码位0
#define MAX7219_DECODE_MODE_0_3 (0x0F)   //解码位0-3
#define MAX7219_DECODE_MODE_0_7 (0xFF)   //解码位0-7

#define MAX7219_SCAN_LIMIT_MAX (LED_POS_MAX - 1)  //只扫描前四个

#define LED_SEG_NUM_HEX_MAX (0xFFFFu)
#define LED_SEG_NUM_OCT_MAX (9999u)

static uchar g_led_mirror = 0;
uchar LED_DISP_MEM[LED_POS_MAX] = {0};
uint16 LED_FLASH_TMR[LED_POS_MAX] = {0};  //闪烁

#define LED_MIRROR_POS(pos) \
    (pos = (g_led_mirror ? (LED_POS_MAX - 1 - pos) : pos))
#define LED_MIRROR_SEG(seg) (seg = (g_led_mirror ? LED_SEGB_MIRROR[seg] : seg))

uchar code LED_SEGB_MIRROR[LED_SEGB_MAX] = {LED_SEGB_G, LED_SEGB_C, LED_SEGB_B,
                                            LED_SEGB_A, LED_SEGB_F, LED_SEGB_E,
                                            LED_SEGB_D, LED_SEGB_DP};

#define u8bset LED_SEGB_SET
#define _sa LED_SEGB_A
#define _sb LED_SEGB_B
#define _sc LED_SEGB_C
#define _sd LED_SEGB_D
#define _se LED_SEGB_E
#define _sf LED_SEGB_F
#define _sg LED_SEGB_G
#define _sp LED_SEGB_DP

uchar code LED_SEG_CODE[LED_SEG_MAX] = {
    u8bset(_sa) | u8bset(_sb) | u8bset(_sc) | u8bset(_sd) | u8bset(_se) |
        u8bset(_sf),            // 0x7E,// 0
    u8bset(_sb) | u8bset(_sc),  // 0x30,// 1 |
    u8bset(_sa) | u8bset(_sb) | u8bset(_sg) | u8bset(_se) |
        u8bset(_sd),  // 0x6D,// 2
    u8bset(_sa) | u8bset(_sb) | u8bset(_sg) | u8bset(_sc) |
        u8bset(_sd),                                        // 0x79,// 3
    u8bset(_sf) | u8bset(_sg) | u8bset(_sb) | u8bset(_sc),  // 0x33,// 4
    u8bset(_sa) | u8bset(_sf) | u8bset(_sg) | u8bset(_sc) |
        u8bset(_sd),  // 0x5B,// 5
    u8bset(_sa) | u8bset(_sf) | u8bset(_se) | u8bset(_sd) | u8bset(_sc) |
        u8bset(_sg),                          // 0x5F,// 6
    u8bset(_sa) | u8bset(_sb) | u8bset(_sc),  // 0x70,// 7
    u8bset(_sa) | u8bset(_sb) | u8bset(_sc) | u8bset(_sd) | u8bset(_se) |
        u8bset(_sf) | u8bset(_sg),  // 0x7F,// 8
    u8bset(_sa) | u8bset(_sf) | u8bset(_sg) | u8bset(_sb) | u8bset(_sc) |
        u8bset(_sd),  // 0x7B,// 9
    u8bset(_sa) | u8bset(_sf) | u8bset(_se) | u8bset(_sb) | u8bset(_sc) |
        u8bset(_sg),  // 0x77,// A
    u8bset(_sf) | u8bset(_se) | u8bset(_sd) | u8bset(_sc) |
        u8bset(_sg),                                        // 0x1F,// b
    u8bset(_sa) | u8bset(_sf) | u8bset(_se) | u8bset(_sd),  // 0x4E,// C [
    u8bset(_sb) | u8bset(_sc) | u8bset(_sd) | u8bset(_se) |
        u8bset(_sg),  // 0x3D,// d
    u8bset(_sa) | u8bset(_sf) | u8bset(_se) | u8bset(_sd) |
        u8bset(_sg),                                        // 0x4F,// E
    u8bset(_sa) | u8bset(_sf) | u8bset(_se) | u8bset(_sg),  // 0x47,// F
    u8bset(_sa) | u8bset(_sf) | u8bset(_se) | u8bset(_sd) |
        u8bset(_sc),                                        // 0x5E,// G
    u8bset(_sa) | u8bset(_sb) | u8bset(_sc) | u8bset(_sd),  // 0x78,// ]
    u8bset(_sf) | u8bset(_se) | u8bset(_sc) | u8bset(_sb) |
        u8bset(_sg),                                        // 0x37,// H
    u8bset(_sf) | u8bset(_se) | u8bset(_sc) | u8bset(_sg),  // 0x17,// h
    u8bset(_sg) | u8bset(_se) | u8bset(_sd),                // 0x0D,// c
    u8bset(_sf) | u8bset(_se) | u8bset(_sd),                // 0x0E,// L
    u8bset(_sa) | u8bset(_sb) | u8bset(_sg) | u8bset(_sf) |
        u8bset(_se),                                        // 0x67,// P
    u8bset(_sg) | u8bset(_sc) | u8bset(_sd) | u8bset(_se),  // 0x1D,// o
    u8bset(_sa) | u8bset(_sf),                              // 0x42,//AF
    u8bset(_sa) | u8bset(_sb),                              // 0x60,//AB
    u8bset(_sd) | u8bset(_se),                              // 0x0C,//DE
    u8bset(_sc) | u8bset(_sd),                              // 0x18,//CD
    u8bset(_sb) | u8bset(_sc) | u8bset(_sg),                // 0x31,//BCG
    u8bset(_se) | u8bset(_sf) | u8bset(_sg),                // 0x07,//EFG
    u8bset(_sp),                                            // 0x80,// DP段
    u8bset(_sa),                                            // 0x40,// A段
    u8bset(_sb),                                            // 0x20,// B段
    u8bset(_sc),                                            // 0x10,// C段
    u8bset(_sd),                                            // 0x08,// D段
    u8bset(_se),                                            // 0x04,// E段
    u8bset(_sf),                                            // 0x02,// F段
    u8bset(_sg),                                            // 0x01,// G段 -
    0x00,                                                   // Space
};

void max7219_writeb(uchar dat) {
    uchar i;
    // LED_CS = 0;
    for (i = 8; i >= 1; i--) {
        LED_CLK = 0;
        LED_DIN = ((dat & 0x80) ? 0xFF : 0x00);
        dat = dat << 1;
        LED_CLK = 1;
    }
    // LED_CS = 1;
}

void max7219_write(uchar add, uchar dat) {
    uchar i = 0;
    LED_CS = 0;
    _nop_();
    max7219_writeb(add);
    _nop_();
    max7219_writeb(dat);
    _nop_();
    LED_CS = 1;
}

void max7219_init(void) {
    max7219_write(MAX7219_ADDR_DECODE_MODE, MAX7219_DECODE_MODE_NONE);
    max7219_write(MAX7219_ADDR_INTENSITY, LED_MAX_LIGHT);
    max7219_write(MAX7219_ADDR_SCAN_LIMIT,
                  MAX7219_SCAN_LIMIT_MAX);  // Display digits 0 1 2 3
    max7219_write(MAX7219_ADDR_SHUTDOWN, 0x01);
    max7219_write(MAX7219_ADDR_DISPTEST, 0x00);
}

typedef enum led_open_e {
    LED_OPEN_NULL = 0,
    LED_OPEN_TRUE,
    LED_OPEN_FALSE,
} led_open_t;

uint8 g_led_open = LED_OPEN_NULL;

void led_open(void) {
    if (g_led_open != LED_OPEN_TRUE) {
        max7219_write(MAX7219_ADDR_SHUTDOWN, 0x01);  //正常模式(掉电模式关)
        g_led_open = LED_OPEN_TRUE;
    }
}

void led_close(void) {
    if (g_led_open != LED_OPEN_FALSE) {
        max7219_write(MAX7219_ADDR_SHUTDOWN, 0x00);  //正常模式(掉电模式开)
        g_led_open = LED_OPEN_FALSE;
    }
}

uchar g_led_light = 0xff;
void led_set_light(uchar n) {
    if (g_led_light != n) {
        g_led_light = n;
        n = ((n > 0x0f) ? 0x0f : n);  // 0-15
        max7219_write(MAX7219_ADDR_INTENSITY, n);
    }
}

void led_set_mirror(uchar mirror) { g_led_mirror = (mirror ? 1 : 0); }

uint8 led_get_mirror(void) { return g_led_mirror; }

uchar led_mirror_segb(uchar segb) {
    uchar segbc = 0, i = 0;
    for (i = 0; i < LED_SEGB_MAX; i++) {
        if (segb & (0x01 << i)) {
            segbc |= (0x01 << LED_SEGB_MIRROR[i]);
        }
    }

    return segbc;
}

void led_set_point(uchar pos, uchar seg, uchar col) {
    if ((pos >= LED_POS_MAX) || (seg >= LED_SEGB_MAX)) {
        return;
    }

    LED_MIRROR_POS(pos);
    LED_MIRROR_SEG(seg);

    if (col) {
        LED_DISP_MEM[pos] |= (uchar)(0x01 << seg);
    } else {
        LED_DISP_MEM[pos] &= (uchar)(~(0x01 << seg));
    }
}

uchar led_get_point(uchar pos, uchar seg) {
    uchar ret = 0;

    pos = ((pos >= LED_POS_MAX) ? (LED_POS_MAX - 1) : pos);
    seg = ((seg >= LED_SEGB_MAX) ? (LED_SEGB_MAX - 1) : seg);

    LED_MIRROR_POS(pos);
    LED_MIRROR_SEG(seg);

    ret = (LED_DISP_MEM[pos] & (uchar)(0x01 << seg));
    return ret;
}

void led_update(void) {
    uchar i = 0;

    static uint16 led_pos_flag0[LED_POS_MAX] = 0;
    static uint16 led_pos_flag1[LED_POS_MAX] = 0;

    for (i = 0; i < LED_POS_MAX; i++) {
        if (LED_FLASH_TMR[i]) {
            if (led_pos_flag0[i] < LED_FLASH_TMR[i]) {
                led_pos_flag0[i] += 1;
            } else {
                led_pos_flag0[i] = 0;
                led_pos_flag1[i] = (led_pos_flag1[i] ? 0 : 1);
            }
        } else {
            led_pos_flag0[i] = 0;
            led_pos_flag1[i] = 0;
        }

        if (led_pos_flag1[i]) {
            max7219_write(i + 1, LED_SEG_CODE[LED_SEG_NULL]);
        } else {
            max7219_write(i + 1, LED_DISP_MEM[i]);
        }
    }
}

void led_clear(uchar dat) {
    uchar i = 0;

    for (i = 0; i < LED_POS_MAX; i++) {
        LED_DISP_MEM[i] = dat;
        LED_FLASH_TMR[i] = 0;
    }
}

void led_open_door(uint16 delay) {
    uchar i = 0, j = 0;
    for (i = 0; i < LED_POS_MAX; i++) {
        for (j = 0; j < LED_SEGB_MAX; j++) {
            led_set_point(i, j, 1);
            led_update();
            delay_xms(delay);
        }
    }
}

void led_close_door(uint16 delay) {
    uchar i = 0, j = 0;
    for (i = 0; i < LED_POS_MAX; i++) {
        for (j = 0; j < LED_SEGB_MAX; j++) {
            led_set_point(LED_POS_MAX - 1 - i, LED_SEGB_MAX - 1 - j, 0);
            led_update();
            delay_xms(delay);
        }
    }
}

void led_init(void) {
    max7219_init();

    led_open();
    led_set_light(LED_MAX_LIGHT);
    led_set_mirror(0);

    led_clear(0);
    led_update();
}

uchar led_seg_cidx_get(char chr) {
    uchar idx = 0;

    if ((chr >= '0') && (chr <= '9')) {
        idx = chr - '0' + LED_SEG_0;
    } else if ((chr >= 'A') && (chr <= 'G')) {
        idx = chr - 'A' + LED_SEG_A;
    } else if ((chr >= 'a') && (chr <= 'g')) {
        if (chr == 'c') {
            idx = LED_SEG_CS;
        } else {
            idx = chr - 'a' + LED_SEG_A;
        }
    } else if (chr == '-') {
        idx = LED_SEG_HL;
    } else if (chr == '_') {
        idx = LED_SEG_SD;
    } else if (chr == '[') {
        idx = LED_SEG_LSB;
    } else if (chr == ']') {
        idx = LED_SEG_RSB;
    } else if (chr == '{') {
        idx = LED_SEG_BCG;
    } else if (chr == '}') {
        idx = LED_SEG_EFG;
    } else if (chr == 'H') {
        idx = LED_SEG_H;
    } else if (chr == 'h') {
        idx = LED_SEG_HS;
    } else if ((chr == 'L') || (chr == 'l')) {
        idx = LED_SEG_L;
    } else if ((chr == 'O') || (chr == 'o')) {
        idx = LED_SEG_O;
    } else if ((chr == 'P') || (chr == 'p')) {
        idx = LED_SEG_P;
    } else if (chr == ' ') {
        idx = LED_SEG_NULL;
    }
#if 0
    else if((chr > 0) && (chr <  LED_SEG_MAX))
    {
        idx = LED_SEG_MIN + chr;
    }
#endif
    else {
        idx = LED_SEG_NULL;
    }

    return idx;
}

uchar led_seg_cpy2mem(uchar pos, uchar segc, uchar col, uchar dp) {
    uchar i = 0;
    if (pos >= LED_POS_MAX) {
        return RTN_ERR;
    }

    for (i = 0; i < LED_SEGB_MAX; i++) {
        if (segc & (0x01 << i)) {
            led_set_point(pos, i, col);
        } else {
            led_set_point(pos, i, !col);
        }
    }

    if (dp) {
        led_set_point(pos, LED_SEGB_DP, col);
    } else {
        led_set_point(pos, LED_SEGB_DP, !col);
    }

    return RTN_OK;
}

uchar led_putc_dp(uchar pos, char chr, uchar col, uchar dp) {
    uchar segidx = 0;
    if (pos >= LED_POS_MAX) {
        return RTN_ERR;
    }

    segidx = led_seg_cidx_get(chr);
    return led_seg_cpy2mem(pos, LED_SEG_CODE[segidx], col, dp);
}

uchar led_putc(uchar pos, char chr, uchar col) {
    return led_putc_dp(pos, chr, col, 0);
}

uchar led_puts_dp(uchar pos, const uchar *chs, uchar col, uchar dp) {
    uchar i = 0, p = pos;
    if ((pos >= LED_POS_MAX) || (chs == NULL)) {
        return RTN_ERR;
    }
    while (*chs) {
        if (led_putc_dp(p, *chs, col, dp) == RTN_OK) {
            p++;
            if (p >= LED_POS_MAX) {
                break;
            }
        }
        chs++;
    }

    return RTN_OK;
}

uchar led_puts(uchar pos, const uchar *chs, uchar col) {
    return led_puts_dp(pos, chs, col, 0);
}

//直接显示一个段码
uchar led_putb(uchar pos, uchar segb, uchar col, uchar andor) {
    uchar i = 0;
    if (pos >= LED_POS_MAX) {
        return RTN_ERR;
    }

    for (i = 0; i < LED_SEGB_MAX; i++) {
        if (andor) {
            if (segb & (0x01 << i)) {
                led_set_point(pos, i, col);
            }
        } else {
            if (segb & (0x01 << i)) {
                led_set_point(pos, i, col);
            } else {
                led_set_point(pos, i, !col);
            }
        }
    }

    return RTN_OK;
}

uchar led_putbs(uchar pos, uchar *segb, uchar len, uchar col, uchar andor) {
    uchar i = 0;
    if ((pos >= LED_POS_MAX) || ((pos + len) > LED_POS_MAX) || (segb == NULL)) {
        return RTN_ERR;
    }

    for (i = pos; i < len; i++) {
        led_putb(i, segb[i - pos], col, andor);
    }

    return RTN_OK;
}

//根据段码索引显示
uchar led_puti(uchar pos, uchar segi, uchar col) {
    uchar segb = 0;

    if ((pos >= LED_POS_MAX) || (segi >= LED_SEG_MAX)) {
        return RTN_ERR;
    }

    segb = LED_SEG_CODE[segi];

    // segb = led_mirror_segb(segb);

    led_putb(pos, segb, col, 0);

    return RTN_OK;
}

uchar led_putis(uchar pos, uchar *segi, uchar len, uchar col) {
    uchar i = 0;
    if ((pos >= LED_POS_MAX) || ((pos + len) > LED_POS_MAX) || (segi == NULL)) {
        return RTN_ERR;
    }

    for (i = pos; i < len; i++) {
        led_puti(i, segi[i - pos], col);
    }

    return RTN_OK;
}

uchar led_set_flashc(uchar pos, uint16 flashtmr) {
    if (pos >= LED_POS_MAX) {
        return RTN_ERR;
    }

    LED_MIRROR_POS(pos);

    LED_FLASH_TMR[pos] = flashtmr;

    return RTN_OK;
}

uchar led_set_flashs(uchar pos, uchar len, uint16 flashtmr) {
    uchar i = 0;
    if ((pos >= LED_POS_MAX) || (len > LED_POS_MAX)) {
        return RTN_ERR;
    }

    len += pos;

    len = ((len > LED_POS_MAX) ? LED_POS_MAX : len);

    for (i = pos; i < len; i++) {
        led_set_flashc(i, flashtmr);
    }

    return RTN_OK;
}
