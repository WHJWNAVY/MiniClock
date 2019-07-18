#ifndef __GPIO_I2C_H__
#define __GPIO_I2C_H__

#include "common.h"

typedef enum gpio_i2c_e
{
    RTN_I2C_OK = 0,
    RTN_I2C_ERR,
} gpio_i2c_t;

void gpio_i2c_init(void);
uchar gpio_i2c_write(uchar dev_addr, uchar addr, uchar dat);
uchar gpio_i2c_read(uchar dev_addr, uchar addr);

#endif