#ifndef __BUZZER_H__
#define __BUZZER_H__

#include "extend.h"
#include "led.h"
#include "delay.h"

#define BUZZER_RCC              RCC_AHB1Periph_GPIOG
#define BUZZER_PORT             GPIOG
#define BUZZER_PIN              GPIO_Pin_1

#define BUZZER_CTRL(a)          if(!a) GPIO_SetBits(BUZZER_PORT, BUZZER_PIN); \
                                else GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN)

#define ON      0
#define OFF     1

void buzzer_init(void);
void buzzer_tweet(void);

#endif
