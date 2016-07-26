#include "GPRS.h"
#include "usart.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "string.h"
#include "stdio.h"
#include "SIM900.h"

char SIM900_GPRS_OK[]="OK"; 
char SIM900_CGCLASS[]="AT+CGCLASS=\"B\"";
char SIM900_CGATT[]="AT+CGATT=1";
char SIM900_CGDCONT[]="AT+CGDCONT=1,\"IP\",\"CMNET\"";
char SIM900_CIPCSGP[]="AT+CIPCSGP=1,\"CMNET\"";
char SIM900_CONNECTOK[]="CONNECT OK";
char SIM900_SEND[]="AT+CIPSEND";
char SIM900_CIPSTART[]="AT+CIPSTART=\"TCP\"";
char SIM900_CLPORT[]="AT+CLPORT=\"TCP\",\"2000\"";


//////////////////////////////////////////////////////////////////////////////////////////////////////////
/*清空缓存 区
void Clear_USART_RX_BUF()
{ u8 i=0;
 for(i=0;i<32;i++)
USART_RX_BUF[i]=0;

}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*GPRS初始化*/
u8 SIM900_GPRS_Init()
{ u8 count=0;
	Clear_USART_RX_BUF();
	
 //发送AT+CGCLASS=\"B\"
	printf("%s\r\n\r\n",SIM900_CGCLASS);
	delay_ms(1500);
	//一秒未返回OK，return 0
	if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK)==NULL)
{
Clear_USART_RX_BUF();	
return 2;
}
	/*
	while((strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK))==NULL)
	{ delay_ms(100);
		count++;
		if(count>30)
		{return 2;
		}
	}*/
	//Clear_USART_RX_BUF();//clear serial buff
	count=0;
//发送	AT+CGDCONT=1,\"IP\",\"CMNET\"
	printf("%s\r\n\r\n",SIM900_CGDCONT);
		delay_ms(1500);
if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK)==NULL)
{
Clear_USART_RX_BUF();	
return 3;
}
/*	
while((strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK))==NULL)
	{ delay_ms(100);
		count++;
		if(count>30)
		{return 3;
		}
	}
	*/
	//Clear_USART_RX_BUF();
	count=0;
	//发送AT+CGATT=1
	printf("%s\r\n",SIM900_CGATT);
		delay_ms(1500);
if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK)==NULL)
{
Clear_USART_RX_BUF();	
return 4;
}	
/*
 while((strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK))==NULL)
	{ delay_ms(100);
		count++;
		if(count>10)
		{return 4;
		}
	}*/
	Clear_USART_RX_BUF();
	count=0;
	printf("%s\r\n",SIM900_CIPCSGP);//容易返回error
		delay_ms(1500);
  if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK)==NULL)
{
Clear_USART_RX_BUF();	
return 5;
}
/*	
while((strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK))==NULL)
	{ delay_ms(100);
		count++;
		if(count>10)
		{return 5;
		}
	}*/
	Clear_USART_RX_BUF();
	count=0;
	printf("%s\r\n\r\n",SIM900_CLPORT);
		delay_ms(1500);
if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK)==NULL)
{
Clear_USART_RX_BUF();	
return 6;
}	
/*
 while((strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK))==NULL)
	{ delay_ms(100);
		count++;
		if(count>10)
		{return 6;
		}
	}
 */
  return 1;//All the setting correct,return 1
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*send final symbol 1A*/
void GPRS_SendHex1601A()
{
  printf("%x",26);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* establish TCP Link*/
u8 FLAG_TCP=0;
u8 establishTCPLink()
{ u8 count=0;
	FLAG_TCP=1;
	Clear_USART_RX_BUF();
  printf("AT+CIPSTART=\"TCP\",\"114.229.251.145\",\"8080\"\r\n");
	delay_ms(5000);//如果已连接，返回ALREAD CONNECT，断电重连（GPRS模块）
	if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_CONNECTOK)==NULL)
{//no CONNECT OK return
Clear_USART_RX_BUF();	
FLAG_TCP=0;
return 0;
}
	/*
	while((strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK))==NULL)
	{ delay_ms(100);
		count++;
		if(count>10)
		{return 0;//无返回ok
		}
	}*/
	//count=0;
	//Clear_USART_RX_BUF();
	//delay_ms(4000);
/*
if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_CONNECTOK)==NULL)
{
Clear_USART_RX_BUF();	
return 1;
}*/
/*
while((strstr((const char*)USART_RX_BUF,(const char*)SIM900_CONNECTOK))==NULL)
	{ delay_ms(100);
		count++;
		if(count>30)
		{return 1;//无返回connect ok
		}
	}
*/	
 //count=0;
	//Clear_USART_RX_BUF();
	FLAG_TCP=0;
	return 2;//成功
}

u8 GPRS_SendReady()
{
 
  return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
/* 读取GPRS传输回来的数据*/
extern u8 FLAG_GPRS_READCMD;
u8 GPRS_READ(u8* rcvData,u8 flagRUN)
{ int i;
	if(flagRUN==0) 
	{ FLAG_GPRS_READCMD=0;
		//LED1_RUN(1);
		return 0;//无数据
	}else
	{
	LED0_RUN(1);	
	FLAG_GPRS_READCMD=0;
  for(i=0;i<20;i++)
	{
	 rcvData[i]=USART_RX_BUF[i]; 
	}
	Clear_USART_RX_BUF();
   return 1;
	}
	 //return 2;
}  









