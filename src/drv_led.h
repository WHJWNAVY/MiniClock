#ifndef __DRV_LED_H__
#define __DRV_LED_H__

#include "common.h"

#define LED_POS_MAX (4u)
#define LED_MAX_LIGHT (0x0C)
#define LED_MIN_LIGHT (0x01)

typedef enum led_seg_e {
    LED_SEG_MIN = 0,
    LED_SEG_0 = LED_SEG_MIN,
    LED_SEG_1,
    LED_SEG_2,
    LED_SEG_3,
    LED_SEG_4,
    LED_SEG_5,
    LED_SEG_6,
    LED_SEG_7,
    LED_SEG_8,
    LED_SEG_9,
    LED_SEG_A,
    LED_SEG_B,
    LED_SEG_C,
    LED_SEG_D,
    LED_SEG_E,
    LED_SEG_F,
    LED_SEG_G,
    LED_SEG_RSB, // ]
    LED_SEG_H,   // H
    LED_SEG_HS,  // h
    LED_SEG_CS,
    LED_SEG_L,
    LED_SEG_P,
    LED_SEG_O,
    LED_SEG_AF,
    LED_SEG_AB,
    LED_SEG_DE,
    LED_SEG_CD,
    LED_SEG_BCG,
    LED_SEG_EFG,
    LED_SEG_SDP,
    LED_SEG_SA,
    LED_SEG_SB,
    LED_SEG_SC,
    LED_SEG_SD,
    LED_SEG_SE,
    LED_SEG_SF,
    LED_SEG_SG,
    LED_SEG_NULL,
    LED_SEG_MAX,
    LED_SEG_HL = LED_SEG_SG,
    LED_SEG_LSB = LED_SEG_C,
    LED_SEG_VL = LED_SEG_1,
    LED_SEG_CEL = LED_SEG_CS,
} led_seg_t;

typedef enum led_segb_e {
    LED_SEGB_MIN = 0,
    LED_SEGB_G = LED_SEGB_MIN,
    LED_SEGB_F,
    LED_SEGB_E,
    LED_SEGB_D,
    LED_SEGB_C,
    LED_SEGB_B,
    LED_SEGB_A,
    LED_SEGB_DP,
    LED_SEGB_MAX,
} led_segb_t;

#define LED_SEGB_SET(seg) ((uchar)(0X01 << seg))

extern void led_open(void);
extern void led_close(void);
extern void led_set_light(uchar n);
extern void led_update(void);
extern void led_clear(uchar dat);
extern void led_init(void);

extern void led_set_mirror(uchar mirror);
extern uint8 led_get_mirror(void);
extern uchar led_set_flashc(uchar pos, uint16 flashtmr);
extern uchar led_set_flashs(uchar pos, uchar len, uint16 flashtmr);
extern void led_set_point(uchar pos, uchar seg, uchar col);
extern uchar led_get_point(uchar pos, uchar seg);

extern void led_open_door(uint16 delay);
extern void led_close_door(uint16 delay);

extern uchar led_putc_dp(uchar pos, char chr, uchar col, uchar dp);
extern uchar led_putc(uchar pos, char chr, uchar col);
extern uchar led_puts_dp(uchar pos, const uchar *chs, uchar col, uchar dp);
extern uchar led_puts(uchar pos, const uchar *chs, uchar col);
extern uchar led_putb(uchar pos, uchar segb, uchar col, uchar andor);
extern uchar led_putbs(uchar pos, uchar *segb, uchar len, uchar col,
                       uchar andor);
extern uchar led_puti(uchar pos, uchar segi, uchar col);
extern uchar led_putis(uchar pos, uchar *segi, uchar len, uchar col);

#endif