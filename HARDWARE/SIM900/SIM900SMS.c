/*这个文档是本项目的短信接收验证程序*/
#include "SIM900SMS.H"
#include "usart.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "string.h"
#include "stdio.h"

 char SIM900_OK[]="OK"; 
 char SIM900_AT[]="AT";
 char SIM900_CSCS[]="AT+CSCS=\"GSM\"";
 char SIM900_SMSRETURN[]=">";
 char SIM900_SMSREAD[]="+CMTI";
 char SIM900_SMSREADCONT[]="CMGR";
 char SIM900_TEST[]="TEST";
 char SIM900_READTEXTCMD[]="AT+CMGR=";

 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                  发送AT指令，判断SIM900工作是否正常                                  */
u8 SIM900_Check(void)
{Clear_USART_RX_BUF();	
 printf("%s\r\n",SIM900_AT);
 delay_ms(1500);
 if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_OK))
  {
    Clear_USART_RX_BUF();	
    return 1;//return OK
  }
  return 0;	 //error
	}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

u8 SIM900_SendSMS()
{ u8 count=0;//flag varial to statistic waiting time by sum
 /* 发薃T+CSCS=\"GSM\"\r\n"，判断是否OK，3秒无则报错*/
	printf("AT+CSCS=\"GSM\"\r\n");
	/*                 wait until time out                                             */
	while((strstr((const char*)USART_RX_BUF,(const char*)SIM900_OK))==NULL)
	{ delay_ms(100);
		count++;
		if(count>30)
		{
			return 0;//no expected return
		}
	}
	Clear_USART_RX_BUF();
	count=0;
	/*发送AT+CMGF=1\r\n，判断是否OK，*/
	printf("AT+CMGF=1\r\n");
	while((strstr((const char*)USART_RX_BUF,(const char*)SIM900_OK))==NULL)
	{ delay_ms(100);
		count++;
		if(count>30)
		{ 
			return 1;
		}
	}
	
  Clear_USART_RX_BUF();
	count=0;
	
	printf("AT+CMGS=\"18020484363\"\r\n");// set destination phone number
	while((strstr((const char*)USART_RX_BUF,(const char*)SIM900_SMSRETURN)==NULL))
	{delay_ms(100);
	 count++;
		if(count>50)
		 { 
			 return 2; 
		 }
	}
printf("Congratulation!!");//send successfully
	delay_ms(3000);
USART_SendData(USART1,0X1A);
while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);	
	LED0=0;
	
return 3;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////??
/*                           试验代码,可不管                                                                                   */
u8 SIM900_Read_Text()
{ u8 HighBit,LowBit;
	
	u8 count=0;
	u8 SMContentArray[20]={0};
	u8 i=0;
	
 if(strstr((const char*)USART_RX_BUF,(const char*)SIM900_SMSREAD))//判断是否+CMTI，有短信到来
 {
   HighBit=USART_RX_BUF[12];//短信位置   十位
	 LowBit=USART_RX_BUF[13];//短信位置    个位
	 /*发送短信读取指令*/
	 printf("AT+CMGR=");
	 USART_SendData(USART1,HighBit);
	 USART_SendData(USART1,LowBit);
	 printf("\r\n");
   
	 while(strstr((const char*)USART_RX_BUF,(const char*)SIM900_SMSREADCONT)==NULL)
	 { delay_ms(100);
		 count++;
		 if(count>40)
		 {
		  count=0;
			 return 0;
		 }
	 }
   delay_ms(200);
	 
   /*读取短信实际内容，放入INFO64数组*/	
	 for(i=0;i<20;i++)
	 {
	  SMContentArray[i]=USART_RX_BUF[64+i];
	 
	 }
	 
	
 }else return 0;

return 1;
}
/*               above is the test code                                                                   */
/*****************************************************************************************************************************/


/************************************************************************************************************************************/
/*读取短信 实际内容  */
extern u8 SMSLocationMode;//判断接收短信的所在位置，10为分隔线
u8 SMContentArray[20]={0};//放入短信实际内容
u8 FLAG_SMS_CMD=0;//判断是否短信服务
u8 SIM900_READ_SM(u8 SMSLocationMode)
{ u8 count=0;
	
	u8 i=0;
	//u8 flag=9;//判断短信位置
	u8 FLAG_SM_CHECKFORMAT=0;//check the SM format to ensure correction of content
	u8 HighBit;//位置高位
	u8 LowBit;//位置低位
	u8 Hex[10]={0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9};//mask,to extract the factual data
	
	if(SMSLocationMode==1)
		{//SM length more than 10
      HighBit=USART_RX_BUF[12];
	    LowBit=USART_RX_BUF[13];
	    HighBit=Hex[HighBit-0x30];
	    LowBit=Hex[LowBit-0x30];
	    SMSLocationMode=2;                      //reset SMS location to error bit
	    Clear_USART_RX_BUF();
	    printf("%s%x%x\r\n\r\n",SIM900_READTEXTCMD,HighBit,LowBit);//send reading SMS instructor
	}else if(SMSLocationMode==0)
	{//SMS location less than 10
	    HighBit=USART_RX_BUF[12];
	    HighBit=Hex[HighBit-0x30];	
      SMSLocationMode=2;	
	    Clear_USART_RX_BUF();
	    printf("%s%x\r\n\r\n",SIM900_READTEXTCMD,HighBit);
	}else 
  {
		  return 9;
	}

   while(strstr((const char*)USART_RX_BUF,(const char*)SIM900_SMSREADCONT)==NULL)// 读取短信实际内容,
{ //JUDGE THE CONTENT BY CMGR,NOT IN THE SERIAL PORT SEGMENT                     // 10秒无返回CMGR则return 0
     delay_ms(100);
		 count++;
		 if(count>100)
		 {
		   count=0;
			 return 0;//stop at this stage,return 0 means led flash 5 times
		 }
}
   delay_ms(200);

 /*                 This stage means having got through check stage,Read the SM actual content              */
	 for(i=0;i<20;i++)
	 {
	   //SMContentArray[i]=USART_RX_BUF[64+i];
	   SMContentArray[i]=USART_RX_BUF[4+i];
		 if(i==19) 
		 FLAG_SM_CHECKFORMAT=1;//6下
	 }

 /*                 check the preset format of SM,  监测是否11** 2*** 3*** 4****                              */	 
  if((SMContentArray[4]==0x32)&&(SMContentArray[8]==0x33)&&(SMContentArray[12]==0x34))//检测短信内容
{//(SMContentArray[0]==0x31)&&(SMContentArray[1]==0x31)&&
   //LED0_RUN(8);//闪8下
   FLAG_SM_CHECKFORMAT=2;//7下
}
  return FLAG_SM_CHECKFORMAT;
}


/*********************************************************************************************************************************/
/*                                                     clear the data buffer                                                    */
void Clear_USART_RX_BUF()
{ u8 i=0;
  for(i=0;i<30;i++)
	USART_RX_BUF[i]=0;
}



