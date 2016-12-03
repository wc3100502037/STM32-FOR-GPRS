#include "sys.h"
#include "usart.h"	  
#include "string.h"
#include "sim900.h"
#include "led.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//Èç¹ûÊ¹ÓÃucos,Ôò°üÀ¨ÏÂÃæµÄÍ·ÎÄ¼ş¼´¿É.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos Ê¹ÓÃ	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32¿ª·¢°å
//´®¿Ú1³õÊ¼»¯		   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//ĞŞ¸ÄÈÕÆÚ:2012/8/18
//°æ±¾£ºV1.5
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3ĞŞ¸ÄËµÃ÷ 
//Ö§³ÖÊÊÓ¦²»Í¬ÆµÂÊÏÂµÄ´®¿Ú²¨ÌØÂÊÉèÖÃ.
//¼ÓÈëÁË¶ÔprintfµÄÖ§³Ö
//Ôö¼ÓÁË´®¿Ú½ÓÊÕÃüÁî¹¦ÄÜ.
//ĞŞÕıÁËprintfµÚÒ»¸ö×Ö·û¶ªÊ§µÄbug
//V1.4ĞŞ¸ÄËµÃ÷
//1,ĞŞ¸Ä´®¿Ú³õÊ¼»¯IOµÄbug
//2,ĞŞ¸ÄÁËUSART_RX_STA,Ê¹µÃ´®¿Ú×î´ó½ÓÊÕ×Ö½ÚÊıÎª2µÄ14´Î·½
//3,Ôö¼ÓÁËUSART_REC_LEN,ÓÃÓÚ¶¨Òå´®¿Ú×î´óÔÊĞí½ÓÊÕµÄ×Ö½ÚÊı(²»´óÓÚ2µÄ14´Î·½)
//4,ĞŞ¸ÄÁËEN_USART1_RXµÄÊ¹ÄÜ·½Ê½
//V1.5ĞŞ¸ÄËµÃ÷
//1,Ôö¼ÓÁË¶ÔUCOSIIµÄÖ§³Ö
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//¼ÓÈëÒÔÏÂ´úÂë,Ö§³Öprintfº¯Êı,¶ø²»ĞèÒªÑ¡Ôñuse MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//±ê×¼¿âĞèÒªµÄÖ§³Öº¯Êı                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//¶¨Òå_sys_exit()ÒÔ±ÜÃâÊ¹ÓÃ°ëÖ÷»úÄ£Ê½    
_sys_exit(int x) 
{ 
	x = x; 
} 
//ÖØ¶¨Òåfputcº¯Êı 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*Ê¹ÓÃmicroLibµÄ·½·¨*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 

//´®¿Ú1ÖĞ¶Ï·şÎñ³ÌĞò
//×¢Òâ,¶ÁÈ¡USARTx->SRÄÜ±ÜÃâÄªÃûÆäÃîµÄ´íÎó   	
u8 USART_RX_BUF[USART_REC_LEN];     //½ÓÊÕ»º³å,×î´óUSART_REC_LEN¸ö×Ö½Ú.
//½ÓÊÕ×´Ì¬
//bit15£¬	½ÓÊÕÍê³É±êÖ¾
//bit14£¬	½ÓÊÕµ½0x0d
//bit13~0£¬	½ÓÊÕµ½µÄÓĞĞ§×Ö½ÚÊıÄ¿
u16 USART_RX_STA=0;       //½ÓÊÕ×´Ì¬±ê¼Ç	  

//³õÊ¼»¯IO ´®¿Ú1 
//bound:²¨ÌØÂÊ
void uart_init(u32 bound){
    //GPIO¶Ë¿ÚÉèÖÃ
    GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//Ê¹ÄÜUSART1£¬GPIOAÊ±ÖÓ
 	  USART_DeInit(USART1);  //¸´Î»´®¿Ú1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_mode = GPIO_mode_AF_PP;	//¸´ÓÃÍÆÍìÊä³ö
    GPIO_Init(GPIOA, &GPIO_InitStructure); //³õÊ¼»¯PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_mode = GPIO_mode_IN_FLOATING;//¸¡¿ÕÊäÈë
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //³õÊ¼»¯PA10

   //Usart1 NVIC ÅäÖÃ

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//ÇÀÕ¼ÓÅÏÈ¼¶3
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//×ÓÓÅÏÈ¼¶3
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	  NVIC_Init(&NVIC_InitStructure);	//¸ù¾İÖ¸¶¨µÄ²ÎÊı³õÊ¼»¯VIC¼Ä´æÆ÷
  
   //USART ³õÊ¼»¯ÉèÖÃ

	 USART_InitStructure.USART_BaudRate = bound;//Ò»°ãÉèÖÃÎª9600;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×Ö³¤Îª8Î»Êı¾İ¸ñÊ½
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//Ò»¸öÍ£Ö¹Î»
	 USART_InitStructure.USART_Parity = USART_Parity_No;//ÎŞÆæÅ¼Ğ£ÑéÎ»
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//ÎŞÓ²¼şÊı¾İÁ÷¿ØÖÆ
	 USART_InitStructure.USART_mode = USART_mode_Rx | USART_mode_Tx;	//ÊÕ·¢Ä£Ê½

    USART_Init(USART1, &USART_InitStructure); //³õÊ¼»¯´®¿Ú
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//¿ªÆôÖĞ¶Ï
    USART_Cmd(USART1, ENABLE);                    //Ê¹ÄÜ´®¿Ú 

}
/*****************************************************************************************************************/
/*                    This serial port segment only receive serial data                                          */
/*                    clear serial buffer function left to other relative segment in other files                 */
/*                    And GSM data definited by self-customized datagram ended with DA\r\n                       */
/*                    so it is convient to judge GSM data coming signal by this kind of end                      */
/*****************************************************************************************************************/

