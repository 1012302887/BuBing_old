#ifndef __INFO_INTER_H__
#define __INFO_INTER_H__

#include "main.h"

/**********************************************************************************
 * bit      :15   14   13   12   11   10   9   8   7   6     5     4   3   2   1
 * keyboard : V    C    X	  Z    G    F    R   E   Q  CTRL  SHIFT  D   A   S   W
 **********************************************************************************/
#define W 			0x0001		
#define S 			0x0002
#define A 			0x0004
#define D 			0x0008
#define SHIFT 	0x0010
#define CTRL 		0x0020
#define Q 			0x0040
#define E				0x0080
#define R 			0x0100
#define F 			0x0200
#define G 			0x0400
#define Z 			0x0800
#define X 			0x1000
#define C 			0x2000
#define V 			0x4000		
#define B				0x8000
/******************************************************/

#define NORMAL_FORWARD_BACK_SPEED 			40.0f  						 //normal forward   speed
#define NORMAL_LEFT_RIGHT_SPEED   			30.0f							 //normal rightward speed
#define HIGH_FORWARD_BACK_SPEED 			  60.0f							 //faster forward   speed								
#define HIGH_LEFT_RIGHT_SPEED   				40.0f							 //faster rightward speed

/* gimbal remote control angle factor */
#define GIMBAL_PIT_REF_FACT 		0.0001f
#define GIMBAL_YAW_REF_FACT     0.001f
#define MOUSE_TO_PITCH_ANGLE_INC_FACT   0.01f
#define MOUSE_TO_YAW_ANGLE_INC_FACT     0.01f 

/* chassis remote control velocity factor */
#define CHASSIS_REF_FACT   			60.0/660.0
#define CHASSIS_ROTATE_FACT			3.0f

/* limit handler */
#define VAL_LIMIT(val, min, max)\
if(val<=min)\
{\
	val = min;\
}\
else if(val>=max)\
{\
	val = max;\
}\

#define       CAR_NUM      4
#define				HATCH_COVER    TIM4->CCR1

#if   CAR_NUM == 1
	#define 			OPEN_COVER		 			1380
	#define  			CLOSE_COVER    			2350
  #define				AUTOSHOOT_X_OFFSET  0
#elif CAR_NUM == 4
	#define 			OPEN_COVER		      888
	#define  			CLOSE_COVER         1766
  #define				AUTOSHOOT_X_OFFSET  -2
#else
	#define 			OPEN_COVER		      1300
	#define  			CLOSE_COVER         2150
  #define				AUTOSHOOT_X_OFFSET  0
#endif

/*			chassis info handle			 */
void get_chassis_info(void);
void send_chassis_motor_ctrl_message(int16_t chassis_cur[]);

/*			gimbal  info handle		   */
void get_gimbal_info(void);
void send_gimbal_motor_ctrl_message(int16_t gimbal_cur[]);
/*			shoot   info handle		   */
void get_shoot_info(void);
void send_shoot_motor_ctrl_message(int16_t shoot_cur[]);

/*	 limit gimbal remote angle   */
void GimbalAngleLimit(void);

/*	remote handler	*/
void keyboard_chassis_hook(void);
void remote_ctrl_chassis_hook(void);
void keyboard_gimbal_hook(void);
void remote_ctrl_gimbal_hook(void);
void keyboard_shoot_hook(void);
void remote_ctrl_shoot_hook(void);

#endif
