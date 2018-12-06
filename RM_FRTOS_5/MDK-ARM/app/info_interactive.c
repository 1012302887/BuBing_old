#include "main.h"
#include "math.h"

static uint8_t auto_shoot = 0;
uint32_t pc_i = 0; 
uint32_t shoot_time = 0;
float trig_angle_before = 0;
float trig_angle_before_1 = 0;
ramp_t pit_ramp 	 = RAMP_GEN_DAFAULT;
ramp_t yaw_ramp    = RAMP_GEN_DAFAULT;
ramp_t FBSpeedRamp = RAMP_GEN_DAFAULT;
ramp_t LRSpeedRamp = RAMP_GEN_DAFAULT;

/************************************		CHASSIS INFO    *****************************************/
/*
																				CHASSIS INFO


																																																*/
/*left wheel:2   right wheel:1 */

/*left wheel:4   right wheel:4 */
void get_chassis_info(void)
{
	/* get gimbal and chassis relative angle */
	chassis.follow_gimbal = moto_yaw.total_angle;

	/* get chassis wheel fdb positin */
  for (uint8_t i = 0; i < 4; i++)
  {
    chassis.wheel_pos_fdb[i] = moto_chassis[i].total_angle/19.0f;
  }
  /* get chassis wheel fdb speed */
  for (uint8_t i = 0; i < 4; i++)
  {
    chassis.wheel_spd_fdb[i] = moto_chassis[i].filter_rate/19.0;
  }
	
	/* get the power surplus from judgment info */
	
	chassis.power_surplus = 1;//InfantryJudge.remainPower / 60;
	
	/* get remote and keyboard chassis control information */
  keyboard_chassis_hook();
  remote_ctrl_chassis_hook();
}

void send_chassis_motor_ctrl_message(int16_t chassis_cur[])
{
	 send_chassis_cur(chassis_cur[0] * chassis.power_surplus, chassis_cur[1] *chassis.power_surplus, 
										chassis_cur[2] * chassis.power_surplus, chassis_cur[3] *chassis.power_surplus);
}

/************************************* CHASSIS INFO ********************************************/




/************************************* GIMBAL  INFO *********************************************/
/*
																			 GIMBAL  INFO


																																																 */
void get_gimbal_info(void)
{
	if(gim.ctrl_mode == GIMBAL_INIT)
	{
		/* get gimbal relative angle */
		gim.sensor.yaw_relative_angle = moto_yaw.total_angle * ramp_calc(&yaw_ramp);//
		gim.sensor.pit_relative_angle = gyro_data.pitch * ramp_calc(&pit_ramp);
		
		gim.sensor.yaw_offset_angle   = gyro_data.yaw;
	}
	else if((gim.ctrl_mode == GIMBAL_NORMAL) || (gim.ctrl_mode == GIMBAL_WRITHE))
	{
		gim.sensor.yaw_relative_angle = gyro_data.yaw   - gim.sensor.yaw_offset_angle;
		gim.sensor.pit_relative_angle = gyro_data.pitch - gim.sensor.pit_offset_angle; 
		pid_yaw.p = 5;
		// supply offset angle
		gim.sensor.yaw_supply_offset_angle = moto_yaw.total_angle;
		
		// shoot big buff offset angle
		gim.sensor.yaw_shoot_buff_offset_angle = 0;//moto_yaw.total_angle;
		
		ramp_init(&pit_ramp, PIT_PREPARE_TIMS_MS);
		ramp_init(&yaw_ramp, YAW_PREPARE_TIMS_MS);
	}
	else if(gim.ctrl_mode == GIMBAL_SUPPLY)
	{
		gim.sensor.yaw_relative_angle = moto_yaw.total_angle - gim.sensor.yaw_supply_offset_angle;
		gim.sensor.pit_relative_angle = gyro_data.pitch;
		
		gim.sensor.yaw_offset_angle   = gyro_data.yaw;
	}
	else if(gim.ctrl_mode == GIMBAL_SHOOT_BUFF)
	{
		gim.sensor.yaw_relative_angle = moto_yaw.total_angle - gim.sensor.yaw_shoot_buff_offset_angle;
		gim.sensor.pit_relative_angle = -(moto_pit.total_angle - gim.sensor.pit_shoot_buff_offset_angle);
		pid_yaw.p = 15;
		// get usart shoot buff angle
		gim.yaw_shoot = pc_data.dynamic_yaw;
		gim.pit_shoot = pc_data.dynamic_pit;
		
		// normal gimbal offset angle
//		gim.sensor.yaw_offset_angle   = 0;//gyro_data.yaw;
//		gim.sensor.pit_offset_angle   = 0;//gyro_data.pitch; 
	}
	
	pc_data.time = GameRobotState.remain_time;
	
	if(pc_data.time > 240)
	{
		Send_Pc_Data[1] = 0x10; //small buff
	}
	else
	{
		Send_Pc_Data[1] = 0x20; //big buff
	}
	
	
	if((handler_run_time-shoot_time) % 1000 == 0)
	{
		trig_angle_before = shoot.trig.trig_pos;
	}
	if(handler_run_time%1000 == 0)
	{
		trig_angle_before_1 = shoot.trig.trig_pos;
	}
	/* get gimbal relative palstance */
  //the Z axis(yaw) of gimbal coordinate system corresponds to the IMU Z axis
  gim.sensor.yaw_palstance = gyro_data.v_z;
  //the Y axis(pitch) of gimbal coordinate system corresponds to the IMU -X axis
  gim.sensor.pit_palstance = gyro_data.v_x;
	
	/* get remote and keyboard gimbal control information */
  keyboard_gimbal_hook();
  remote_ctrl_gimbal_hook();
	
	GimbalAngleLimit();
}

