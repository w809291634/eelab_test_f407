#include "dcmotor.h"

static vs32 encoder_count;
vu32 dcmotor_dir;
double dcmotor_speed;
#define REDUCTION_RATIO       150                               // 电机减速比
#define PULSE_PERIOD          6                                 // 使用单脉冲计数，编码器端一圈大约6计数
/*********************************************************************************************
* 名称:dcmotor_init()
* 功能:直流电机初始化函数
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void dcmotor_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure={0};                      //定义一个GPIO_InitTypeDef类型的结构体
  RCC_AHB1PeriphClockCmd(DCMOTOR_RCC1, ENABLE);                 //开启相关的GPIO外设时钟
  RCC_AHB1PeriphClockCmd(DCMOTOR_RCC2, ENABLE);
  
  // 初始化电机引脚1
  GPIO_InitStructure.GPIO_Pin = DCMOTOR_PIN1;                   //选择要控制的GPIO引脚
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //设置引脚的输出类型为推挽
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //设置引脚模式为输出模式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //设置引脚上下拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //设置引脚速率
  GPIO_Init(DCMOTOR_GPIO1, &GPIO_InitStructure);                //初始化GPIO配置
  
  // 初始化电机引脚2
  GPIO_InitStructure.GPIO_Pin = DCMOTOR_PIN2;                   //选择要控制的GPIO引脚
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //设置引脚的输出类型为推挽
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //设置引脚模式为输出模式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //设置引脚上下拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //设置引脚速率
  GPIO_Init(DCMOTOR_GPIO2, &GPIO_InitStructure);                //初始化GPIO配置
  
  DCMOTOR_STOP;
  encoder_init(1000,8400-1);                                   //(8400)0.1ms计数周期，0.1*1000=0.1s溢出
}

/*********************************************************************************************
* 名称:encoder_init()
* 功能:直流电机编码器初始化函数 使用PB1获取编码器脉冲计数值,定时器作为一个速度时基
* 参数:arr 定时器 arr寄存器值 psr 预分频值
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void encoder_init(u32 arr,u32 psc)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};
  NVIC_InitTypeDef   NVIC_InitStructure={0};
  EXTI_InitTypeDef   EXTI_InitStructure={0};
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
  
  // 定时器初始化
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);           //TIM时钟使能   
  
  TIM_TimeBaseStructure.TIM_Prescaler=psc;                      //定时器分频,定时器3挂载到APB1，为84MHZ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;     //向上计数模式
  TIM_TimeBaseStructure.TIM_Period=arr;                         //自动重装载值
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);                //初始化定时器
  TIM_ARRPreloadConfig(TIM3,ENABLE);                            //ARPE使能 
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                      //允许定时器更新中断
  TIM_Cmd(TIM3, ENABLE);                                        //使能TIM
  
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;                 //定时器中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;    //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;           //子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
  // GPIO 初始化
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);         //使能GPIO时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                     //对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);                        //初始化GPIO
  
  // EXTI 初始化
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);        //使能SYSCFG时钟
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1); //PB1 连接到中断线1
  
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;           //中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        //下降沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     //使能
  EXTI_Init(&EXTI_InitStructure);                               //按上述参数配置
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;              //外部中断号
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;  //抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;         //子优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);                               //按上述配置初始化
}

/*********************************************************************************************
* 名称:EXTI1_IRQHandler()
* 功能:外部中断1服务程序
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void EXTI1_IRQHandler(void)
{
  static char last_dir;
  if(last_dir!=dcmotor_dir){
    last_dir=dcmotor_dir;
    encoder_count=0;
  }
  if(dcmotor_dir) encoder_count++;
  else encoder_count--;
  EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE0上的中断标志位 
}

/*********************************************************************************************
* 名称:TIM3_IRQHandler()
* 功能:定时器3中断服务函数  
* 参数:无
* 返回:无
* 修改:
* 注释:
*********************************************************************************************/
void TIM3_IRQHandler(void)
{
  double count;
  if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)  //溢出中断
  {
    count = encoder_count*1000/100;             //定时器100ms溢出，这里单位 计数/s
    //count = encoder_count*60*1000/100;        //计数/min
    encoder_count =0;
    dcmotor_speed = count/REDUCTION_RATIO/PULSE_PERIOD;  //电机减速器端实际转速 r/s
  }
  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
