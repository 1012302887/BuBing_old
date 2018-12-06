#include "main.h"
#include "math.h"

/* stack usage monitor */
UBaseType_t shoot_stack_surplus;

/* shoot task global parameter */
shoot_t shoot = {0};

extern TaskHandle_t can_msg_seng_task_t;

extern float trig_angle_before;
extern float trig_angle_before_1;
extern uint32_t shoot_time;
uint32_t shoot_time_last;
int shoot_time_ms;
//static float pidin, ppidp, ppidi, ppidd, spidp, spidi, spidd = 0;
void shoot_task(void const * argument)
{
	osEvent event;
  
  for(;;)
  {
    event = osSignalWait(SHOT_TASK_EXE_SIGNAL, osWaitForever);
    
    if (event.status == osEventSignal)
    {
      if (event.value.signals & SHOT_TASK_EXE_SIGNAL)
      {
				 switch (shoot.mode)
				{
					case REMOTE_CTRL:
						shoot_remote_handler();
					break;
					
					case KEYBOR_CTRL:
						shoot_keyboard_handler();
						pid_fric_spd[0].i = 0;
						pid_fric_spd[1].i = 0;
					break;
					
					case SHOOT_BUFF:
						shoot_buff_handler();
						pid_fric_spd[0].i = 2;
						pid_fric_spd[1].i = 2;
					break;
					default:
					break;
				}
				
				if(shoot.heat > shoot.trig.max_ref)
					shoot.trig.spd_ref = 0;	

				glb_cur.shoot_cur[0] = chassis_pid_calc(&pid_fric_spd[0], shoot.fric.fric_wheel_spd_fdb[0], -shoot.fric.fric_wheel_spd_ref[0] * 28.571248f);
				glb_cur.shoot_cur[1] = chassis_pid_calc(&pid_fric_spd[1], shoot.fric.fric_wheel_spd_fdb[1],  shoot.fric.fric_wheel_spd_ref[1] * 28.571248f);
				glb_cur.shoot_cur[2] = chassis_pid_calc(&pid_trigger_spd, shoot.trig.trig_spd, shoot.trig.spd_ref);
				
				osSignalSet(can_msg_seng_task_t, SHOOT_MOTOR_MSG_SEND);
			}
		}
		
		shoot_stack_surplus = uxTaskGetStackHighWaterMark(NULL);
	}
}

void shoot_remote_handler(void)
{
	if(shoot.fric.switching == ON)
	{
		shoot.trig.delay++;
		shoot.trig.max_ref = 88888;
		shoot.fric.fric_wheel_spd_ref[0] = shoot.fric.fric_wheel_spd_ref[1] = 15;		
		if(shoot.trig.delay > 1000)
		{
			shoot.trig.spd_ref = -6;
		}
	}
	else
	{	
		shoot.trig.delay = 0;
		shoot.trig.spd_ref = 0;
		shoot.fric.fric_wheel_spd_ref[0] = shoot.fric.fric_wheel_spd_ref[1] = 0;	
	}
}

void shoot_keyboard_handler(void)
{
	if(shoot.fric.switching == ON)
	{
		if(RC_CtrlData.key.v & B)
		{
			shoot.fric.fric_wheel_spd_ref[0] = shoot.fric.fric_wheel_spd_ref[1] = 16;
		}
		else
		{
			shoot.fric.fric_wheel_spd_ref[0] = shoot.fric.fric_wheel_spd_ref[1] = 14;
		}
	}
	else
	{
		shoot.fric.fric_wheel_spd_ref[0] = shoot.fric.fric_wheel_spd_ref[1] = 0;
	}
	
	if(shoot.trig.switching == ON)
	{
		switch (shoot.level)
	  {		
		 case Lv_3:
			 if(shoot.buff & FIVEFOLD_HEAT)
			 {
				 shoot.trig.spd_ref = -15;
			 }
			 else
			 {
				shoot.trig.spd_ref = -6;
			 }				 
		 break;
		
		 case Lv_2:
			 shoot.trig.max_ref = 210;
			 if(shoot.buff & FIVEFOLD_HEAT)
			 {
				 shoot.trig.spd_ref = -10;
			 }
			 else
			 {
				 shoot.trig.spd_ref = -4;  //4
			 }
		 break;
		
		 default:
			 shoot.trig.max_ref = 100;
			 if(shoot.buff & FIVEFOLD_HEAT)
			 {
				 shoot.trig.spd_ref = -6;
			 }
			 else
			 {
				 shoot.trig.spd_ref = -2; //2~3
			 }
		 break;
	  }
	}
	else
	{
		shoot.trig.spd_ref = 0;
	}
	shoot.get_pos = 1;
}

void shoot_buff_handler(void)
{
	static uint8_t  shoot_buff_init = 1;
	static uint8_t  shoot_angle_reset = 0;
	static uint16_t shoot_wait = 0;
	static uint8_t  shoot_ok = 1;
	if((shoot.get_pos)|| (shoot_angle_reset % 5 == 0))
	{
		shoot.fric.shoot_buff_angle = shoot.trig.trig_pos;
	}
	shoot.trig.delay++;
	shoot.fric.fric_wheel_spd_ref[0] = shoot.fric.fric_wheel_spd_ref[1] = 20;
		
	if(pc_data.bug != pc_data.last_bug)
	{
		shoot_ok = 1;
	}
	
	if(shoot_ok == 1)
	{
		shoot_wait++;
		if(shoot_wait > 300)
		{
			shoot_angle_reset++;
			shoot.fric.shoot_buff_angle -= 51;
			shoot.trig.delay = 0;
			shoot.trig.pos_ref = shoot.fric.shoot_buff_angle;
			shoot_buff_init= 0;	
			shoot_wait = 0;
			shoot_ok = 0;
		}		
	}

	if((shoot.trig.delay > 300) || (shoot_buff_init))
	{
		shoot.trig.pos_ref = shoot.trig.trig_pos;
		shoot_buff_init = 1;
	}
	shoot.get_pos = 0;
	
	shoot.trig.spd_ref = pid_calc(&pid_trigger, shoot.trig.trig_pos , shoot.trig.pos_ref);
}

void shoot_param_init(void)
{
		 for (int k = 0; k < 2; k++)
  {
    PID_struct_init(&pid_fric_spd[k], POSITION_PID, 10000, 7000,
		150, 3, 0); 
	}
	
	shoot.trig.max_ref = 100;
	 /* bullet trigger motor pid parameter */
  PID_struct_init(&pid_trigger, POSITION_PID, 30, 2000,
                  0.5, 0, 0);
  PID_struct_init(&pid_trigger_spd, POSITION_PID, 10000, 5000,
                  1200, 2, 0);
}
