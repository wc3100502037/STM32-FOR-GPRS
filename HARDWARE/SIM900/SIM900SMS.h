#ifndef _SIM900SMS_H
#define _SIM900SMS_H
#include "sys.h" 
u8 SIM900_Check(void);
void Clear_USART_RX_BUF(void);
u8 SIM900_SendSMS(void);
void Clear_RX_BUF(void);
u8 SIM900_Read_Text(void);
u8 SIM900_READ_TEXT_TEST(u8);
#endif

