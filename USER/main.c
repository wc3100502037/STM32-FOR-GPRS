#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "string.h"
#include "key.h"
#include "sim900.h"
#include "GPRS.h"
#include "motor.h"    
#include "PID.h"
#include "motor.h"
#include "SpeedSense.h"

u8 judgeReadGPRSData(u8*);        //function declarm of check GPRS data
u16 getActualSetSpeed(u8* );

/***********************************************************************************************************************/	
/*                                         varial definetion                                                           */
	extern u8 MAIN_FLAG_READSM_STATE;
	extern u8 FLAG_GSM_DATA_COMING;   //GPRSµ½À´±êÖ¾
	extern u8 FLAG_SMS_CMD;         //¶ÌÐÅµ½À´±êÖ¾
	u8 SMSLocationMode;             //¶ÌÐÅËùÔÚÄÚ´æÎ»ÖÃ£¬0 for less than 10,1 for more than 10,2 for nothing
	u16 actualMotorSpeedMax=3500;   //Êµ¼Êµç»ú×î´ó×ªËÙÉèÖÃ
	u16 FREQUERY_SIHGLE;           //²âÁ¿µÃµ½µÄspeed ÆµÂÊ
	u8  FLAG_SENSESPEED_CMD=1;      //put into Timer function to let sensenspeed function execute at every 1 minute
/***********************************************************************************************************************/	



/******************************************Defination Of IO Port**********************************************************************/	
//#define MotorSpeedOut GPIO_Pin;
//#define MotorSpeedSenseIn ;
/********************************************************End******************************************************************/	



/*****************************************************************************************************************************/	
int main(void)
 {
		u8 SMSFLAG=1,CheckFLAG=0;	
		u8 FLAG_SIM900_Init=0,FLAG_GPRS_EstablishTCP=0;     //some flag value of GPRS
		u8 GPRS_GSM_RECDATA[20]={0};                      //GPRS received data are stored into this array
		u8 FLAG_JUDGE_GSM_DATA=255;                       //judge received GSM data to ensure the right format of datagram
		u8 FLAG_MAIN_GSM_COMING_ENABLE=0;                 //Ö÷º¯ÊýÅÐ¶ÏÊÇ·ñÓÐÊý¾Ýdaolai
		
		SMSLocationMode=2;   	                         //SMS default setted to 2,means nothing
		
		FLAG_GSM_DATA_COMING=0;                        //flag of gprs data coming,frist definited at usart.c
		
		KEY_Init();
		delay_init();	    	                           //ÑÓÊ±º¯Êý³õÊ¼»¯	
		NVIC_Configuration();                          // ÉèÖÃÖÐ¶ÏÓÅÏÈ¼¶·Ö×é
		uart_init(9600);	                             //´®¿Ú³õÊ¼»¯Îª9600
		FLAG_SIM900_Init=SIM900_Init();                //STEP 1 to initiate GPRS function
		LED_Init();		  	                             //³õÊ¼»¯ÓëLEDÁ¬½ÓµÄÓ²¼þ½Ó¿Ú 
	//	TIM1_PWM_Init(899,0);                          //µç»úÊä³ö£¬Õ¼¿ÕÖÜÆÚ900
	//	TIM2_Cap_Init(0xffff,72-1);                    //ËÙ¶ÈÆµÂÊÊäÈë
		/*Both LED is off in high */
		LED0=1;//RED 
		LED1=1;//GRENN

/**************************************************STEP 1***************************************************************/		
/*                                      Judge if GPRS_Init is successful                                               */
/*                                      test code by return code and led                                               */
	switch(FLAG_SIM900_Init)
	{
			case 1:                   //return code is 1 means succeed,refer to GPRS.c
				LED0_RUN(4);
				delay_ms(1000);
				LED1_RUN(4);
				break;
			case 2:
				LED0_RUN(3);
				delay_ms(1000);
				LED1_RUN(3);
				break;
			case 3:	
			case 4:
			case 5:
			case 6:
				LED0_RUN(2);
				delay_ms(1000);
				LED1_RUN(2);
				break;
			default:break;
	}
/***************************************************************************************************************************/	
	
	
/***********************************************************************************************************************/	
/*                                         STEP 2                                                                      */
/*                                         Establish TCP Link                                                          */
/***********************************************************************************************************************/	
//judge if eatablish TCP Link successful
  FLAG_GPRS_EstablishTCP=establishTCPLink();
  switch(FLAG_GPRS_EstablishTCP)
	{
			case 0://ÎÞ·µ»ØOK
				LED0_RUN(1);
				delay_ms(1000);
				LED1_RUN(1);
				break;
			case 1://ÎÞ·µ»ØCONNECT OK
				LED0_RUN(2);
				delay_ms(1000);
				LED1_RUN(2);
				break;
			case 2://½¨Á¢³É¹¦
				LED0_RUN(3);
				delay_ms(1000);
				LED1_RUN(3);
				break;
			default:
				break;
	}
/********************************************************************************************************************/	

	
/**************************Strating Working Loop*********************************************************************/			
 while(1)
	{ 
    /***********************************************************************************************/		
    /*                                  ¶ÌÐÅ´¦Àí                                                   */
	  MAIN_FLAG_READSM_STATE=SIM900_READ_SM(SMSLocationMode);	
		switch(MAIN_FLAG_READSM_STATE)
		{
		     case 0:LED0_RUN(5);break;//flash 5 timers,not receive "+CMGR" 
				 case 1:LED0_RUN(6);break;//received SM,but has not the right format;flash 6 timers ;RED FLASH
				 case 2:LED0_RUN(7);break;//rece      BNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN                                                                                                               BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBived right format SM,flash 8 timers.RED FLASH
			   default: break;		
		}
		MAIN_FLAG_READSM_STATE=255;// reset to default value	
  /*                                 end of SMS handle                                             */
  /*************************************************************************************************/		

		
		
/***************************************************************************************************************/		
/*                                 starting dealing GSM                                                        */
/*Judge whether GPRS data coming,and get the right data by judge flag of FLAG_JUDGE_GSM_DATA                   */
			
		FLAG_MAIN_GSM_COMING_ENABLE=GPRS_GSM_READDATA(GPRS_GSM_RECDATA,FLAG_GSM_DATA_COMING);
		if((FLAG_MAIN_GSM_COMING_ENABLE==1)||(FLAG_MAIN_GSM_COMING_ENABLE==2))//gsm data coming
		{
				LED0_RUN(1);
				LED1_RUN(3);
				FLAG_JUDGE_GSM_DATA=judgeReadGPRSData(GPRS_GSM_RECDATA);//return 1 means getting the right format of gsm data
				/*execute your code */
				if(FLAG_JUDGE_GSM_DATA==1)//format right
				{
					 motor_out(300);		
				}
				
				switch(FLAG_JUDGE_GSM_DATA)
				{
					case 0:LED0_RUN(4);
						 break;
					case 1:LED0_RUN(8);
						 break;
					default:break;
				
				}//switch end
				FLAG_JUDGE_GSM_DATA=255;			  //mask reset to error value
		}
		    FLAG_MAIN_GSM_COMING_ENABLE=0;           //
/*                                  End of GPRS                                                                   */		
/******************************************************************************************************************/		


/******************************************************************************************************************/		
/************************test code to judge the right connection of Hardware***************************************/							
 if(KEY_Scan(0)==WKUP_PRES)//°´ÏÂWK_UP½¡£¬ÅÐ¶ÏÊÇ·ñÕý³££¬Õý³£ÔòLED1ÁÁ
		{
		 CheckFLAG=SIM900_Check();
			if(CheckFLAG==1)
			{
				LED1_RUN(2);//GRENN flash 2 timers
			  CheckFLAG=0;
			}else LED1=1;
		}
	}  
}    
/*******************************************End of Main****************************************************************/		
/**********************************************************************************************************************/		


