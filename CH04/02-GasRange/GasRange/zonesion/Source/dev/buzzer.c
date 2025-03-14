#include "buzzer.h"

/*********************************************************************************************
* 名称:buzzer_init()
* 功能:蜂鸣器初始化
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void buzzer_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(BUZZER_RCC, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  
  GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
  GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
  
  BUZZER_CTRL(OFF);
}

/*********************************************************************************************
* 名称:buzzer_tweet()
* 功能:蜂鸣器响一下
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void buzzer_tweet(void)
{
  BUZZER_CTRL(ON);
  delay_ms(30);
  BUZZER_CTRL(OFF);
}
