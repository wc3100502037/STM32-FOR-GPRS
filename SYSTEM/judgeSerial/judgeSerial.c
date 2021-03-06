#include "judgeSerial.h"
#include "usart.h"
#include "led.c"
#include "string.h"

u8 FLAG_RX=0;
u8 FLAG_OK=0;
u8 FLAG_CONNECTOK=0;
u8 SMSREADTEST=6;
extern u8 mode;
  char SIM900_SMSREAD[]="CMTI";
	char SIM900_OK[]="OK"; 
	char SIM900_CONNECTOK[]="CONNECT";
u8 judgeSerial(u8 length)
{int i;
i=length;

			if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[0]!=0x2b)&&(USART_RX_BUF[1]!=0x43)&&(USART_RX_BUF[2]!=0x4d)&&(USART_RX_BUF[3]!=0x47)&&(USART_RX_BUF[4]!=0x52))//换行，但首句不是CMGR              
      {i=0;
			}
      else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[i-3]==0x4b)&&(USART_RX_BUF[i-4]==0x4f)&&(USART_RX_BUF[0]==0x2b)&&(USART_RX_BUF[1]==0x43)&&(USART_RX_BUF[2]==0x4d)&&(USART_RX_BUF[3]==0x47)&&(USART_RX_BUF[4]==0x52))//结尾以OK\r\n结束�  以+CMGR开始//////////////////////////////////////////////                                                                                                                   �
			{i=0;		  
			}	else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[1]==0x43)&&(USART_RX_BUF[2]==0x4d)&&(USART_RX_BUF[3]==0x54)&&(USART_RX_BUF[4]==0x49))//接收短信CMTI,结尾以换行结shu  ///////////////////////////////////////////////////////////////////////////////////////////////////
			{i=0;
			FLAG_RX=1;		
			}else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[i-4]==0x4f)&&(USART_RX_BUF[i-3]==0x4b))//接收到OK\r\n
			{
				i=0;
				FLAG_OK=1;	
			}else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[i-12]==0x43)&&(USART_RX_BUF[i-11]==0x4f)&&(USART_RX_BUF[i-10]==0x4e))//judge if it is end with CONNECT OK\r\n                  
			{
		      i=0;
				  FLAG_CONNECTOK=1;
			}
			
			
			//strstr(str1,str2)用于判断str2是否是str1的子串并返回首次出现的位置，未找到返回false
			/*SIM900_SMSREAD[]="CMTI"*/
    if((strstr((const char*)USART_RX_BUF,(const char*)SIM900_SMSREAD)!=NULL)&&(FLAG_RX==1))
		  { LED0_RUN(2);//串口工作则红灯闪烁2下
				if((USART_RX_BUF[13]==0x0d)&&(USART_RX_BUF[14]==0x0a)) mode=0;//个位短信条数在十条以内
				else if((USART_RX_BUF[14]==0x0d)&&(USART_RX_BUF[15]==0x0a)) mode=1;//十位，短信条数在十条以上。
		    else mode=2;
				FLAG_RX=0;
				i=0;			
      }else if((strstr((const char*)USART_RX_BUF,(const char*)SIM900_OK)!=NULL)&&(FLAG_OK==1))
			{ //如果接收OK\r\n
				LED0_RUN(3);
				i=0;
				FLAG_OK=0;
			//	LED_FLASH(4);//red flash
			}else if((strstr((const char*)USART_RX_BUF,(const char*)SIM900_CONNECTOK)!=NULL)&&(FLAG_CONNECTOK==1))
			{//receive CONNECT OK
			  i=0;
				FLAG_CONNECTOK=0;
				//LED0_RUN(4);
			}
     
			return 0;


}