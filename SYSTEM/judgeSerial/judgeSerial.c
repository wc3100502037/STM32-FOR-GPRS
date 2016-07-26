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

			if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[0]!=0x2b)&&(USART_RX_BUF[1]!=0x43)&&(USART_RX_BUF[2]!=0x4d)&&(USART_RX_BUF[3]!=0x47)&&(USART_RX_BUF[4]!=0x52))//ªª––£¨µ´ ◊æ‰≤ª «CMGR              
      {i=0;
			}
      else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[i-3]==0x4b)&&(USART_RX_BUF[i-4]==0x4f)&&(USART_RX_BUF[0]==0x2b)&&(USART_RX_BUF[1]==0x43)&&(USART_RX_BUF[2]==0x4d)&&(USART_RX_BUF[3]==0x47)&&(USART_RX_BUF[4]==0x52))//Ω·Œ≤“‘OK\r\nΩ· ¯¨  “‘+CMGRø™ º//////////////////////////////////////////////                                                                                                                   ¯
			{i=0;		  
			}	else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[1]==0x43)&&(USART_RX_BUF[2]==0x4d)&&(USART_RX_BUF[3]==0x54)&&(USART_RX_BUF[4]==0x49))//Ω” ’∂Ã–≈CMTI,Ω·Œ≤“‘ªª––Ω·shu  ///////////////////////////////////////////////////////////////////////////////////////////////////
			{i=0;
			FLAG_RX=1;		
			}else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[i-4]==0x4f)&&(USART_RX_BUF[i-3]==0x4b))//Ω” ’µΩOK\r\n
			{
				i=0;
				FLAG_OK=1;	
			}else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[i-12]==0x43)&&(USART_RX_BUF[i-11]==0x4f)&&(USART_RX_BUF[i-10]==0x4e))//judge if it is end with CONNECT OK\r\n                  
			{
		      i=0;
				  FLAG_CONNECTOK=1;
			}
			
			
			//strstr(str1,str2)”√”⁄≈–∂œstr2 «∑Ò «str1µƒ◊”¥Æ≤¢∑µªÿ ◊¥Œ≥ˆœ÷µƒŒª÷√£¨Œ¥’“µΩ∑µªÿfalse
			/*SIM900_SMSREAD[]="CMTI"*/
    if((strstr((const char*)USART_RX_BUF,(const char*)SIM900_SMSREAD)!=NULL)&&(FLAG_RX==1))
		  { LED0_RUN(2);//¥Æø⁄π§◊˜‘Ú∫Ïµ∆…¡À∏2œ¬
				if((USART_RX_BUF[13]==0x0d)&&(USART_RX_BUF[14]==0x0a)) mode=0;//∏ˆŒª∂Ã–≈Ãı ˝‘⁄ ÆÃı“‘ƒ⁄
				else if((USART_RX_BUF[14]==0x0d)&&(USART_RX_BUF[15]==0x0a)) mode=1;// ÆŒª£¨∂Ã–≈Ãı ˝‘⁄ ÆÃı“‘…œ°£
		    else mode=2;
				FLAG_RX=0;
				i=0;			
      }else if((strstr((const char*)USART_RX_BUF,(const char*)SIM900_OK)!=NULL)&&(FLAG_OK==1))
			{ //»Áπ˚Ω” ’OK\r\n
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