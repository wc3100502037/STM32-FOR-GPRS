#ifndef SIM900GPRS_H
#define SIM900GPRS_H
#include "sys.h"
u8 SIM900_Init(void);
void GPRS_SendHex1601a(void);
u8 establishTCPLink(void);
u8 GPRS_SendRead(void);
u8 GPRS_GSM_READDATA(u8*,u8);
u8 GPRS_Send_GSM(u16* );
#endif

