/*********************************************************************************************
* 文件: led.c
* 作者：liutong 2015.8.19
* 说明：
* 修改：
* 注释：
*********************************************************************************************/
#include "rgb.h"
/*********************************************************************************************
* 名称：rgb_init
* 功能：初始化RGB对应的GPIO
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void rgb_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};                    
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC |
                         RCC_AHB1Periph_GPIOD , ENABLE);        //使能GPIO时钟
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                     //选中引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //输出速度2MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //无上下拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);                        //根据上述参数配置
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;                    //选中引脚
  GPIO_Init(GPIOC, &GPIO_InitStructure);                        //根据上述参数配置
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;                    //选中引脚
  GPIO_Init(GPIOD, &GPIO_InitStructure);                        //根据上述参数配置
  
  GPIO_SetBits(GPIOA, GPIO_Pin_4 );
  GPIO_SetBits(GPIOC, GPIO_Pin_5 );
  GPIO_SetBits(GPIOD, GPIO_Pin_6 );
}

/*********************************************************************************************
* 名称：turn_off
* 功能：置引脚高电平，关闭rgb
* 参数：rgb
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void turn_off(unsigned char rgb){
  if(rgb & LEDR)
    GPIO_SetBits(GPIOA, GPIO_Pin_4);                            //置引脚高电平，关闭RGB灯的红灯
  if(rgb & LEDG)
    GPIO_SetBits(GPIOC, GPIO_Pin_5);                            //置引脚高电平，关闭RGB灯的绿灯
  if(rgb & LEDB)
    GPIO_SetBits(GPIOD, GPIO_Pin_6);                            //置引脚高电平，关闭RGB灯的蓝灯
}

/*********************************************************************************************
* 名称：turn_on
* 功能：置引脚低电平，打开rgb
* 参数：rgb
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void turn_on(unsigned char rgb){
  if(rgb & LEDR)
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);                          //置引脚低电平，打开RGB灯的红灯
  if(rgb & LEDG)
    GPIO_ResetBits(GPIOC, GPIO_Pin_5);                          //置引脚低电平，打开RGB灯的绿灯
  if(rgb & LEDB)
    GPIO_ResetBits(GPIOD, GPIO_Pin_6);                          //置引脚低电平，打开RGB灯的蓝灯
}
/*********************************************************************************************
* 名称：get_rgb_status
* 功能：获取LED状态
* 参数：
* 返回：rgb_status，bit4-bit6分别表示RGB灯的状态
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_rgb_status(void){
  unsigned char rgb_status = 0;
  if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4))
    rgb_status |= LEDR;                                         //高电平将rgb_status bit4置1
  else
    rgb_status &= ~LEDR;                                        //低电平将rgb_status bit4置0
  
  if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_5))
    rgb_status |= LEDG;                                         //高电平将rgb_status bit5置1
  else
    rgb_status &= ~LEDG;                                        //低电平将rgb_status bit5置0
  
  if(GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6))
    rgb_status |= LEDB;                                         //高电平将rgb_status bit6置1
  else
    rgb_status &= ~LEDB;                                        //低电平将rgb_status bit6置0
  return rgb_status;                                            //返回rgb_status
}
