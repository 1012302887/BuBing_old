#ifndef __usart_H
#define __usart_H

#include "stm32f4xx_hal.h"

#define RECEIVELEN 128    

typedef struct
{
	int16_t raw_yaw_angle;
	int16_t raw_pit_angle;
	
	float filter_yaw;
	float	filter_pit;
	
  float dynamic_yaw;
	float dynamic_pit;
	
	uint16_t star_shoot;
	uint16_t last_star_shoot;
	uint16_t bug;
	uint16_t last_bug;
	
	uint16_t time;
	
	float last_dynamic_yaw;
	float last_dynamic_pit;
	
	float coordinate;
	float coordinate_data[5];
	float last_coordinate;
	float last_coordinate_data[5];
	
	float last_times;
	float now_times;
	
	float v_now;
	float v_last;
	float v_now_i;
	
	float	yaw_befoer[101];
	
	uint8_t shoot_data;
} pc_data_t;

/*remote info*/
typedef __packed struct
{
	int16_t ch0;
	int16_t ch1;
	int16_t ch2;
	int16_t ch3;
	int8_t s1;
	int8_t s2;
}Remote;

typedef __packed struct
{
	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t last_press_l;
	uint8_t last_press_r;
	uint8_t press_l;
	uint8_t press_r;
}Mouse;	
typedef	__packed struct
{
	uint16_t v;
}Key;

typedef __packed struct
{
	Remote rc;
	Mouse mouse;
	Key key;
}RC_Ctl_t;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart6;

extern DMA_HandleTypeDef  UART2RxDMA_Handler;

extern RC_Ctl_t RC_CtrlData;   //remote control data
extern uint8_t Send_Pc_Data[3]; 

extern pc_data_t pc_data;

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);
void MX_USART4_UART_Init(void);
void MX_USART6_UART_Init(void);

void Get_Remote_info(RC_Ctl_t *rc, uint8_t *pData);

void UsartReceive_IDLE(UART_HandleTypeDef *huart);

#endif 
