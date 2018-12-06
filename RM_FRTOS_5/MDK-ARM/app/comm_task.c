#include "main.h"

/* stack usage monitor */
UBaseType_t can_send_surplus;

/* communicate task global parameter */
motor_current_t glb_cur;

void can_msg_send_task(void const *argument)
{
	osEvent event;
	for(;;)
	{
		event = osSignalWait(SHOOT_MOTOR_MSG_SEND    | \
												 GIMBAL_MOTOR_MSG_SEND   | \
                         CHASSIS_MOTOR_MSG_SEND, osWaitForever);
		
		 if (event.status == osEventSignal)
    {
      if (event.value.signals & CHASSIS_MOTOR_MSG_SEND)
      {
//        can1_time_ms = HAL_GetTick() - can1_time_last;
//        can1_time_last = HAL_GetTick();
        send_chassis_motor_ctrl_message(glb_cur.chassis_cur);
      }
			else if (event.value.signals & GIMBAL_MOTOR_MSG_SEND)
      {
//        can_time_ms = HAL_GetTick() - can_time_last;
//        can_time_last = HAL_GetTick();
				send_gimbal_motor_ctrl_message(glb_cur.gimbal_cur);
      }
			
			if (event.value.signals & SHOOT_MOTOR_MSG_SEND)
			{
				send_shoot_motor_ctrl_message(glb_cur.shoot_cur);
			}
		
    }
		can_send_surplus = uxTaskGetStackHighWaterMark(NULL);
	}
}
