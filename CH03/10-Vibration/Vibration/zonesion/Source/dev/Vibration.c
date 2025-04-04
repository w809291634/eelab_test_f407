/*********************************************************************************************
* 文件：Vibration.c
* 作者：Lixm 2017.10.17
* 说明：震动驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "Vibration.h"

#define VIBRATION_RCC     RCC_AHB1Periph_GPIOA
#define VIBRATION_GPIO    GPIOA
#define VIBRATION_PIN     GPIO_Pin_0

/*********************************************************************************************
* 名称：vibration_init()
* 功能：震动传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void vibration_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure={0};                      //定义一个GPIO_InitTypeDef类型的结构体
  RCC_AHB1PeriphClockCmd(VIBRATION_RCC, ENABLE);                //开启震动相关的GPIO外设时钟
  
  GPIO_InitStructure.GPIO_Pin = VIBRATION_PIN;                  //选择要控制的GPIO引脚
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //设置引脚的输出类型为推挽
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //设置引脚模式为输入模式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //设置引脚为上拉模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //设置引脚速率为2MHz
  
  GPIO_Init(VIBRATION_GPIO, &GPIO_InitStructure);               //初始化GPIO配置
}

/*********************************************************************************************
* 名称：unsigned char get_vibration_status(void)
* 功能：获取震动传感器状态
* 参数：无
* 返回：1 表示有震动 
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_vibration_status(void)                     
{  
  if(GPIO_ReadInputDataBit(VIBRATION_GPIO,VIBRATION_PIN))       //获取震动传感器的状态
    return 0;
  else
    return 1;
}