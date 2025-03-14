/*********************************************************************************************
* 定时器头文件
*********************************************************************************************/
#include "timer.h"

/*********************************************************************************************
* 名称：timer_init()
* 功能：定时器初始化函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void timer_init(void)
{
  TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;                //初始化基本定时器结构体        
  NVIC_InitTypeDef NVIC_InitStructure;                          //初始化中断向量配置结构体
 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);          //初始化定时器时钟
      
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);               //初始化中断优先级分组为分组0	
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;               //配置中断线为TIM7_IRQ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //相应优先级为0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;            //抢占优先级为3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //使能中断
  NVIC_Init(&NVIC_InitStructure);                               //初始化中断配置参数
  //配置中断触发时长为10MS
  TIM_BaseInitStructure.TIM_Period = 1000 - 1;                  //配置计数周期为1000
  TIM_BaseInitStructure.TIM_Prescaler = 840 - 1;                //配置预分频系数为840

  TIM_TimeBaseInit(TIM7, &TIM_BaseInitStructure);               //初始化基本定时器TIM7
  
  TIM_ClearFlag(TIM7, TIM_FLAG_Update);                         //清空TIM7溢出标志位
  TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);                      //使能TIM7中断
  
  TIM_Cmd(TIM7, ENABLE);                                        //使能TIM7
}

/*********************************************************************************************
* 名称：TIM7_IRQHandler()
* 功能：定时器TIM7中断服务函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
extern uint8_t rgb_twinkle;                                     //RGB灯闪烁执行标志位
extern uint8_t get_lumination;                                  //光强采集执行标志位
uint8_t count_rgb = 0;                                          //RGB延时计数系数
uint8_t count_get = 0;                                          //光强采集执行延时计数系数

void TIM7_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET){            //如果中断标志位置位
    //rgb灯闪烁执行标志位控制
    if((count_rgb % 20) == 19){                                 //RGB灯计时是否满100MS
      rgb_twinkle = 1;                                          //标志位置位
      count_rgb = 0;                                            //计数清零
    }else count_rgb ++;                                         //计数加一
    //光强采集执行标志位控制
    if((count_get % 100) == 99){                                //光强采集延时是否为1S                                        
      count_get = 0;                                            //计数清零
      get_lumination = 1;                                       //光强采集执行标志位置位
    }else count_get ++;                                         //计数加一
    
    TIM_ClearITPendingBit(TIM7 , TIM_IT_Update);                //清空定时器TIM7标志位
  }
}
