/*********************************************************************************************
* 文件：stepmotor.c
* 作者：Lixm 2017.10.17
* 说明：步进电机驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "stepmotor.h"

/*********************************************************************************************
* 名称：stepmotor_init()
* 功能：步进电机传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void stepmotor_init(void)
{
  // PC1 DIR
  // PB8 连接STEP引脚  TIM4 Channel3 
  // PB9 EN
  GPIO_InitTypeDef GPIO_InitStructure={0};                      //定义一个GPIO_InitTypeDef类型的结构体
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};           //定时器配置
  TIM_OCInitTypeDef  TIM_OCInitStructure={0};
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);         //开启步进电机相关的GPIO外设时钟
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);         //开启步进电机相关的GPIO外设时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM4);         //复用为定时器
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                     //选择要控制的GPIO引脚
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //设置引脚的输出类型为推挽
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //设置引脚模式为输出模式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //设置引脚为下拉模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //设置引脚速率为2MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);                        //初始化GPIO配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  

  GPIO_SetBits(GPIOB,GPIO_Pin_9);

  // 初始化PWM引脚
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                     //PB8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	          //速度100MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上拉
  GPIO_Init(GPIOB,&GPIO_InitStructure);                         //初始化PB10

  TIM_TimeBaseStructure.TIM_Period = 999;                       //计数器重装值
  TIM_TimeBaseStructure.TIM_Prescaler = 83;                     //预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //计数模式
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);               //按上述配置初始化TIM4
  //初始化TIM4 Channel3 PWM模式

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //选择定时器模式:TIM脉冲宽度调制模式2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //输出极性:TIM输出比较极性低
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);                      //根据T指定的参数初始化外设TIM4 3OC

  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);             //使能TIM4在CCR3上的预装载寄存器

  TIM_ARRPreloadConfig(TIM4,ENABLE);                            //ARPE使能
  TIM_Cmd(TIM4, ENABLE);                                        //使能TIM4
  TIM_SetCompare3(TIM4,0);
}

/*********************************************************************************************
* 名称：void stepmotor_control(unsigned char cmd)
* 功能：步进电机控制驱动
* 参数：控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void stepmotor_control(unsigned char cmd)
{ 
  if(cmd & 0x01){
    if(cmd & 0x02){
      GPIO_SetBits(GPIOC,GPIO_Pin_1);
      TIM_SetCompare3(TIM4,500);
    }else{
      GPIO_ResetBits(GPIOC,GPIO_Pin_1);
      TIM_SetCompare3(TIM4,500);
    }
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
  }
  else GPIO_SetBits(GPIOB,GPIO_Pin_9);
}