void send_gimbal_motor_ctrl_message(int16_t gimbal_cur[])
{
  /* 0: yaw motor current
     1: pitch motor current
     2: trigger motor current*/
  send_gimbal_cur(-gimbal_cur[0], gimbal_cur[1], gimbal_cur[2]);
}

/************************************* GIMBAL  INFO *********************************************/




/************************************* SHOOT   INFO *********************************************/
/*
																			 SHOOT   INFO

																																																 */

void get_shoot_info(void)
{
	shoot.hp							 				= GameRobotState.robotHp;
	shoot.heat        		 				= InfantryJudge.shooterHeat17;
	shoot.buff            				= GameRobotState.robotbuff;
	shoot.level       				  	= GameRobotState.robotLevel;
	shoot.trig.trig_spd 				  = moto_trigger.filter_rate / 36;
	shoot.trig.trig_pos        		= moto_trigger.total_angle / 36;
	shoot.fric.shoot_fact_speed   = bullet_data.bulletSpeed;
	
	  /* get friction wheel fdb speed */
  for (uint8_t i = 0; i < 2; i++)
  {
    shoot.fric.fric_wheel_spd_fdb[i] = moto_friction[i].filter_rate * (6.2832 / 8.192);
  }
	
	/* get remote and keyboard trig and friction wheel control information */
	
	pc_data.last_bug = pc_data.bug;
	pc_data.bug = pc_data.star_shoot;
	
  remote_ctrl_shoot_hook();
	keyboard_shoot_hook();
}

void send_shoot_motor_ctrl_message(int16_t shoot_cur[])
{
  /* 0: up friction wheel motor current
     1: down friction wheel motor current
     2: trigger motor current*/
  send_shoot_cur(shoot_cur[0], shoot_cur[1], shoot_cur[2]);
}

/************************************* SHOOT   INFO *********************************************/



static uint32_t turn_time_last = 0;
/******************************** CHASSIS REMOTE  HANDLER ***************************************/

void remote_ctrl_chassis_hook(void)
{
	if((gim.ctrl_mode != GIMBAL_INIT) && (ctrl_mode == REMOTE_CTRL))
	{
		/* get chassis wheel ref speed */
		if(ramp_mode == RAMP_UP)
		{
			chassis.vx_offset = RC_CtrlData.rc.ch1 * 30.0f / 660;
			chassis.vy_offset = RC_CtrlData.rc.ch0 * 30.0f / 660;
		}
		else
		{
			chassis.vx_offset = RC_CtrlData.rc.ch1 * CHASSIS_REF_FACT;
			chassis.vy_offset = RC_CtrlData.rc.ch0 * CHASSIS_REF_FACT;
		}
	}
}

