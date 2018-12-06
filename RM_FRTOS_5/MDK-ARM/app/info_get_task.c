#include "main.h"

UBaseType_t info_stack_surplus;
uint32_t info_time_last;
int info_time_ms;
extern uint32_t pc_i;

void info_get_task(void const *argument)
{
	osEvent event;
	for(;;)
	{
		event = osSignalWait(GIMBAL_INFO_GET_SIGNAL |\
												 INFO_GET_SIGNAL,osWaitForever);
		
		if(event.status == osEventSignal)
		{
			if(event.value.signals & GIMBAL_INFO_GET_SIGNAL)
			{
				taskENTER_CRITICAL();
				
				get_gimbal_info();
				
				taskEXIT_CRITICAL();
			}
			if(event.value.signals & INFO_GET_SIGNAL)
			{
				info_time_ms = HAL_GetTick() - info_time_last;
				info_time_last = HAL_GetTick();
				
				taskENTER_CRITICAL();
				
				get_chassis_info();
				
				get_shoot_info();
				
				taskEXIT_CRITICAL();
				//Ni_Ming(0xf1,RC_CtrlData.rc.ch0,RC_CtrlData.rc.ch1,RC_CtrlData.rc.ch2,RC_CtrlData.rc.ch3);
				//Ni_Ming(0xf2,RC_CtrlData.rc.s1,RC_CtrlData.rc.s2,0,0);
				
				//Ni_Ming(0xf2,glb_cur.gimbal_cur[0],glb_cur.gimbal_cur[1],gim.pid.yaw_angle_ref,gim.pid.pit_angle_ref);
				//Ni_Ming(0xf2,pid_yaw_spd.out,gim.sensor.pit_relative_angle,pid_pit.out,pid_pit_spd.out);
			//Ni_Ming(0xf1,gyro_data.yaw,gyro_data.pitch_angle,gyro_data.raw_pitch,gyro_data.pitch);
//				printf("p=%f,y=%f,s=%d\r\n",pc_data.dynamic_pit,pc_data.dynamic_yaw,pc_data.star_shoot);
//				printf("%d\r\n",shoot.heat);
//				printf("s=%f h=%d f=%d\r\n",shoot.fric.shoot_fact_speed,shoot.heat ,bullet_data.bulletFreq);
				//Ni_Ming(0xf2,gim.pid.pit_angle_ref,gim.sensor.pit_relative_angle,pid_pit.out,pid_yaw.out);
//				Ni_Ming(0xf1,pc_data.yaw_befoer[(pc_i+ 1)%70], pc_data.dynamic_yaw, gim.sensor.yaw_relative_angle, pc_data.filter_yaw);
//				Ni_Ming(0xf1, pc_data.dynamic_yaw, -pc_data.filter_yaw , 0, 0);
//				Ni_Ming(0xf1, pc_data.dynamic_pit , pc_data.dynamic_yaw, gim.pid.yaw_angle_ref, 0);
//				printf("p=%f y=%f %d\r\n",shoot_buff_data.dynamic_pit,shoot_buff_data.dynamic_yaw ,shoot_buff_data.raw_pit_angle);
//				Ni_Ming(0xf1, moto_pit.total_angle, gyro_data.pitch, moto_pit.total_angle + gyro_data.pitch, 0);
//				Ni_Ming(0xf1, shoot.fric.fric_wheel_spd_ref[0], shoot.fric.shoot_fact_speed, 0, 0);
//				Ni_Ming(0xf1,gim.pid.yaw_angle_fdb, gim.pid.yaw_angle_ref, 0, 0);
//				Ni_Ming(0xf1,shoot.trig.trig_spd , shoot.trig.pos_ref, 0, 0);
//				Ni_Ming(0xf1, chassis.wheel_spd_ref[0], chassis.wheel_spd_fdb[0], chassis.wheel_spd_ref[2], chassis.wheel_spd_ref[3]);
//				Ni_Ming(0xf1,gyro_data.v_x,gyro_data.v_y,gyro_data.v_z,0);
//				Ni_Ming(0xf2,gyro_data.pitch ,gyro_data.yaw ,gyro_data.roll ,0);
			}
		//Ni_Ming(0xf1,gim.sensor.yaw_relative_angle,gim.sensor.pit_relative_angle,gyro_data.yaw,gyro_data.pitch);
		//printf("%f,%f",gim.sensor.yaw_relative_angle,gim.sensor.pit_relative_angle);
		}
		
		info_stack_surplus =uxTaskGetStackHighWaterMark (NULL);
	}
}
