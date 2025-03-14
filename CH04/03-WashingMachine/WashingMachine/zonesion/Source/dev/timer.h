#ifndef __TIMER_H__
#define __TIMER_H__

#include "stm32f4xx.h"
extern uint32_t ticks;

void timer_init(void);
uint32_t xtime(void);

#endif