void keyboard_chassis_hook(void)
{
	static float forward_back_speed = 0;
	static float left_right_speed 	 = 0;
	if((gim.ctrl_mode != GIMBAL_INIT) && (ctrl_mode == KEYBOR_CTRL))
	{
		/*********** speed mode: normal speed/high speed ***********/                   
		if(ramp_mode == RAMP_UP)
		{
			forward_back_speed = 30.0f;
			left_right_speed   = 30.0f;
		}
		else if(RC_CtrlData.key.v & SHIFT)																	
		{
			forward_back_speed = HIGH_FORWARD_BACK_SPEED;
			left_right_speed 	 = HIGH_LEFT_RIGHT_SPEED;
		}
		else if(gim.ctrl_mode == GIMBAL_WRITHE)
		{
			forward_back_speed = 40.0f;
			left_right_speed 	 = 30.0f;
		}
		else
		{
			forward_back_speed = NORMAL_FORWARD_BACK_SPEED;
			left_right_speed   = NORMAL_LEFT_RIGHT_SPEED;
		}
		
		/*********** get forward chassis wheel ref speed ***********/
		if(RC_CtrlData.key.v & W)
		{
			chassis.vx_offset =  forward_back_speed * ramp_calc(&FBSpeedRamp);
		}
		else if(RC_CtrlData.key.v & S)
		{
			chassis.vx_offset = -forward_back_speed * ramp_calc(&FBSpeedRamp);
		}
		else
		{
			chassis.vx_offset = 0;
			ramp_init(&FBSpeedRamp, MOUSR_FB_RAMP_TICK_COUNT);
		}
		
		/*********** get rightward chassis wheel ref speed ***********/
		if(RC_CtrlData.key.v & A)
		{
			chassis.vy_offset = -left_right_speed * ramp_calc(&LRSpeedRamp);
		}
		else if(RC_CtrlData.key.v & D)
		{
			chassis.vy_offset = left_right_speed * ramp_calc(&LRSpeedRamp);
		}
		else
		{
			chassis.vy_offset = 0;
			ramp_init(&LRSpeedRamp, MOUSR_LR_RAMP_TICK_COUNT);
		}
		
		/*********** get chassis rotate ref ***********/
		if(RC_CtrlData.key.v & Q && auto_shoot != 1)
		{
			if(gim.ctrl_mode == GIMBAL_SUPPLY)
			{
				chassis.vw_offset = -30.0f ;//* ramp_calc(&LRSpeedRamp);;
			}
			else
			{
				gim.pid.yaw_angle_ref += CHASSIS_ROTATE_FACT;
			}
		}
		else if(RC_CtrlData.key.v & E && auto_shoot != 1)
		{
			if(gim.ctrl_mode == GIMBAL_SUPPLY)
			{
				chassis.vw_offset = 30.0f ;
			}
			else
			{
					gim.pid.yaw_angle_ref -= CHASSIS_ROTATE_FACT;
			}
		}
		else
		{
			chassis.vw_offset = 0;
		}
		
		/*********** chassis mode is waist ***********/
		if((RC_CtrlData.key.v & CTRL) && (handler_run_time -turn_time_last>350))																
		{
			turn_time_last = handler_run_time;
			
			if(gim.ctrl_mode == GIMBAL_NORMAL)
			{	
				gim.ctrl_mode = GIMBAL_WRITHE;
			}
			else
			{
				gim.ctrl_mode = GIMBAL_NORMAL;
			}
		}
		
	}
	
}

