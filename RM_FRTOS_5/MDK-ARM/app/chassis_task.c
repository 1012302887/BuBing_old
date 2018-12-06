#include "main.h"
#include "math.h"

/* stack usage monitor */
UBaseType_t chassis_stack_surplus;

/* chassis task global parameter */
chassis_t chassis = {0};

uint32_t chassis_time_last;
int chassis_time_ms;
static float d_theta = 0;
extern ramp_t FBSpeedRamp;
extern ramp_t LRSpeedRamp;
extern TaskHandle_t can_msg_seng_task_t;
//static float pidin, ppidp, ppidi, ppidd, spidp, spidi, spidd = 0;

/* chassis task*/
void chassis_task(void const *argument)
{
	chassis_time_ms = HAL_GetTick() - chassis_time_last;
	chassis_time_last = HAL_GetTick();
	
	if(gim.ctrl_mode == GIMBAL_INIT)//chassis dose not follow gimbal when gimbal initializa
	{
		chassis.vw = 0;
	}
	else if(gim.ctrl_mode == GIMBAL_NORMAL)
	{
		chassis.vw = pid_calc(&pid_rotate, chassis.follow_gimbal, 0);
	}
	else if(gim.ctrl_mode == GIMBAL_WRITHE)
	{
		if((chassis.vx_offset == 0) && (chassis.vy_offset == 0))
		{
			if(chassis.follow_gimbal > STATIC_WRITHE_ANGLE_LIMIT)
			{
				chassis.writhe_speed_fac =  1;
			}
			else if(chassis.follow_gimbal < -STATIC_WRITHE_ANGLE_LIMIT)
			{
				chassis.writhe_speed_fac =  -1;
			}
		}
		else
		{
			if(chassis.follow_gimbal > RUN_WRITHE_ANGLE_LIMIT)
			{
				chassis.writhe_speed_fac =  1;
			}
			else if(chassis.follow_gimbal < -RUN_WRITHE_ANGLE_LIMIT)
			{
				chassis.writhe_speed_fac =  -1;
			}
		}	
		
		chassis.vw = pid_calc(&pid_rotate, chassis.writhe_speed_fac * WRITHE_SPEED_LIMIT, 0);
	}
	else if(gim.ctrl_mode == GIMBAL_SUPPLY)
	{
		chassis.vw = chassis.vw_offset;
	}
	else if(gim.ctrl_mode == GIMBAL_SHOOT_BUFF)
	{
		chassis.vw = 0;
	}
	
/* run straight line with waist */
	if(chassis.follow_gimbal < 0){
		d_theta = chassis.follow_gimbal - 0;//rad
	}
	else if(chassis.follow_gimbal <= 0+ 180 ){
		d_theta = chassis.follow_gimbal - 0;
	}
	else if(chassis.follow_gimbal <= 360){
		d_theta = 360 - chassis.follow_gimbal + 0;
	}
	d_theta /= -57.295;
	
	chassis.vx = chassis.vx_offset * cos(d_theta) - chassis.vy_offset * sin(d_theta);
	chassis.vy = chassis.vx_offset * sin(d_theta) + chassis.vy_offset * cos(d_theta);
	
	chassis.wheel_spd_ref[0] = -chassis.vx + chassis.vy + chassis.vw;
	chassis.wheel_spd_ref[1] =  chassis.vx + chassis.vy + chassis.vw;
	chassis.wheel_spd_ref[2] = -chassis.vx - chassis.vy + chassis.vw;
	chassis.wheel_spd_ref[3] =  chassis.vx - chassis.vy + chassis.vw;
	

	if(gim.stop == 1)
	{
			for(int i =0; i < 4; i++)
		{
			chassis.wheel_spd_ref[i] = 0;
		}
	}

	for(int i =0; i < 4; i++)
	{
		chassis.current[i] = chassis_pid_calc(&pid_spd[i], chassis.wheel_spd_fdb[i], chassis.wheel_spd_ref[i]);
	}
	
	memcpy(glb_cur.chassis_cur, chassis.current, sizeof(chassis.current));
	osSignalSet(can_msg_seng_task_t, CHASSIS_MOTOR_MSG_SEND);
	
	chassis_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
}

/* chissis Initialization*/
void chassis_param_init(void)
{
	memset(&chassis, 0, sizeof(chassis_t));
	
	chassis.writhe_speed_fac = -1;
	
	/* initializa chassis speed ramp */
	ramp_init(&LRSpeedRamp, MOUSR_LR_RAMP_TICK_COUNT);
	ramp_init(&FBSpeedRamp, MOUSR_FB_RAMP_TICK_COUNT);
	
	/* initializa chassis follow gimbal pid */
		PID_struct_init(&pid_rotate, POSITION_PID, 40, 0, 
		1.7, 0, 0);//2.0
	
	/* initializa chassis wheels position pid */
//	 for (int k = 0; k < 4; k++)
//  {
//    PID_struct_init(&pid_pos[k], POSITION_PID, 6, 6,
//		0.001, 0, 0); 
//	}
	/* initializa chassis wheels speed pid */
	 for (int k = 0; k < 4; k++)
  {
    PID_struct_init(&pid_spd[k], POSITION_PID, 8000, 0,
		400, 2, 0); 
	}
}
