/*********************************************************************************************
* 文件：ServoMotor.c
* 作者：Lixm 2017.10.17
* 说明：继电器驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "ServoMotor.h"
#include <math.h> 

#define TIM_RCC           RCC_APB1Periph_TIM2
#define TIMX              TIM2

#define TIM_CHX_AF        GPIO_AF_TIM2
#define TIM_CHX_RCC       RCC_AHB1Periph_GPIOA
#define TIM_CHX_GPIO      GPIOA
#define TIM_CHX_PIN       GPIO_Pin_5
#define TIM_CHX_PINSOURCE GPIO_PinSource5

/*********************************************************************************************
* 名称：ServoMotor_init()
* 功能：舵机PWM初始化  PA5/TIM2_CH1
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void ServoMotor_init(u32 arr,u32 psc)
{
  //此部分需手动修改IO口设置
  GPIO_InitTypeDef GPIO_InitStructure={0};
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
  TIM_OCInitTypeDef  TIM_OCInitStructure={0};

  RCC_AHB1PeriphClockCmd(TIM_CHX_RCC, ENABLE);
  RCC_APB1PeriphClockCmd(TIM_RCC,ENABLE);  	                    //TIM时钟使能    
  
  GPIO_PinAFConfig(TIM_CHX_GPIO,TIM_CHX_PINSOURCE,TIM_CHX_AF); 

  GPIO_InitStructure.GPIO_Pin = TIM_CHX_PIN;                     //PIN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	          //速度100MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上拉
  GPIO_Init(TIM_CHX_GPIO,&GPIO_InitStructure);                  //初始化
  
  TIM_TimeBaseStructure.TIM_Prescaler=psc;                      //定时器分频,此处定时器挂载到APB1，为84MHZ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;     //向上计数模式
  TIM_TimeBaseStructure.TIM_Period=arr;                         //自动重装载值
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
  TIM_TimeBaseInit(TIMX,&TIM_TimeBaseStructure);                //初始化定时器

  //初始化TIM4 Channel1 PWM模式	 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //选择定时器模式:TIM脉冲宽度调制模式
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //输出极性
  TIM_OC1Init(TIMX, &TIM_OCInitStructure);                      //初始化通道1

  TIM_OC1PreloadConfig(TIMX, TIM_OCPreload_Enable);             //使能TIM在CCR1上的预装载寄存器

  TIM_ARRPreloadConfig(TIMX,ENABLE);                            //ARPE使能 
  TIM_Cmd(TIMX, ENABLE);                                        //使能TIM
}

/*********************************************************************************************
* 名称：ServoPWM_control
* 功能：伺服PWM驱动控制
        20ms -- 20000 arr值 -- 20000 us -- 1us对应1个arr值
        0.5ms -- 500 arr值   2.5%
        2.5ms -- 2500 arr值  12.5%
        周期为 20ms  0.5ms-2.5ms 高电位占空 控制舵机0-180度
* 参数：angle 角度 0-180
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
#define OFFSET  1.0f                                            //舵机角度修正值
void Servo_control(u8 angle)
{ 
  u32 pwm = (u32)round((2000* angle)/180.0f);
  TIM_SetCompare1(TIMX,(500+pwm)/OFFSET);                       //修改比较值，修改占空比
}
