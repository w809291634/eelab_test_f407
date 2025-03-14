#include "buzzer.h"

/*********************************************************************************************
* ����:buzzer_init()
* ����:��������ʼ��
* ����:��
* ����:��
* �޸�:
* ע��:
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
* ����:buzzer_tweet()
* ����:��������һ��
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void buzzer_tweet(void)
{
  BUZZER_CTRL(ON);
  delay_ms(30);
  BUZZER_CTRL(OFF);
}
