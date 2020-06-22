#include "drv_18b20.h"

sbit PIN_DQ = P3 ^ 6;

void delay_2us() // @24MHz
{
    uchar i = 0;

    i = 14;
    while (--i);
}

void delay_10us() // @24MHz
{
    uchar i = 0;

    i = 78;
    while (--i);
}

void delay_60us() // @24MHz
{
    uchar i = 0, j = 0;

    _nop_();
    i = 2;
    j = 220;
    do
    {
        while (--j);
    }
    while (--i);
}

void delay_80us() // @24MHz
{
    uchar i = 0, j = 0;

    _nop_();
    _nop_();
    i = 3;
    j = 123;
    do
    {
        while (--j);
    }
    while (--i);
}

void delay_500us() // @24MHz
{
    uchar i = 0, j = 0;

    i = 16;
    j = 147;
    do
    {
        while (--j);
    }
    while (--i);
}

uint8 ds18b20_reset()
{
    uint8 ret;
    PIN_DQ = 1;
    PIN_DQ = 0;
    delay_500us(); //��λ����

    PIN_DQ = 1;
    delay_80us();  //�ȴ�ds18b20��Ӧ
    ret = (PIN_DQ) ? 1 : 0;
    delay_500us();

    return ret;
}

void ds18b20_wrbit(bit b)
{
    PIN_DQ = 0;
    delay_10us();

    PIN_DQ = b;
    delay_60us();

    PIN_DQ = 1;
    delay_2us();
}

void ds_18b20_wrbyte(uint8 byte)
{
    uint8 i;
    for(i = 1; i != 0; i <<= 1)
    {
        if(i & byte)
            ds18b20_wrbit(1);
        else
            ds18b20_wrbit(0);
    }
}

bit ds18b20_rdbit()
{
    bit b;
    PIN_DQ = 0;
    delay_10us();

    PIN_DQ = 1;
    delay_2us();

    b = PIN_DQ;
    delay_60us();
    return b;
}

uint8 ds18b20_rdbyte()
{
    uint8 i = 0, tmp = 0;

    for(i = 1; i != 0; i <<= 1)
    {
        if(ds18b20_rdbit())
            tmp |= i;
    }

    return tmp;
}

#if 0
uint8 ds18b20_convert(uint16 *temp)
{
    static uint8 fg = 0; //�¶�״̬
    if(fg == 0)
    {
        if(ds18b20_reset())
            return RTN_ERR;
        ds_18b20_wrbyte(0xCC); //����ROMƥ��
        ds_18b20_wrbyte(0x44); //�����¶�ת��
        fg = 1;
    }
    else if(PIN_DQ) //����Ϊ��˵���¶�ת������
    {
        ds18b20_reset();
        ds_18b20_wrbyte(0xCC); //����ROMƥ��
        ds_18b20_wrbyte(0xBE); //���ݴ��������¶�

        *temp = ds18b20_rdbyte();  //��ȡ�����ֽڵ��¶�������
        *temp |= (ds18b20_rdbyte() << 8);
        fg = 0;
    }

    return RTN_OK;
}
#else
uint8 ds18b20_convert(uint8 *tmph, uint8 *tmpl)
{
    static uint8 fg = 0; //�¶�״̬
    if(fg == 0)
    {
        if(ds18b20_reset())
            return RTN_ERR;
        ds_18b20_wrbyte(0xCC); //����ROMƥ��
        ds_18b20_wrbyte(0x44); //�����¶�ת��
        fg = 1;
    }
    else if(PIN_DQ) //����Ϊ��˵���¶�ת������
    {
        ds18b20_reset();
        ds_18b20_wrbyte(0xCC); //����ROMƥ��
        ds_18b20_wrbyte(0xBE); //���ݴ��������¶�

        *tmpl = ds18b20_rdbyte(); //��ȡ�����ֽڵ��¶�������
        *tmph = ds18b20_rdbyte();
        fg = 0;
    }

    return RTN_OK;
}

uint8 ds18b20_get_temp(uint8 *sign, uint16 *interger, uint16 *decimal)
{
    uint16 xdata temp = 0;
    uint8 xdata templ = 0;
    uint8 xdata temph = 0;

    if(ds18b20_convert(&temph, &templ) != RTN_OK)
    {
        return RTN_ERR;
    }

    if((temph == 0) && (templ == 0))
    {
        return RTN_ERR;
    }

    temp = ((temph << 8) | templ);

    if(temph & 0x80)    //�����λΪ1˵���Ǹ��¶�
    {
        *sign = 1;              //���Ų���
        temp = ~temp;
        temp++;
    }
    else
    {
        *sign = 0;
    }

    *interger = ((temp >> 4) & 0xFFu);          //��������,��λ
    *decimal = (((temp & 0x0Fu) * 625) / 1000); //С������,һλ

    return RTN_OK;
}
#endif
