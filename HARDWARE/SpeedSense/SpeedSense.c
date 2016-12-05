#include "SpeedSense.h"


TIM_ICInitTypeDef  TIM2_ICInitStructure;

void TIM2_Cap_Init(u16 arr,u16 psc)
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能TIM2时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIOA时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;  //PA0 清除之前设置  
	GPIO_InitStructure.GPIO_mode = GPIO_mode_IPD; //PA0 输入  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);						 //PA0 下拉
	
	//初始化定时器2 TIM2	 
	TIM_TimeBaseStructure.TIM_Period = arr; //设定计数器自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 	//预分频器   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_Countermode = TIM_Countermode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
  
	//初始化TIM2输入捕获参数
	  TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	选择输入端 IC1映射到TI1上
  	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
  	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
  	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
  	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
  	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;     //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                     //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC1,ENABLE);//允许更新中断 ,允许CC1IE捕获中断	
	
  TIM_Cmd(TIM2,ENABLE ); 	//使能定时器2
 
}

/*                                       get the actual frequency                                                  */
u16 Counter_Now_Captured=0;//current counter of register when captured
u16 FLAG_Capture=0;        //to judge first or second time to capture high rising
u16 SUM_TIMER_COUNTER=0;   //
u16 FREQUERY_SUM=0;        //
extern u16 FREQUERY_SIHGLE;//current freq
//定时器2中断服务程序	 
void TIM2_IRQHandler(void)
{ 
		if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)//Timer2 Update interrupt not occur
	{	    
		   TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);
		    if(FLAG_Capture==0)//first time to capture high rising
		{
			  Counter_Now_Captured=TIM_GetCapture1(TIM2);
			  FLAG_Capture=1;		//Has captured first high rising
			
		} else if(FLAG_Capture==1)
			{
				if(TIM2,TIM_GetITStatus(TIM2,TIM_IT_Update)!=SET)//timer2 update interrupt not occur
				{
					 SUM_TIMER_COUNTER=TIM_GetCapture1(TIM2);//capture current value
				   SUM_TIMER_COUNTER=SUM_TIMER_COUNTER-Counter_Now_Captured;		
				} else
				{/*Timer update interrupt occur*/
				   TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
				   SUM_TIMER_COUNTER=0xffff+TIM_GetCapture1(TIM2)-Counter_Now_Captured+1;	
				}
			     FLAG_Capture=0;
			}
			FREQUERY_SIHGLE=1000000/SUM_TIMER_COUNTER;
    /*	intended to sum the frequency by 50	times
			if(SUM_TIMER_COUNTER!=0)//amount of timer counter between two high rising
		 {
		   FREQUERY_SIHGLE=1000000/SUM_TIMER_COUNTER;
		   flag++;
		   FREQUERY_SUM=FREQUERY_SUM+FREQUERY_SIHGLE;
		 }
		 if(flag==50)
		 {
			 flag=0;
	     TalFr=(int)FREQUERY_SUM/50;
		   FREQUERY_SUM=0;
		 }
     */
	}
   TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update); //清除中断标志位
   TIM_Cmd(TIM2,DISABLE);                                 //disable  interrupt unless set able
}

extern u8 FLAG_SENSESPEED_CMD;
void setSenseSpeedCmd()
{
  if(FLAG_SENSESPEED_CMD==1)
	 TIM_Cmd(TIM2,ENABLE);
}
