#include "drv_cfg.h"

//测试工作频率为24MHz

#if 0
sfr     IAP_DATA    =   0xC2;
sfr     IAP_ADDRH   =   0xC3;
sfr     IAP_ADDRL   =   0xC4;
sfr     IAP_CMD     =   0xC5;
sfr     IAP_TRIG    =   0xC6;
sfr     IAP_CONTR   =   0xC7;
#endif

#define WAIT_TIME_30M 0x80
#define WAIT_TIME_24M 0x81
#define WAIT_TIME_20M 0x82
#define WAIT_TIME_12M 0x83
#define WAIT_TIME_6M 0x84
#define WAIT_TIME_3M 0x85
#define WAIT_TIME_2M 0x86
#define WAIT_TIME_1M 0x87

#define WAIT_TIME WAIT_TIME_24M

// Start address for STC8F2K16S2 EEPROM
#define IAP_EEPROM_START_ADDR (0x0000)
#define IAP_EEPROM_END_ADDR (0xBFFF)
#define IAP_EEPROM_PAGE_SIZE (512) //扇区大小512字节
#define CONFIG_SAVE_FLAG_LEN (5)
#define CONFIG_CRC_FLAG_LEN (4)
#define CONFIG_FLAG_HEAD_ADD (IAP_EEPROM_START_ADDR)
#define CONFIG_SAVE_ADDRESS (CONFIG_FLAG_HEAD_ADD + CONFIG_SAVE_FLAG_LEN)
#define CONFIG_FLAG_TAIL_ADD(len) (CONFIG_SAVE_ADDRESS + (len))
uint8 code CONFIG_SAVE_FLAG[CONFIG_SAVE_FLAG_LEN + 1] = {"WNAVY"};

void eeprom_iap_idle() {
    IAP_CONTR = 0;    //关闭IAP功能
    IAP_CMD = 0;      //清除命令寄存器
    IAP_TRIG = 0;     //清除触发寄存器
    IAP_ADDRH = 0x80; //将地址设置到非IAP区域
    IAP_ADDRL = 0;
}

uint8 eeprom_iap_read(uint16 addr) {
    uint8 dat;

    IAP_CONTR = WAIT_TIME; //使能IAP
    IAP_CMD = 1;           //设置IAP读命令
    IAP_ADDRL = addr;      //设置IAP低地址
    IAP_ADDRH = addr >> 8; //设置IAP高地址
    IAP_TRIG = 0x5a;       //写触发命令(0x5a)
    IAP_TRIG = 0xa5;       //写触发命令(0xa5)
    _nop_();
    dat = IAP_DATA;    //读IAP数据
    eeprom_iap_idle(); //关闭IAP功能

    return dat;
}

void eeprom_iap_program(uint16 addr, uint8 dat) {
    IAP_CONTR = WAIT_TIME; //使能IAP
    IAP_CMD = 2;           //设置IAP写命令
    IAP_ADDRL = addr;      //设置IAP低地址
    IAP_ADDRH = addr >> 8; //设置IAP高地址
    IAP_DATA = dat;        //写IAP数据
    IAP_TRIG = 0x5a;       //写触发命令(0x5a)
    IAP_TRIG = 0xa5;       //写触发命令(0xa5)
    _nop_();
    eeprom_iap_idle(); //关闭IAP功能
}

void eeprom_iap_erase(uint16 addr) {
    IAP_CONTR = WAIT_TIME; //使能IAP
    IAP_CMD = 3;           //设置IAP擦除命令
    IAP_ADDRL = addr;      //设置IAP低地址
    IAP_ADDRH = addr >> 8; //设置IAP高地址
    IAP_TRIG = 0x5a;       //写触发命令(0x5a)
    IAP_TRIG = 0xa5;       //写触发命令(0xa5)
    _nop_();               //
    eeprom_iap_idle();     //关闭IAP功能
}

#if 0
void main()
{
    eeprom_iap_erase(0x0400);
    P0 = eeprom_iap_read(0x0400);                //P0=0xff
    eeprom_iap_program(0x0400, 0x12);
    P1 = eeprom_iap_read(0x0400);                //P1=0x12

    while (1);
}
#endif

/*****************************************************************************
 函 数 名  : cfgsave_flagwrite
 功能描述  : 检查保存在配置文件头部区的校验标志
 输入参数  : cfglen     配置文件的大小(长度)
 输出参数  : 无
 返 回 值  : RTN_OK-成功，RTN_ERR-失败
*****************************************************************************/
uchar cfgsave_flagchk(uchar cfglen) {
    uint i = 0;

    if (cfglen <= 0) {
        return RTN_ERR;
    }

    delay_xus(100);
    for (i = 0; i < CONFIG_SAVE_FLAG_LEN; i++) // check the config save flag
    {
        // check the head flag
        if (eeprom_iap_read(CONFIG_FLAG_HEAD_ADD + i) != (uchar)CONFIG_SAVE_FLAG[i]) {
            return RTN_ERR;
        }
    }

    return RTN_OK;
}