/**************************************************************************************************************************/
/*check and acquire the content of GPRS            */
/*and if it is right with the preset mask,return 1 */
/*the mask is 0x22 0x$$ 0x$$ 0x44,$ for value      */
/**************************************************************************************************************************/
u8 judgeReadGPRSData(u8* rcvData)
{ 
		int flag=0;
		/*    check the content of GPRS,and judge the checkcode as follow                   */
		if((rcvData[0]==0x31)&&(rcvData[1]==0x31)&&(rcvData[4]==0x32)&&(rcvData[8]==0x33)&&(rcvData[12]==0x34))//check the content of gprs
			{
			 //LED0_RUN(8);//ÉÁ8Ï
			 return 1;//right preset code,and reurn 1
			}
			return 255;//wrong code,return 255
	}
/***************************************************************************************************************************/



/***************************************************************************************************************************/
/*                          handle the gprs data to the right speed                                                        */
u16 getActualSetSpeed(u8* rcvData)
{
		int actualSpeedValue=0;
		actualSpeedValue=rcvData[1]*256+rcvData[2];
		return actualSpeedValue;

}
/***************************************************************************************************************************/
/*                         end of handle the gprs data to the right speed                                                  */
/***************************************************************************************************************************/

/***************************************************************************************************************************/
/********************************************Get the Actual Speed by Frequency**************************************************************************/
u16 getActualSenseSpeed()
{
    u16 SenseFre=FREQUERY_SIHGLE;
	  return FREQUERY_SIHGLE;
}


/***********************************************************************************************************************************/
/*                                     Following code are test code to test SM and GPRS function                                   */



/************************************************************************************************************************************/




