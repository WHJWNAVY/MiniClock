#include "gpio_i2c.h"

sbit GPIO_I2C_SDA = P3 ^ 4;
sbit GPIO_I2C_SCL = P3 ^ 3;

/*********I2C��ʱ4us***********/
void gpio_i2c_wait(void) // 5us, @11.0592MHz
{
    uchar i;
    _nop_();
    _nop_();
    _nop_();
    i = 10;
    while (--i)
        ;
}

void gpio_i2c_nop(void) {
    _nop_();
    _nop_();
}

void gpio_i2c_init(void) {
    GPIO_I2C_SDA = 1;
    GPIO_I2C_SCL = 1;
    delay_xms(10);
}

/********����I2C����********/
uchar gpio_i2c_start(void) {
    GPIO_I2C_SDA = 1;
    GPIO_I2C_SCL = 1;
    gpio_i2c_wait();
    // if(!GPIO_I2C_SDA)
    //{
    //    return RTN_I2C_ERR;   //SDA��Ϊ�͵�ƽ������æ,�˳�
    //}
    GPIO_I2C_SDA = 0;
    gpio_i2c_wait();
    // while(GPIO_I2C_SDA)
    //{
    //    return RTN_I2C_ERR; //SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
    //}
    GPIO_I2C_SCL = 0;
    gpio_i2c_wait();
    return RTN_I2C_OK;
}

/********�ر�I2C����*******/
void gpio_i2c_stop(void) {
    GPIO_I2C_SDA = 0;
    // GPIO_I2C_SCL = 0;
    gpio_i2c_wait();
    GPIO_I2C_SCL = 1;
    gpio_i2c_wait();
    GPIO_I2C_SDA = 1;
    gpio_i2c_wait();
}

/*********����ACK*********/
void gpio_i2c_ack(void) {
    GPIO_I2C_SDA = 0;
    GPIO_I2C_SCL = 0;
    gpio_i2c_wait();
    GPIO_I2C_SCL = 1;
    gpio_i2c_wait();
    GPIO_I2C_SCL = 0;
}

/*********����NOACK*********/
void gpio_i2c_nack(void) {
    GPIO_I2C_SDA = 1;
    GPIO_I2C_SCL = 0;
    gpio_i2c_wait();
    GPIO_I2C_SCL = 1;
    gpio_i2c_wait();
    GPIO_I2C_SCL = 0;
}

/*********��ȡACK�ź�*********/
uchar gpio_i2c_waitack(void) //����Ϊ:1=��ACK,0=��ACK
{
    uchar cnt = 0;
    GPIO_I2C_SCL = 0;
    GPIO_I2C_SDA = 1; //����SDAΪ���루�������͵ĵ�Ƭ����Ҫ����IO��������Ĵ�����
    gpio_i2c_wait();
    GPIO_I2C_SCL = 1;
    gpio_i2c_wait();
    while (GPIO_I2C_SDA) {
        if ((cnt++) > 200) {
            GPIO_I2C_SCL = 0;
            gpio_i2c_wait();
            return RTN_I2C_ERR;
        }
    }
    GPIO_I2C_SCL = 0;
    gpio_i2c_wait();
    return RTN_I2C_OK;
}

/************I2C����һ���ֽ�*************/
void gpio_i2c_sendbyte(uchar demand) //���ݴӸ�λ����λ//
{
    uchar i = 8;

    while (i--) {
        GPIO_I2C_SCL = 0;
        gpio_i2c_nop();
        GPIO_I2C_SDA = (bit)(demand & 0x80);
        demand <<= 1;
        gpio_i2c_wait();
        GPIO_I2C_SCL = 1;
        gpio_i2c_wait();
    }
    GPIO_I2C_SCL = 0;
}

/*********I2C����һ�ֽ�*********/
uchar gpio_i2c_recvbyte(void) //���ݴӸ�λ����λ//
{
    uchar i = 8;
    uchar ddata = 0;
    GPIO_I2C_SDA = 1; //����SDAΪ���루�������͵ĵ�Ƭ����Ҫ����IO��������Ĵ�����
    while (i--) {
        ddata <<= 1; //���ݴӸ�λ��ʼ��ȡ
        GPIO_I2C_SCL = 0;
        gpio_i2c_wait();
        GPIO_I2C_SCL = 1;
        gpio_i2c_wait(); //�Ӹ�λ��ʼ ddata|=GPIO_I2C_SDA;ddata<<=1
        if (GPIO_I2C_SDA) {
            ddata |= 0x01;
        }
    }
    GPIO_I2C_SCL = 0;
    return ddata;
}

/******I2C��ָ����ַдһ���ֽ�******/
uchar gpio_i2c_write(uchar dev_addr, uchar addr, uchar dat) {
    if (gpio_i2c_start() != RTN_I2C_OK) {
        return RTN_I2C_ERR;
    }
    gpio_i2c_sendbyte(dev_addr);
    gpio_i2c_waitack();
    gpio_i2c_sendbyte(addr); //����д��ַ
    gpio_i2c_waitack();
    gpio_i2c_sendbyte(dat); //д����
    gpio_i2c_waitack();
    gpio_i2c_stop();

    return RTN_I2C_OK;
}

/******I2C��ָ����ַ��һ���ֽ�******/
uchar gpio_i2c_read(uchar dev_addr, uchar addr) {
    uchar dat;

    if (gpio_i2c_start() != RTN_I2C_OK) {
        return RTN_I2C_ERR;
    }
    gpio_i2c_sendbyte(dev_addr);
    if (gpio_i2c_waitack() != RTN_I2C_OK) {
        gpio_i2c_stop();
        return RTN_I2C_ERR;
    }
    gpio_i2c_sendbyte(addr); //����Ҫ���ĵ�ַ
    gpio_i2c_waitack();
    gpio_i2c_start(); //�ٴη��Ϳ�ʼ
    gpio_i2c_sendbyte(dev_addr + 1);
    gpio_i2c_waitack();
    dat = gpio_i2c_recvbyte(); //������
    gpio_i2c_nack();
    gpio_i2c_stop();
    return dat;
}