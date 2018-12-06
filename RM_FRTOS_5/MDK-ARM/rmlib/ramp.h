#ifndef __RAMP_H__
#define __RAMP_H__

#include "stm32f4xx_hal.h"

typedef struct ramp_t
{
  int32_t count;
  int32_t scale;
  float   out;
  void  (*init)(struct ramp_t *ramp, int32_t scale);
  float (*calc)(struct ramp_t *ramp);
}ramp_t;

#define RAMP_GEN_DAFAULT \
{ \
              .count = 0, \
              .scale = 0, \
              .out = 0, \
              .init = &ramp_init, \
              .calc = &ramp_calc, \
            } \
            
void  ramp_init(ramp_t *ramp, int32_t scale);
float ramp_calc(ramp_t *ramp);

#endif
