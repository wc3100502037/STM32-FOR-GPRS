#include "motor.h"
#include "led.h"
#include "usart.h"


void TIM1_PWM_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  
	                                                                     	

   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_mode = GPIO_mode_AF_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_Countermode = TIM_Countermode_Up;  
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 

 
	TIM_OCInitStructure.TIM_OCmode = TIM_OCmode_PWM2; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  

  TIM_CtrlPWMOutputs(TIM1,ENABLE);	

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);   
	
	TIM_ARRPreloadConfig(TIM1, ENABLE); 
	
	TIM_Cmd(TIM1, ENABLE);  
 
   
}
/************************************************************************************************************************/
/*********************************output the actual speed value**********************************************************/
extern u16 actualMotorSpeedMax;
void motor_out(u16 setValue)
{
  u16 actualDutyValue;//实际占空比
	actualDutyValue=(int)((setValue/actualMotorSpeedMax)*900);
	TIM_SetCompare1(TIM1,actualDutyValue);
}
