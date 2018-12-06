#include "main.h"

TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim9;
																																							/*APB1 42Mhz    time clocks 84Mhz*/
																																							/*APB2 84Mhz    time9 clocks 168Mhz*/ 
/* TIM4 init function */
void MX_TIM4_Init(void)
{
  TIM_OC_InitTypeDef sConfigOC;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 84-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 20000;			//20ms
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_PWM_Init(&htim4);
	
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
#if   CAR_NUM == 1
	sConfigOC.Pulse = 2350;
#elif CAR_NUM == 4
	sConfigOC.Pulse = 1766;
#else
	sConfigOC.Pulse = 2150;
#endif
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
 
	HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1);
	
  HAL_TIM_MspPostInit(&htim4);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1); 
}

/* TIM5 1ms interrupt init function */
void MX_TIM5_Init(void)
{
	htim5.Instance=TIM5;                          
  htim5.Init.Prescaler=84-1;          //84Mhz          
  htim5.Init.CounterMode=TIM_COUNTERMODE_UP;    
  htim5.Init.Period=1000;                        
  htim5.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim5);
    
  HAL_TIM_Base_Start_IT(&htim5); 
}

/* TIM9 init function */
void MX_TIM9_Init(void)
{
  TIM_OC_InitTypeDef sConfigOC;

  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 168-1;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 1000;			//1000ms
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_PWM_Init(&htim9);
	
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
 
	HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_2);
	
  HAL_TIM_MspPostInit(&htim9);
	HAL_TIM_PWM_Start(&htim9,TIM_CHANNEL_1);  //绿灯
	HAL_TIM_PWM_Start(&htim9,TIM_CHANNEL_2);  //红灯
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM5)
	{
		__HAL_RCC_TIM5_CLK_ENABLE();            
		HAL_NVIC_SetPriority(TIM5_IRQn,5,0);    
		HAL_NVIC_EnableIRQ(TIM5_IRQn);    
	}
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{
  if(tim_pwmHandle->Instance==TIM1)
  {
    __HAL_RCC_TIM1_CLK_ENABLE();
  }
  else if(tim_pwmHandle->Instance==TIM4)
  {
    __HAL_RCC_TIM4_CLK_ENABLE();
  }
  else if(tim_pwmHandle->Instance==TIM9)
  {
    __HAL_RCC_TIM9_CLK_ENABLE();
  }
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(timHandle->Instance==TIM1)
  {
    /**TIM1 GPIO Configuration    
    PE8     ------> TIM1_CH1N
    PE9     ------> TIM1_CH1
    PE10     ------> TIM1_CH2N
    PE11     ------> TIM1_CH2
    PE12     ------> TIM1_CH3N
    PE13     ------> TIM1_CH3
    PE14     ------> TIM1_CH4 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  }
  else if(timHandle->Instance==TIM4)
  {
    /**TIM4 GPIO Configuration    
    PD12     ------> TIM4_CH1
    PD13     ------> TIM4_CH2
    PD14     ------> TIM4_CH3
    PD15     ------> TIM4_CH4 
    */
		__HAL_RCC_GPIOD_CLK_ENABLE();	
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  }
  else if(timHandle->Instance==TIM9)
  {
    /**TIM9 GPIO Configuration    
    PE5     ------> TIM9_CH1   绿灯
    PE6     ------> TIM9_CH2 	 红灯
    */
		__HAL_RCC_GPIOE_CLK_ENABLE();	
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  }

}

void TIM5_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim5);
}

//回调函数，定时器中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==(&htim5))
	{

  }
}