u8 Res;                  //serial port received data put into this variable
u8 i=0;                  //flag of data length to indicate received data
u8 FLAG_SM_Received=0;   //flag of small message coming,if set to 1 means A message come  
u8 FLAG_OK=0;            //ordinary OK flag indicates the right return OK
u8 FLAG_GSM_CONNECTED=0; //flag of GSM connection,if succeed to a server than set to 1. 
u8 SMSREADTEST=6;
extern u8 SMSLocationMode;       //SMS location flag,to judge location number of SMS,0 for less than 10 and 1 for more than 10
u8 FLAG_GSM_DATA_COMING;           //flag of GPRS data coming,changed by usart received data 
extern u8 FLAG_SMS_CMD;
/* 0x43->C,0x4d->M,0x4f->O   */
/* 0x54->T,0x49->I,0x47->G   */
/* 0x52->R ,0x4b->K          */
/* 0x2b->+,                  */
/* 0x44->D,0x41->A           */
extern u8 FLAG_WHEN_Execute_EstabTCPLink;	
#if EN_USART1_RX   //Èç¹ûÊ¹ÄÜÁË½ÓÊÕ
void USART1_IRQHandler(void)                	//´®¿Ú1ÖĞ¶Ï·şÎñ³ÌĞò
	{
	char SIM900_SMSREAD[]="CMTI";
	char SIM900_OK[]="OK"; 
	char SIM900_CONNECTOK[]="CONNECT";
	char SIM900_DATA[]="DA";	
	SMSLocationMode=2;	
#ifdef OS_TICKS_PER_SEC	 	//Èç¹ûÊ±ÖÓ½ÚÅÄÊı¶¨ÒåÁË,ËµÃ÷ÒªÊ¹ÓÃucosIIÁË.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE)!=RESET)//½ÓÊÕÖĞ¶Ï(½ÓÊÕµ½µÄÊı¾İ±ØĞëÊÇ0x0d 0x0a½áÎ²)
{
		     Res =USART_ReceiveData(USART1);//(USART1->DR);//¶ÁÈ¡½ÓÊÕµ½µÄÊı¾İ
         USART_RX_BUF[i]=Res;
         i++;//stored data number increased by 1
    //to judge serial content,and ensure end by "enter"
 	  if((i>1)&&(USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d))
   { 
		 if(FLAG_WHEN_Execute_EstabTCPLink==0)
	  { //This flag of FLAG_WHEN_Execute_EstabTCPLink to ensure returned OK is not CONNECT OK  
			//ÅĞ¶ÏÊÇ·ñÀ´µ½CONNECT oK »òÆÕÍ¨OK,ÒÔÏÂÖ´ĞĞÆÕÍ¨OK´úÂë
			 if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[0]!=0x2b)&&(USART_RX_BUF[1]!=0x43)&&(USART_RX_BUF[2]!=0x4d)&&(USART_RX_BUF[3]!=0x47)&&(USART_RX_BUF[4]!=0x52))//»»ĞĞ£¬µ«Ê×¾ä²»ÊÇCMGR              
      {//Õâ²ÅÊÇÅĞ¶ÏÊÇ·ñµ½À´£Ï£Ë		£¬ÕâÊÇÆÕÍ¨OK£¬·ÇCONNECT OK	  
				 FLAG_OK=1;//variable set 
				 i=0;
			}else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[i-3]==0x4b)&&(USART_RX_BUF[i-4]==0x4f)&&(USART_RX_BUF[0]==0x2b)&&(USART_RX_BUF[1]==0x43)&&(USART_RX_BUF[2]==0x4d)&&(USART_RX_BUF[3]==0x47)&&(USART_RX_BUF[4]==0x52))//½áÎ²ÒÔOK\r\n½áÊø¬  ÒÔ+CMGR¿ªÊ¼//////////////////////////////////////////////                                                                                                                   ø
			     {/*½áÎ²ÒÔOK\r\n½áÊø, ÒÔ+CMGR¿ª*/
						 i=0;		  
		       }else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[1]==0x43)&&(USART_RX_BUF[2]==0x4d)&&(USART_RX_BUF[3]==0x54)&&(USART_RX_BUF[4]==0x49))//½ÓÊÕ¶ÌĞÅCMTI,½áÎ²ÒÔ»»ĞĞ½áshu  ///////////////////////////////////////////////////////////////////////////////////////////////////
			             { //½ÓÊÕ¶ÌĞÅCMTI,½áÎ²ÒÔ»»ĞĞ½áshu  //
							   			i=0;
			                FLAG_SM_Received=1;	//There is message coming,flag is set.
                    // FLAG_SMS_CMD=1;				
			             }else if(((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[i-4]==0x4f)&&(USART_RX_BUF[i-3]==0x4b)))//½ÓÊÕµ½OK\r\n
			                  {//½ÓÊÕµ½OK\r\n
				                 
				                  FLAG_OK=1;	
				                  //  LED0_RUN(2);  
				                  FLAG_GSM_DATA_COMING=1;
			                    i=0;
			                  }else if((USART_RX_BUF[i-3]==0x41)&&(USART_RX_BUF[i-4]==0x44))//DA\r\n
			                        {/*self-customized datagram ended by DA\r\n that indicates GSM data coming*/
																// GPRSÓĞÊı¾İ´«¹ıÀ´
				                       //LED1_RUN(1);
				                         i=0;
				                         FLAG_GSM_DATA_COMING=1;
			                        }
	   }else if((USART_RX_BUF[i-1]==0x0a)&&(USART_RX_BUF[i-2]==0x0d)&&(USART_RX_BUF[i-12]==0x43)&&(USART_RX_BUF[i-11]==0x4f)&&(USART_RX_BUF[i-10]==0x4e))          
			   {/*THIS IF-ELSE SEGMENT EXECUTE WHEN ESTABLISH tcp LINK TO IDENTIFY ok AND coNNECT OK*/
					/*judge if it is end with CONNECT OK\r\n,to judge whether it is frist connected to server*/        
		        i=0;
				    FLAG_GSM_CONNECTED=1;
				    FLAG_WHEN_Execute_EstabTCPLink=0;//reset to 0 
			   }		
