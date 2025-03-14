#include "timer.h"

void timer_init(void)
{
  TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  TIM_BaseInitStructure.TIM_Period = 1000 - 1;                  //10ms一次中断
  TIM_BaseInitStructure.TIM_Prescaler = 840 - 1;

  TIM_TimeBaseInit(TIM7, &TIM_BaseInitStructure);
  
  TIM_ClearFlag(TIM7, TIM_FLAG_Update); 
  TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);
  
  TIM_Cmd(TIM7, ENABLE); 
}

extern uint8_t rgb_twinkle;
extern uint8_t tim_second;
extern uint8_t mode_twinkle;
uint8_t count_sec = 0;
uint8_t count_rgb = 0;
uint8_t count_mode = 0;
uint32_t ticks = 0;                                             // 系统的运行时间计数值

void TIM7_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET){
    ticks++;
    
    if((count_rgb % 20) == 19){
      rgb_twinkle = 1;
      count_rgb = 0; 
    }else count_rgb ++; 
    
    if((count_sec % 50) == 49){
      count_sec = 0;
      tim_second = 1;
    }else  count_sec ++;
    
    if((count_mode % 40) == 39){
      mode_twinkle = 1;
      count_mode = 0;
    }else count_mode ++;
    
    TIM_ClearITPendingBit(TIM7 , TIM_IT_Update); 
  }
}

/*********************************************************************************************
* 名称：xtime()
* 功能：系统的运行时间获取
* 参数：无
* 返回：系统的运行时间，单位为ms，注意这里需要和配置tim的时基相关
* 修改：
* 注释：
*********************************************************************************************/
uint32_t xtime(void){
  return (uint32_t)(ticks*10);
}