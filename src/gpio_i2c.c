#include "gpio_i2c.h"

sbit GPIO_I2C_SDA    = P3 ^ 4;
sbit GPIO_I2C_SCL    = P3 ^ 3;

/*********I2C延时4us***********/
void gpio_i2c_wait(void)//5us, @11.0592MHz
{
    uchar i;
    _nop_();
    _nop_();
    _nop_();
    i = 10;
    while (--i);
}

void gpio_i2c_nop(void)
{
    _nop_();
    _nop_();
}

void gpio_i2c_init(void)
{
    GPIO_I2C_SDA = 1;
    GPIO_I2C_SCL = 1;
    delay_xms(10);
}

/********开启I2C总线********/
uchar gpio_i2c_start(void)
{
    GPIO_I2C_SDA = 1;
    GPIO_I2C_SCL = 1;
    gpio_i2c_wait();
    //if(!GPIO_I2C_SDA)
    //{
    //    return RTN_I2C_ERR;   //SDA线为低电平则总线忙,退出
    //}
    GPIO_I2C_SDA = 0;
    gpio_i2c_wait();
    //while(GPIO_I2C_SDA)
    //{
    //    return RTN_I2C_ERR; //SDA线为高电平则总线出错,退出
    //}
    GPIO_I2C_SCL = 0;
    gpio_i2c_wait();
    return RTN_I2C_OK;
}

/********关闭I2C总线*******/
void gpio_i2c_stop(void)
{
    GPIO_I2C_SDA = 0;
    //GPIO_I2C_SCL = 0;
    gpio_i2c_wait();
    GPIO_I2C_SCL = 1;
    gpio_i2c_wait();
    GPIO_I2C_SDA = 1;
    gpio_i2c_wait();
}

/*********发送ACK*********/
void gpio_i2c_ack(void)
{
    GPIO_I2C_SDA = 0;
    GPIO_I2C_SCL = 0;
    gpio_i2c_wait();
    GPIO_I2C_SCL = 1;
    gpio_i2c_wait();
    GPIO_I2C_SCL = 0;
}

/*********发送NOACK*********/
void gpio_i2c_nack(void)
{
    GPIO_I2C_SDA = 1;
    GPIO_I2C_SCL = 0;
    gpio_i2c_wait();
    GPIO_I2C_SCL = 1;
    gpio_i2c_wait();
    GPIO_I2C_SCL = 0;
}

/*********读取ACK信号*********/
uchar gpio_i2c_waitack(void)     //返回为:1=有ACK,0=无ACK
{
    uchar cnt = 0;
    GPIO_I2C_SCL = 0;
    GPIO_I2C_SDA = 1;        //设置SDA为输入（其它类型的单片机需要配置IO输入输出寄存器）
    gpio_i2c_wait();
    GPIO_I2C_SCL = 1;
    gpio_i2c_wait();
    while(GPIO_I2C_SDA)
    {
        if((cnt++) > 200)
        {
            GPIO_I2C_SCL = 0;
            gpio_i2c_wait();
            return RTN_I2C_ERR;
        }
    }
    GPIO_I2C_SCL = 0;
    gpio_i2c_wait();
    return RTN_I2C_OK;
}

/************I2C发送一个字节*************/
void gpio_i2c_sendbyte(uchar demand) //数据从高位到低位//
{
    uchar i = 8;

    while(i--)
    {
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

/*********I2C读入一字节*********/
uchar gpio_i2c_recvbyte(void)      //数据从高位到低位//
{
    uchar i = 8;
    uchar ddata = 0;
    GPIO_I2C_SDA = 1;            //设置SDA为输入（其它类型的单片机需要配置IO输入输出寄存器）
    while(i--)
    {
        ddata <<= 1;    //数据从高位开始读取
        GPIO_I2C_SCL = 0;
        gpio_i2c_wait();
        GPIO_I2C_SCL = 1;
        gpio_i2c_wait();  //从高位开始 ddata|=GPIO_I2C_SDA;ddata<<=1
        if(GPIO_I2C_SDA)
        {
            ddata |= 0x01;
        }
    }
    GPIO_I2C_SCL = 0;
    return ddata;
}

/******I2C向指定地址写一个字节******/
uchar gpio_i2c_write(uchar dev_addr, uchar addr, uchar dat)
{
    if(gpio_i2c_start() != RTN_I2C_OK)
    {
        return RTN_I2C_ERR;
    }
    gpio_i2c_sendbyte(dev_addr);
    gpio_i2c_waitack();
    gpio_i2c_sendbyte(addr);       //设置写地址
    gpio_i2c_waitack();
    gpio_i2c_sendbyte(dat);      //写数据
    gpio_i2c_waitack();
    gpio_i2c_stop();

    return RTN_I2C_OK;
}

/******I2C从指定地址读一个字节******/
uchar gpio_i2c_read(uchar dev_addr, uchar addr)
{
    uchar dat;

    if(gpio_i2c_start() != RTN_I2C_OK)
    {
        return RTN_I2C_ERR;
    }
    gpio_i2c_sendbyte(dev_addr);
    if(gpio_i2c_waitack() != RTN_I2C_OK)
    {
        gpio_i2c_stop();
        return RTN_I2C_ERR;
    }
    gpio_i2c_sendbyte(addr);       //设置要读的地址
    gpio_i2c_waitack();
    gpio_i2c_start();              //再次发送开始
    gpio_i2c_sendbyte(dev_addr + 1);
    gpio_i2c_waitack();
    dat = gpio_i2c_recvbyte();     //读数据
    gpio_i2c_nack();
    gpio_i2c_stop();
    return dat;
}