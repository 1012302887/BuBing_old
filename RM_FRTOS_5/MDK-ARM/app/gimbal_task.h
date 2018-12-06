#ifndef __GIMBAL_TASK_H__
#define __GIMBAL_TASK_H__

#include "stm32f4xx_hal.h"

#define PIT_PREPARE_TIMS_MS			 200
#define YAW_PREPARE_TIMS_MS			 1000

typedef enum
{
  GIMBAL_INIT          = 0,
	GIMBAL_NORMAL        = 1,
	GIMBAL_WRITHE        = 2,
	GIMBAL_SUPPLY				 = 3,
  GIMBAL_SHOOT_BUFF    = 4,
	GIMBAL_AUTO_SHOOT    = 5,
} gimbal_mode_e;

typedef struct
{
	float normal_yaw_angle_ref;
	float supply_yaw_angle_ref;
	
  /* position loop */
  float yaw_angle_ref;
  float pit_angle_ref;
  float yaw_angle_fdb;
  float pit_angle_fdb;
  /* speed loop */
  float yaw_spd_ref;
  float pit_spd_ref;
  float yaw_spd_fdb;
  float pit_spd_fdb;
} gim_pid_t;

typedef struct
{
  /* unit: degree */
  float pit_relative_angle;
  float yaw_relative_angle;
  float gyro_angle;
  /* uint: degree/s */
  float yaw_palstance;
  float pit_palstance;
	
	  /* gimbal information */
  float         pit_offset_angle;
  float         yaw_offset_angle;
	
	float         pit_supply_offset_angle;
  float         yaw_supply_offset_angle;
	
	float					yaw_shoot_buff_offset_angle;
	float					pit_shoot_buff_offset_angle;
} gim_sensor_t;

typedef struct
{
  /* ctrl mode */
  gimbal_mode_e ctrl_mode;
  gimbal_mode_e last_ctrl_mode;
  
	/* gimbal information */
  gim_sensor_t  sensor;

  /* gimbal ctrl parameter */
  gim_pid_t     pid;
	
	int16_t 			delay;
	int16_t				limit_delay;
	uint8_t       stop;
	
	float pit_shoot;
	float yaw_shoot;
} gimbal_t;

extern gimbal_t gim;
extern uint32_t handler_run_time;

void gimbal_param_init(void);
void gimbal_task(void const * argument);
void init_mode_handler(void);
void normal_mode_handler(void);
void supply_mode_handler(void);
void shoot_buff_mode_handler(void);
void autoshoot_mode_handler(void);


#endif
