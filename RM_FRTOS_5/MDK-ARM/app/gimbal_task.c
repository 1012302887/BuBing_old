#include "main.h"
#include "math.h"

/* stack usage monitor */
UBaseType_t gimbal_stack_surplus;

/* gimbal task global parameter */
gimbal_t gim;

uint32_t gimbal_time_last;
int gimbal_time_ms;
uint32_t handler_run_time = 0;
extern ramp_t pit_ramp;
extern ramp_t yaw_ramp;
extern TaskHandle_t can_msg_seng_task_t;
extern TaskHandle_t shoot_task_t;
extern TaskHandle_t info_get_task_t;
//static float pidin, ppidp, ppidi, ppidd, spidp, spidi, spidd = 0;
void gimbal_task(void const * argument)
{
	handler_run_time++;
	gimbal_time_ms = HAL_GetTick() - gimbal_time_last;
	gimbal_time_last = HAL_GetTick();
	
	/* gimbal mode switch */
	 switch (gim.ctrl_mode)
	{
		case GIMBAL_INIT:
			init_mode_handler();
		break;
		
		case GIMBAL_NORMAL:
			normal_mode_handler();
		break;
		
		case GIMBAL_WRITHE:
			normal_mode_handler();
		break;
		
		case GIMBAL_SUPPLY:
			supply_mode_handler();
		break;
		
		case GIMBAL_SHOOT_BUFF:
			shoot_buff_mode_handler();
		break;
		
		default:
		break;
	}
	
	/* gimbal pit and yaw position pid */
//	pid_pit.p = ppidp;
//	pid_pit.i = ppidi;
//	pid_pit.d = ppidd;
	
	pid_calc(&pid_yaw, gim.pid.yaw_angle_fdb, gim.pid.yaw_angle_ref);
	pid_calc(&pid_pit, gim.pid.pit_angle_fdb, gim.pid.pit_angle_ref);
	
	gim.pid.yaw_spd_ref = pid_yaw.out;
	gim.pid.pit_spd_ref = pid_pit.out;
 
	/* gimbal pid and yaw speed pid */
	gim.pid.yaw_spd_fdb = gim.sensor.yaw_palstance;
	gim.pid.pit_spd_fdb = gim.sensor.pit_palstance;
	
//	pid_pit_spd.p = spidp;
//	pid_pit_spd.i = spidi;
//	pid_pit_spd.d = spidd;
	
	pid_calc(&pid_yaw_spd, gim.pid.yaw_spd_fdb, gim.pid.yaw_spd_ref);
	pid_calc(&pid_pit_spd, gim.pid.pit_spd_fdb, gim.pid.pit_spd_ref);

	/* gimbal current out */
	glb_cur.gimbal_cur[0] = pid_yaw_spd.out;
	glb_cur.gimbal_cur[1] = pid_pit_spd.out;
	glb_cur.gimbal_cur[2] = 0;
	
	osSignalSet(can_msg_seng_task_t, GIMBAL_MOTOR_MSG_SEND);
	osSignalSet(info_get_task_t, GIMBAL_INFO_GET_SIGNAL);
	
	if(gim.ctrl_mode != GIMBAL_INIT)
	{
		osSignalSet(shoot_task_t, SHOT_TASK_EXE_SIGNAL);
		

		if(handler_run_time %1000 ==0)
		{
			HAL_UART_Transmit(&huart6, Send_Pc_Data, 3, 10);
		}
	}
	
	gimbal_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
}

void init_mode_handler(void)
{
	gim.pid.pit_angle_fdb = gim.sensor.pit_relative_angle;
  gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle;
	if(handler_run_time > 2000)
	{
		gim.ctrl_mode = GIMBAL_NORMAL;
	}
}

void normal_mode_handler(void)
{
	gim.pid.pit_angle_fdb = gim.sensor.pit_relative_angle;
  gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle;
}

void supply_mode_handler(void)
{
	gim.pid.pit_angle_fdb = gim.sensor.pit_relative_angle;
  gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle;
}

void shoot_buff_mode_handler(void)
{
	if(pc_data.shoot_data == 1)
	{
		gim.pid.yaw_angle_ref = 0;
		gim.pid.pit_angle_ref = -2;
		gim.delay++;
		if(gim.delay > 1000)
		{
			pc_data.shoot_data = 2;
			gim.delay = 0;
		}			
	}
	else if(pc_data.shoot_data == 2)
	{
		gim.pid.yaw_angle_ref = gim.yaw_shoot;
		gim.pid.pit_angle_ref = -gim.pit_shoot;	
	}

	gim.pid.pit_angle_fdb = gim.sensor.pit_relative_angle;
  gim.pid.yaw_angle_fdb = gim.sensor.yaw_relative_angle;
}

void gimbal_param_init(void)
{
	memset(&gim, 0, sizeof(gimbal_t));
	
	gim.ctrl_mode 		 = GIMBAL_INIT;
	gim.last_ctrl_mode = GIMBAL_INIT;
	
	/* pitch and yaw ramp Initializa */
	ramp_init(&pit_ramp, PIT_PREPARE_TIMS_MS);
	ramp_init(&yaw_ramp, YAW_PREPARE_TIMS_MS);
	
	/* pitch axis motor pid parameter */
  PID_struct_init(&pid_pit, POSITION_PID, 1000, 1000,
                  30, 0, 0); //30
  PID_struct_init(&pid_pit_spd, POSITION_PID, 7000, 2000,
                  60, 0, 0); //60

  /* yaw axis motor pid parameter */
  PID_struct_init(&pid_yaw, POSITION_PID, 1000, 1000,
                  7, 0, 0); //
  PID_struct_init(&pid_yaw_spd, POSITION_PID, 6000, 2000,
                  80, 2, 0);
									
	PID_struct_init(&pid_auto_pit, POSITION_PID, 1, 0,
                  0.0015 , 0, 0);
									
	pc_data.v_now_i = 0;
	
	Send_Pc_Data[0] = 0xAA;
	Send_Pc_Data[2] = 0xBB;
}
