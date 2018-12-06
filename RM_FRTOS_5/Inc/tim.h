#ifndef __tim_H
#define __tim_H

#include "main.h"

#define LED1 TIM9->CCR1    //ÂÌµÆ
#define LED2 TIM9->CCR2    //ºìµÆ

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim9;

void MX_TIM1_Init(void);
void MX_TIM4_Init(void);
void MX_TIM5_Init(void);
void MX_TIM9_Init(void);
          
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);            

#endif
