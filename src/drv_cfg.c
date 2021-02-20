#include "drv_cfg.h"

//���Թ���Ƶ��Ϊ24MHz

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
#define IAP_EEPROM_PAGE_SIZE (512) //������С512�ֽ�
#define CONFIG_SAVE_FLAG_LEN (5)
#define CONFIG_CRC_FLAG_LEN (4)
#define CONFIG_FLAG_HEAD_ADD (IAP_EEPROM_START_ADDR)
#define CONFIG_SAVE_ADDRESS (CONFIG_FLAG_HEAD_ADD + CONFIG_SAVE_FLAG_LEN)
#define CONFIG_FLAG_TAIL_ADD(len) (CONFIG_SAVE_ADDRESS + (len))
uint8 code CONFIG_SAVE_FLAG[CONFIG_SAVE_FLAG_LEN + 1] = {"WNAVY"};

void eeprom_iap_idle() {
    IAP_CONTR = 0;    //�ر�IAP����
    IAP_CMD = 0;      //�������Ĵ���
    IAP_TRIG = 0;     //��������Ĵ���
    IAP_ADDRH = 0x80; //����ַ���õ���IAP����
    IAP_ADDRL = 0;
}

uint8 eeprom_iap_read(uint16 addr) {
    uint8 dat;

    IAP_CONTR = WAIT_TIME; //ʹ��IAP
    IAP_CMD = 1;           //����IAP������
    IAP_ADDRL = addr;      //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8; //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;       //д��������(0x5a)
    IAP_TRIG = 0xa5;       //д��������(0xa5)
    _nop_();
    dat = IAP_DATA;    //��IAP����
    eeprom_iap_idle(); //�ر�IAP����

    return dat;
}

void eeprom_iap_program(uint16 addr, uint8 dat) {
    IAP_CONTR = WAIT_TIME; //ʹ��IAP
    IAP_CMD = 2;           //����IAPд����
    IAP_ADDRL = addr;      //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8; //����IAP�ߵ�ַ
    IAP_DATA = dat;        //дIAP����
    IAP_TRIG = 0x5a;       //д��������(0x5a)
    IAP_TRIG = 0xa5;       //д��������(0xa5)
    _nop_();
    eeprom_iap_idle(); //�ر�IAP����
}

void eeprom_iap_erase(uint16 addr) {
    IAP_CONTR = WAIT_TIME; //ʹ��IAP
    IAP_CMD = 3;           //����IAP��������
    IAP_ADDRL = addr;      //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8; //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;       //д��������(0x5a)
    IAP_TRIG = 0xa5;       //д��������(0xa5)
    _nop_();               //
    eeprom_iap_idle();     //�ر�IAP����
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
 �� �� ��  : cfgsave_flagwrite
 ��������  : ��鱣���������ļ�ͷ������У���־
 �������  : cfglen     �����ļ��Ĵ�С(����)
 �������  : ��
 �� �� ֵ  : RTN_OK-�ɹ���RTN_ERR-ʧ��
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
 �� �� ��  : cfgsave_flagwrite
 ��������  : �������ļ�ͷ��������У���־
 �������  : cfglen     �����ļ��Ĵ�С(����)
 �������  : ��
 �� �� ֵ  : RTN_OK-�ɹ���RTN_ERR-ʧ��
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
 �� �� ��  : cfgsave_read
 ��������  : ���ڲ�eeprom�ж�ȡ����
 �������  : len    Ҫ�������������ĳ���(�ֽ�)
 �������  : config ָ�򱣴����õ�ָ��
 �� �� ֵ  : RTN_OK-�ɹ���RTN_ERR-ʧ��
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
 �� �� ��  : cfgsave_write
 ��������  : �����ñ��浽�ڲ�eeprom��
 �������  : config ָ�򱣴����õ�ָ��
             len    Ҫ�������������ĳ���(�ֽ�)
 �������  : ��
 �� �� ֵ  : RTN_OK-�ɹ���RTN_ERR-ʧ��
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