/*strstr(str1,str2)ÓÃÓÚÅĞ¶Ïstr2ÊÇ·ñÊÇstr1µÄ×Ó´®²¢·µ»ØÊ×´Î³öÏÖµÄÎ»ÖÃ£¬Î´ÕÒµ½·µ»Øfalse                           */
/*SIM900_SMSREAD[]="CMTI"                                                                                      */
    if((strstr((const char*)USART_RX_BUF,(const char*)SIM900_SMSREAD)!=NULL)&&(FLAG_SM_Received==1))
		  {/*this segment handle Small Message Service*/
				LED0_RUN(2);//´®¿Ú¹¤×÷ÔòºìµÆÉÁË¸2ÏÂ
				if((USART_RX_BUF[13]==0x0d)&&(USART_RX_BUF[14]==0x0a)) SMSLocationMode=0;//¸öÎ»¶ÌĞÅÌõÊıÔÚÊ®ÌõÒÔÄÚ
				else if((USART_RX_BUF[14]==0x0d)&&(USART_RX_BUF[15]==0x0a)) SMSLocationMode=1;//Ê®Î»£¬¶ÌĞÅÌõÊıÔÚÊ®ÌõÒÔÉÏ¡£
		    else SMSLocationMode=2;
				FLAG_SM_Received=0;
				i=0;			
      }else if((strstr((const char*)USART_RX_BUF,(const char*)SIM900_OK)!=NULL)&&(FLAG_OK==1))
			      { /*This segment handle ordinary OK*/
							//Èç¹û½ÓÊÕOK\r\n				
				      //LED0_RUN(2);
				      i=0;
				      FLAG_OK=0;
			      }else if((strstr((const char*)USART_RX_BUF,(const char*)SIM900_CONNECTOK)!=NULL)&&(FLAG_GSM_CONNECTED==1))
			            {/*This segment handle GSM connection succeed*/
										//receive CONNECT OK
			               i=0;
				             FLAG_GSM_CONNECTED=0;
				             //LED0_RUN(4);
			            }
		
}else if(i>=50)
      {/*receive more than 50 byte and not include OK\r\n*/
	      i=0;
      }
#ifdef OS_TICKS_PER_SEC	 	//Èç¹ûÊ±ÖÓ½ÚÅÄÊı¶¨ÒåÁË,ËµÃ÷ÒªÊ¹ÓÃucosIIÁË.
	OSIntExit();  											 
#endif
} 
#endif	
}