uchar cfgsave_crcchk(uchar *config, uchar cfglen) {
    uint i = 0;
    uint32 crc = 0;
    uint8 *pcrc = 0;

    if (cfglen <= 0) {
        return RTN_ERR;
    }

    delay_xus(100);
    crc = get_crc32(config, cfglen);
    pcrc = (uint8 *)(&crc);
    for (i = 0; i < CONFIG_CRC_FLAG_LEN; i++) // check the config crc flag
    {
        // check the crc flag
        if (eeprom_iap_read(CONFIG_FLAG_TAIL_ADD(cfglen) + i) != (uchar)pcrc[i]) {
            return RTN_ERR;
        }
    }

    return RTN_OK;
}

/*****************************************************************************
 函 数 名  : cfgsave_flagwrite
 功能描述  : 在配置文件头部区加入校验标志
 输入参数  : cfglen     配置文件的大小(长度)
 输出参数  : 无
 返 回 值  : RTN_OK-成功，RTN_ERR-失败
*****************************************************************************/
uchar cfgsave_flagwrite(uchar cfglen) {
    uint i = 0;
    uint32 crc = 0;
    uint8 *pcrc = 0;

    if (cfglen <= 0) {
        return RTN_ERR;
    }

    delay_xus(100);
    for (i = 0; i < CONFIG_SAVE_FLAG_LEN; i++) {
        // write the head flag
        eeprom_iap_program(CONFIG_FLAG_HEAD_ADD + i, (uchar)(CONFIG_SAVE_FLAG[i]));
    }

    return RTN_OK;
}

uchar cfgsave_crcwrite(uchar *config, uchar cfglen) {
    uint i = 0;
    uint32 crc = 0;
    uint8 *pcrc = 0;

    if (cfglen <= 0) {
        return RTN_ERR;
    }

    delay_xus(100);
    crc = get_crc32(config, cfglen);
    pcrc = (uint8 *)(&crc);
    for (i = 0; i < CONFIG_CRC_FLAG_LEN; i++) {
        // write the crc32 tail flag
        eeprom_iap_program(CONFIG_FLAG_TAIL_ADD(cfglen) + i, (uchar)(pcrc[i]));
    }

    return RTN_OK;
}

/*****************************************************************************
 函 数 名  : cfgsave_read
 功能描述  : 从内部eeprom中读取配置
 输入参数  : len    要读区的配置区的长度(字节)
 输出参数  : config 指向保存配置的指针
 返 回 值  : RTN_OK-成功，RTN_ERR-失败
*****************************************************************************/
uchar cfgsave_read(uchar *config, uint len) {
    uint i;

    if ((config == NULL) || (len <= 0)) {
        return RTN_ERR;
    }

    if (cfgsave_flagchk(len) != RTN_OK) // check the config save flag
    {
        return RTN_ERR;
    }

    delay_xus(100);

    for (i = 0; i < len; i++) {
        // config[i] = eeprom_iap_read(CONFIG_SAVE_ADDRESS + i);
        *(config + i) = eeprom_iap_read(CONFIG_SAVE_ADDRESS + i);
    }

    delay_xus(100);
    if (cfgsave_crcchk(config, len) != RTN_OK) // check the config crc flag
    {
        return RTN_ERR;
    }

    return RTN_OK;
}

/*****************************************************************************
 函 数 名  : cfgsave_write
 功能描述  : 把配置保存到内部eeprom中
 输入参数  : config 指向保存配置的指针
             len    要读区的配置区的长度(字节)
 输出参数  : 无
 返 回 值  : RTN_OK-成功，RTN_ERR-失败
*****************************************************************************/
uchar cfgsave_write(uchar *config, uint len) {
    uint i;

    if ((config == NULL) || (len <= 0)) {
        return RTN_ERR;
    }

    delay_xus(100);

    eeprom_iap_erase(CONFIG_FLAG_HEAD_ADD);
    cfgsave_flagwrite(len); // write flag at the head of the config

    delay_xus(100);

    for (i = 0; i < len; i++) {
        eeprom_iap_program(CONFIG_SAVE_ADDRESS + i, (uchar)(*(config + i)));
    }

    delay_xus(100);
    cfgsave_crcwrite(config, len); // write crc at the tail of the config

    return RTN_OK;
}
