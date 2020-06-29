#ifndef __CHNIA_NONLI_H__
#define __CHNIA_NONLI_H__

#include "common.h"

#define NONLI_STRING_LEN 8

typedef struct china_nonli_s {
    uchar month;  //ũ����
    uchar intcal; //ũ������
    uchar day;    //ũ����
} china_nonli_t;

/*****************************************************************************
 �� �� ��  : china_nonli_get
 ��������  : ���������RTCʱ���ȡũ��ʱ��
 �������  : tm         ��ǰRTCʱ��
 �������  : nonli      ��ǰũ��ʱ��
 �� �� ֵ  : �ɹ�-RTN_OK, ʧ��-RTN_ERR
*****************************************************************************/
extern uchar china_nonli_get(rtc_time_t *tm, china_nonli_t *nonli);

#endif
