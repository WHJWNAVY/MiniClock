#ifndef  _DRV_DS18B20_H_
#define  _DRV_DS18B20_H_

#include "common.h"
//extern uint8 ds18b20_convert(uint16* temp);
extern uint8 ds18b20_get_temp(uint8* sign, uint16* interger, uint16* decimal);
#endif


