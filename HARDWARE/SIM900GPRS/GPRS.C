/**********************************************************************************************************/
/************************This program segment mainly focus on the GSM data and its handle***************************/
/**********************************************************************************************************/

#include "GPRS.h"
#include "usart.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "string.h"
#include "stdio.h"
#include "SIM900.h"

/**********************************************************************************************************/
/*                               some AT instructor                                                       */
char SIM900_GPRS_OK[]="OK"; 
char SIM900_CGCLASS[]="AT+CGCLASS=\"B\"";
char SIM900_CGATT[]="AT+CGATT=1";
char SIM900_CGDCONT[]="AT+CGDCONT=1,\"IP\",\"CMNET\"";
char SIM900_CIPCSGP[]="AT+CIPCSGP=1,\"CMNET\"";
char SIM900_CONNECTOK[]="CONNECT OK";
char SIM900_SEND[]="AT+CIPSEND";
char SIM900_CIPSTART[]="AT+CIPSTART=\"TCP\"";
char SIM900_CLPORT[]="AT+CLPORT=\"TCP\",\"2000\"";
char SIM900_CIPSEND[]="AT+CIPSEND";
char SIM900_CHECKCODE_CIPSEND_RETURN[]=">";
char SIM900_CHECKCODE_GSM_SEND_OK[]="SEND OK";
u8   SIM900_ENDOFSENDGSM=0x1A;
/**********************************************************************************************************/

/**********************************************************************************************************/
/*                                 STEP 1                                                                 */
/*                                 GPRS初始化                                                             */
u8 SIM900_GPRS_Init()
{ u8 count=0;
	Clear_USART_RX_BUF();	
	printf("%s\r\n\r\n",SIM900_CGCLASS);//发送AT+CGCLASS=\"B\"
	delay_ms(1500);
/*                    一秒未返回OK，return                                                                                   */ 
	if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK)==NULL)
     {
       Clear_USART_RX_BUF();	
       return 2;               //return 2 indicates the stage of program procedure stops at here
     }
	count=0;
/*set AT instructor step by step and return relative result code indicating the condition        */
/*         发送	AT+CGDCONT=1,\"IP\",\"CMNET\"                                                    */
	printf("%s\r\n\r\n",SIM900_CGDCONT);
	delay_ms(1500);                   //delay 1.5s to ensure return code successfully into the buffer
  if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK)==NULL)
      {
        Clear_USART_RX_BUF();	
        return 3;                       //return 3 if failed at this stage
      }
	count=0;
  printf("%s\r\n",SIM900_CGATT);    //发送AT+CGATT=1 
	delay_ms(1500);
  if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK)==NULL)
      {
        Clear_USART_RX_BUF();	
        return 4;                        //return 4 if stop at this stage
      }	
	Clear_USART_RX_BUF();
	count=0;
	printf("%s\r\n",SIM900_CIPCSGP);       //容易返回error
	delay_ms(1500);
  if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK)==NULL)
      {
        Clear_USART_RX_BUF();	
        return 5;
      }
	Clear_USART_RX_BUF();
	count=0;
	printf("%s\r\n\r\n",SIM900_CLPORT);
	delay_ms(1500);
  if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_GPRS_OK)==NULL)
      {
        Clear_USART_RX_BUF();	
        return 6;
      }	  
   return 1;                       //All the setting correct,return 1
}
/********************************************End of STEP1**********************************************************/


/**********************************************************************************************************/
/*                                        send final symbol 1A                                            */
void GPRS_SendHex1601A()
{
  printf("%x",26);

}
/*******************************************End**************************************************************/

/************************************************************************************************************/
/*                                  STEP 2                                                                  */
/*                                  establish TCP Link                                                      */
u8 FLAG_WHEN_Execute_EstabTCPLink=0;//used in serial judge segment,if excuting the establishTCP than
                                    //FLAG_WHEN_Execute_EstabTCPLink=1,so judge segment execute into other if-else sentence to identify 
                                    //CONNECT OK and OK,it must be reset to FALG_TCP=0 every time regardless of whether
                                    //conenction is ok.
u8 establishTCPLink()
{ 
	FLAG_WHEN_Execute_EstabTCPLink=1;
	Clear_USART_RX_BUF();
  printf("AT+CIPSTART=\"TCP\",\"114.229.251.145\",\"8080\"\r\n");//Please set IP and PORT
	delay_ms(5000);//如果已连接，返回ALREAD CONNECT，断电重连（GPRS模块）
	if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_CONNECTOK)==NULL)
    {//no CONNECT OK return
       Clear_USART_RX_BUF();	
       FLAG_WHEN_Execute_EstabTCPLink=0;
       return 0;
    }
	FLAG_WHEN_Execute_EstabTCPLink=0;
	return 2;//成功
}


/**********************************************************************************************************/
/*                                   读取GPRS传输回来的数据                                               */
/*                              This function read gsm data                                               */
/*                    #prameter :1 for receiveed date array,2 for flag of whether gsm data coming         */
extern u8 FLAG_GSM_DATA_COMING;
u8 GPRS_GSM_READDATA(u8* rcvData,u8 flagRUN)
{ 
	int i;
	if(flagRUN==0) //no gsm data coming
	{ FLAG_GSM_DATA_COMING=0;
		//LED1_RUN(1);
		return 0;//无数据
	}else
	{
	  LED0_RUN(1);	
	  FLAG_GSM_DATA_COMING=0;
    for(i=0;i<20;i++)
	   {
	     rcvData[i]=USART_RX_BUF[i]; 
	   }
	  Clear_USART_RX_BUF();
    return 1;
	} 
}  
/****************************************END******************************************************************/


/**************************************************************************************************************/
/***************************************Send Data By GSM*******************************************************/
/********************TestCode1,2 ared used to test,deleted if completed******************* ********************/

u8 GPRS_Send_GSM(u16* GSMData)
{  u16 TestCode1=123,TestCode2=321;//TestCode to test the connection of sending gsm
   Clear_USART_RX_BUF();
	 printf("AT+CIPSEND");//notice that new line not needed
/**************************waiting for "<" to identify readness to send********************************/
	 delay_ms(1000);
   if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_CHECKCODE_CIPSEND_RETURN)==NULL)
      {
        Clear_USART_RX_BUF();	
        return 1;//result code to help identify error stage
      }
	 /*to ensure the form of intervene of data and form of data(u8 or u16)*/
	 printf("%d",TestCode1);//first data to send
	 printf("%d",TestCode2);//second data to send
	 printf("%x\r\n",SIM900_ENDOFSENDGSM);//send 0x1A to end 
/**************************waiting for "SEND OK" to identify successfully send********************************/
	 delay_ms(3000);
   if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_CHECKCODE_GSM_SEND_OK)==NULL)
      {
        Clear_USART_RX_BUF();	
        return 2;//result code to help identify error stage
      }		
   return 0;//if all stage succeed,return 0
}
/******************************************END*****************************************************************/
/**************************************************************************************************************/




