/*********************************************************************************************
* 文件: led.c
* 作者：liutong 2015.8.19
* 说明：RGB_R->PG8,
        RGB_G->PG7,
        RGB_B->PG6
* 功能：RGB驱动程序
* 修改：
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "rgb.h"

/*********************************************************************************************
* 指定本文件的引脚配置参数
*********************************************************************************************/
#define RGB_R_RCC       RCC_AHB1Periph_GPIOG
#define RGB_G_RCC       RCC_AHB1Periph_GPIOG
#define RGB_B_RCC       RCC_AHB1Periph_GPIOG

#define RGB_R_GPIO      GPIOG
#define RGB_G_GPIO      GPIOG
#define RGB_B_GPIO      GPIOG

#define RGB_R_PIN       GPIO_Pin_8
#define RGB_G_PIN       GPIO_Pin_7
#define RGB_B_PIN       GPIO_Pin_6

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
  
  RCC_AHB1PeriphClockCmd(RGB_R_RCC | RGB_G_RCC |
                         RGB_B_RCC , ENABLE);                   //使能GPIO时钟
  
  GPIO_InitStructure.GPIO_Pin = RGB_R_PIN;                      //选中引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //输出速度2MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //无上下拉
  GPIO_Init(RGB_R_GPIO, &GPIO_InitStructure);                   //根据上述参数配置
  
  GPIO_InitStructure.GPIO_Pin = RGB_G_PIN ;                     //选中引脚
  GPIO_Init(RGB_G_GPIO, &GPIO_InitStructure);                   //根据上述参数配置
  
  GPIO_InitStructure.GPIO_Pin = RGB_B_PIN ;                     //选中引脚
  GPIO_Init(RGB_B_GPIO, &GPIO_InitStructure);                   //根据上述参数配置
  
  GPIO_SetBits(RGB_R_GPIO, RGB_R_PIN );
  GPIO_SetBits(RGB_G_GPIO, RGB_G_PIN );
  GPIO_SetBits(RGB_B_GPIO, RGB_B_PIN );
}

/*********************************************************************************************
* 名称：turn_off
* 功能：置引脚高电平，关闭灯
* 参数：rgb
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void rgb_turn_off(unsigned char rgb){
  if(rgb & LEDR)
    GPIO_SetBits(RGB_R_GPIO, RGB_R_PIN);                        //置引脚高电平,关闭
  if(rgb & LEDG)
    GPIO_SetBits(RGB_G_GPIO, RGB_G_PIN);
  if(rgb & LEDB)
    GPIO_SetBits(RGB_B_GPIO, RGB_B_PIN);
}

/*********************************************************************************************
* 名称：turn_on
* 功能：置引脚低电平，打开灯
* 参数：rgb
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void rgb_turn_on(unsigned char rgb){
  if(rgb & LEDR)
    GPIO_ResetBits(RGB_R_GPIO, RGB_R_PIN);                      //置引脚低电平，打开灯
  if(rgb & LEDG)
    GPIO_ResetBits(RGB_G_GPIO, RGB_G_PIN);
  if(rgb & LEDB)
    GPIO_ResetBits(RGB_B_GPIO, RGB_B_PIN);
}

/*********************************************************************************************
* 名称：get_rgb_status
* 功能：获取灯的状态
* 参数：
* 返回：status，按位分别表示灯的状态
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_rgb_status(void){
  unsigned char status = 0;
  if(GPIO_ReadOutputDataBit(RGB_R_GPIO, RGB_R_PIN))
    status |= LEDR;
  else
    status &= ~LEDR;
  
  if(GPIO_ReadOutputDataBit(RGB_G_GPIO, RGB_G_PIN))
    status |= LEDG; 
  else
    status &= ~LEDG;
  
  if(GPIO_ReadOutputDataBit(RGB_B_GPIO, RGB_B_PIN))
    status |= LEDB;
  else
    status &= ~LEDB;
  return status;                                                //返回status
}

/*********************************************************************************************
* 名称：void rgb_ctrl(uint8_t cfg)
* 功能：RGB控制函数
* 参数：cfg:RGB的控制位，低三位分别控制一盏灯
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void rgb_ctrl(uint8_t cfg)
{
  uint8_t set1, set2, set3;
  set1 = cfg & 0x01;
  set2 = (cfg & 0x02) >> 1;
  set3 = (cfg & 0x04) >> 2;
  
  DR(!set1);
  DG(!set2);
  DB(!set3);
}
