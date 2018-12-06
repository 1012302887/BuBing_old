#ifndef __COMM_TASK_H__
#define __COMM_TASK_H__

#include "main.h"

#define GIMBAL_INFO_GET_SIGNAL ( 1 << 3 )
#define SHOOT_MOTOR_MSG_SEND   ( 1 << 4 )
#define GIMBAL_MOTOR_MSG_GET   ( 1 << 5 )
#define GIMBAL_MOTOR_MSG_SEND  ( 1 << 6 )
#define CHASSIS_MOTOR_MSG_SEND ( 1 << 7 )
#define SHOT_TASK_EXE_SIGNAL   ( 1 << 8 )
#define INFO_GET_SIGNAL   		 ( 1 << 9 )

typedef struct
{
  /* 4 chassis motor current */
  int16_t chassis_cur[4];
  /* yaw/pitch motor current */
  int16_t gimbal_cur[3];
	/* friction/trigger motor current */
	int16_t shoot_cur[3];
} motor_current_t;

void can_msg_send_task(void const *argument);

extern motor_current_t glb_cur;

#endif
