#ifndef __can_H
#define __can_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "main.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

extern void Error_Handler(void);

void MX_CAN1_Init(void);
void MX_CAN2_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ can_H */
