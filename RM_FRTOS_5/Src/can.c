#include "main.h"
CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;
static CanTxMsgTypeDef     Tx1Message;      
static CanRxMsgTypeDef     Rx1Message;    
static CanTxMsgTypeDef     Tx2Message;      
static CanRxMsgTypeDef     Rx2Message; 

/* CAN1 init function */
void MX_CAN1_Init(void)
{
	CAN_FilterConfTypeDef  hcan1_filter;
	
  hcan1.Instance = CAN1;
	hcan1.pTxMsg= &Tx1Message;
	hcan1.pRxMsg= &Rx1Message;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SJW = CAN_SJW_1TQ;
  hcan1.Init.BS1 = CAN_BS1_9TQ;
  hcan1.Init.BS2 = CAN_BS2_4TQ;
  hcan1.Init.TTCM = DISABLE;
  hcan1.Init.ABOM = DISABLE;
  hcan1.Init.AWUM = DISABLE;
  hcan1.Init.NART = DISABLE;
  hcan1.Init.RFLM = DISABLE;
  hcan1.Init.TXFP = DISABLE;
	HAL_CAN_Init(&hcan1);
	
	hcan1_filter.FilterIdHigh=0X0000;     //32位ID
  hcan1_filter.FilterIdLow=0X0000;
  hcan1_filter.FilterMaskIdHigh=0X0000; //32位MASK
  hcan1_filter.FilterMaskIdLow=0X0000;  
  hcan1_filter.FilterFIFOAssignment=CAN_FILTER_FIFO0;//过滤器0关联到FIFO0
  hcan1_filter.FilterNumber=0;          //过滤器0
  hcan1_filter.FilterMode=CAN_FILTERMODE_IDMASK;
  hcan1_filter.FilterScale=CAN_FILTERSCALE_32BIT;
  hcan1_filter.FilterActivation=ENABLE; //激活滤波器0
  hcan1_filter.BankNumber=14;
	
  HAL_CAN_ConfigFilter(&hcan1,&hcan1_filter);
}
/* CAN2 init function */
void MX_CAN2_Init(void)
{
	CAN_FilterConfTypeDef  hcan2_filter;
	
	hcan2.Instance = CAN2;
	hcan2.pTxMsg= &Tx2Message;
	hcan2.pRxMsg= &Rx2Message;
  hcan2.Init.Prescaler = 3;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SJW = CAN_SJW_1TQ;
  hcan2.Init.BS1 = CAN_BS1_9TQ;
  hcan2.Init.BS2 = CAN_BS2_4TQ;
  hcan2.Init.TTCM = DISABLE;
  hcan2.Init.ABOM = DISABLE;
  hcan2.Init.AWUM = DISABLE;
  hcan2.Init.NART = DISABLE;
  hcan2.Init.RFLM = DISABLE;
  hcan2.Init.TXFP = DISABLE;

	HAL_CAN_Init(&hcan2);
	
	hcan2_filter.FilterIdHigh=0X0000;     
  hcan2_filter.FilterIdLow=0X0000;
  hcan2_filter.FilterMaskIdHigh=0X0000; 
  hcan2_filter.FilterMaskIdLow=0X0000;  
  hcan2_filter.FilterFIFOAssignment=CAN_FILTER_FIFO0;
  hcan2_filter.FilterNumber=14;         
  hcan2_filter.FilterMode=CAN_FILTERMODE_IDMASK;
  hcan2_filter.FilterScale=CAN_FILTERSCALE_32BIT;
  hcan2_filter.FilterActivation=ENABLE; 
  hcan2_filter.BankNumber=14;
  HAL_CAN_ConfigFilter(&hcan2,&hcan2_filter);
}
void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(canHandle->Instance==CAN1)
  {
    __HAL_RCC_CAN1_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();		
    /**CAN1 GPIO Configuration    
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
		HAL_NVIC_SetPriority(CAN1_TX_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn); 
  }
  else if(canHandle->Instance==CAN2)
  {
    __HAL_RCC_CAN2_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();	
    /**CAN2 GPIO Configuration    
    PB12     ------> CAN2_RX
    PB6     ------> CAN2_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		HAL_NVIC_SetPriority(CAN2_TX_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN2_TX_IRQn);
    HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn); 
  }
}

//CAN1接受中断
void CAN1_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan1);
}

//CAN2接收中断
void CAN2_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&hcan2);
}
