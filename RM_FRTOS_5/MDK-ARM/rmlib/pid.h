#ifndef __pid_H__
#define __pid_H__

#include "stm32f4xx.h"

enum
{
  LLAST = 0,
  LAST,
  NOW,
  POSITION_PID,
  DELTA_PID,
};
typedef struct pid_t
{
  float p;
  float i;
  float d;

  float set;
  float get;
  float err[3];

  float pout;
  float iout;
  float dout;
  float out;
  
  uint32_t pid_mode;
  uint32_t max_out;
  uint32_t integral_limit;

  void (*f_param_init)(struct pid_t *pid, 
                       uint32_t      pid_mode,
                       uint32_t      max_output,
                       uint32_t      inte_limit,
                       float         p,
                       float         i,
                       float         d);
  void (*f_pid_reset)(struct pid_t *pid, float p, float i, float d);
 
} pid_t;

void PID_struct_init(
    pid_t*   pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,

    float kp,
    float ki,
    float kd);

float pid_calc(pid_t *pid, float fdb, float ref);
float chassis_pid_calc(pid_t *pid, float fdb, float ref);
		
extern pid_t pid_auto_yaw;
extern pid_t pid_auto_pit;
extern pid_t pid_pit;
extern pid_t pid_yaw;
extern pid_t pid_pit_spd;
extern pid_t pid_yaw_spd;
extern pid_t pid_trigger;
extern pid_t pid_trigger_spd;
extern pid_t pid_fric[2];
extern pid_t pid_fric_spd[2];

extern pid_t pid_rotate;
extern pid_t pid_pos[4];
extern pid_t pid_spd[4];

#endif