/********************************* GIMBAL REMOTE  HANDLER ***************************************/
void remote_ctrl_gimbal_hook(void)
{
	if((gim.ctrl_mode != GIMBAL_INIT) && (ctrl_mode == REMOTE_CTRL))
		{					
			/* get remote gimbal info */
		  gim.pid.yaw_angle_ref -= RC_CtrlData.rc.ch2 * GIMBAL_YAW_REF_FACT;
			gim.pid.pit_angle_ref -= RC_CtrlData.rc.ch3 * GIMBAL_PIT_REF_FACT;
		}
}
static float add_angle = 0;
void keyboard_gimbal_hook(void)
{	
	if((gim.ctrl_mode != GIMBAL_INIT) && (ctrl_mode == KEYBOR_CTRL))
	{
		VAL_LIMIT(RC_CtrlData.mouse.x, -128, 128); 
		VAL_LIMIT(RC_CtrlData.mouse.y, -18, 32);   
		/* get remote gimbal info */
		pc_i++;
		pc_data.yaw_befoer[pc_i%100] = gim.sensor.yaw_relative_angle; 
		if(auto_shoot == 1)
		{	
			pid_yaw.p = 20;
			pid_calc(&pid_auto_pit, pc_data.filter_pit, 0);
			if(RC_CtrlData.key.v & Q )      {add_angle += 0.01f;}
			else if(RC_CtrlData.key.v & E ) {add_angle -= 0.01f;}
			else {add_angle = 0;}
			gim.pid.yaw_angle_ref  = pc_data.filter_yaw-RC_CtrlData.mouse.x*0.2f+add_angle;	
			gim.pid.pit_angle_ref += RC_CtrlData.mouse.y * MOUSE_TO_PITCH_ANGLE_INC_FACT ;
	//		gim.pid.pit_angle_ref -= pid_auto_pit.out;
		}
		else if(gim.ctrl_mode == GIMBAL_SUPPLY)
		{
			gim.pid.yaw_angle_ref	 = 0;
			gim.pid.pit_angle_ref += RC_CtrlData.mouse.y * MOUSE_TO_PITCH_ANGLE_INC_FACT;
		}
		else if(gim.ctrl_mode == GIMBAL_SHOOT_BUFF)
		{
//			RC_CtrlData.mouse.x = 0;
//			RC_CtrlData.mouse.y = 0;
		}
		else 
		{
			pc_data.v_now_i = 0;
			pid_yaw.p = 10;
			gim.pid.yaw_angle_ref -= RC_CtrlData.mouse.x * MOUSE_TO_YAW_ANGLE_INC_FACT  ;//+ shoot_buff_data.dynamic_yaw * 0.015f;
			gim.pid.pit_angle_ref += RC_CtrlData.mouse.y * MOUSE_TO_PITCH_ANGLE_INC_FACT ;//+ shoot_buff_data.dynamic_pit * 0.015f;
		}
	}	
	else
	{
		RC_CtrlData.mouse.x = 0;
		RC_CtrlData.mouse.y = 0;
	}
		     
	/* car is or is not supply state */
  if((RC_CtrlData.key.v & Z) && (handler_run_time -turn_time_last>350))																
	{
		turn_time_last = handler_run_time;
		
		if(gim.ctrl_mode == GIMBAL_NORMAL)
		{	
			HATCH_COVER   = OPEN_COVER;
			gim.ctrl_mode = GIMBAL_SUPPLY;
		}
		else
		{
			HATCH_COVER   = CLOSE_COVER;
			gim.ctrl_mode = GIMBAL_NORMAL;
		}
	}
	
	/* car is or is not shoot_buff state */
	if((RC_CtrlData.key.v & R) && (handler_run_time -turn_time_last>350))																
	{
		turn_time_last = handler_run_time;
		
		if(gim.ctrl_mode == GIMBAL_NORMAL)
		{	
			gim.ctrl_mode = GIMBAL_SHOOT_BUFF;
			shoot.mode    = SHOOT_BUFF;
			pc_data.shoot_data = 1;
		}
		else
		{
			gim.ctrl_mode = GIMBAL_NORMAL;
			shoot.mode    = KEYBOR_CTRL;
			pc_data.shoot_data = 0;
		}
	}
	
	/* car is or is not autushoot */
	if(RC_CtrlData.mouse.press_r == 1)
	{
		auto_shoot = 1;
	}
	else
	{
		auto_shoot = 0;
	}
	
}

/*********************************** SHOOT REMOTE  HANDLER **************************************/

void remote_ctrl_shoot_hook(void)
{
	if(shoot.mode == REMOTE_CTRL)
	{
		shoot.fric.switching = RC_CtrlData.rc.s1;
		shoot.mode = RC_CtrlData.rc.s2;
	}
	else if(shoot.mode != SHOOT_BUFF)
	{
		shoot.mode = RC_CtrlData.rc.s2;
	}		
}

void keyboard_shoot_hook(void)
{
	if((RC_CtrlData.key.v & F) && (handler_run_time -turn_time_last>350))																
	{
		turn_time_last = handler_run_time;
		
		if(shoot.fric.switching == ON)
			shoot.fric.switching  = OFF;
		else
			shoot.fric.switching  = ON;
	}
	
	if((RC_CtrlData.mouse.press_l == 1) && (shoot.fric.switching == ON))
	{
		shoot.trig.switching = ON;
	}
	else
	{
		shoot.trig.switching = OFF;
	}
	
//	if((RC_CtrlData.key.v & B) && (handler_run_time -turn_time_last>350))																
//	{
//		turn_time_last = handler_run_time;
//		
//		if(shoot.fric.shoot_speed_high == 0)
//		{
//			shoot.fric.shoot_speed_high = 1;
//		}
//		else
//		{
//			shoot.fric.shoot_speed_high = 0;
//		}
//	}
}
/*********************************** SHOOT REMOTE  HANDLER **************************************/

void GimbalAngleLimit(void)
{
	if(ramp_mode == RAMP_UP)
	{
		VAL_LIMIT(gim.pid.pit_angle_ref, -50 ,-5);  
	}
	else if(ramp_mode == RAMP_DOWN)
	{
		VAL_LIMIT(gim.pid.pit_angle_ref, -50 ,30);  
	}
	else
	{
		VAL_LIMIT(gim.pid.pit_angle_ref, -40 ,12);
	}
	VAL_LIMIT(gim.pid.yaw_angle_ref, gim.sensor.yaw_relative_angle-30, gim.sensor.yaw_relative_angle+30);  
}
