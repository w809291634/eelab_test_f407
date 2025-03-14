/*********************************************************************************************
* 文件：Relay.c
* 作者：Lixm 2017.10.17
* 说明：继电器驱动代码
* 修改：
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "Relay.h"

/*********************************************************************************************
* 名称：relay_init()
* 功能：继电器传感器初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void relay_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;                          //定义一个GPIO_InitTypeDef类型的结构体
  RCC_AHB1PeriphClockCmd(RELAY_RCC, ENABLE);                    //开启继电器相关的GPIO外设时钟
  
  GPIO_InitStructure.GPIO_Pin = RELAY1_PIN | RELAY2_PIN;        //选择要控制的GPIO引脚
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //设置引脚的输出类型为推挽
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //设置引脚模式为输出模式
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //设置引脚为下拉模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //设置引脚速率为2MHz
  
  GPIO_Init(RELAY_PORT, &GPIO_InitStructure);                   //初始化GPIO配置
  GPIO_ResetBits(RELAY_PORT,GPIO_Pin_2 | GPIO_Pin_3);
}

/*********************************************************************************************
* 名称：void relay_control(unsigned char cmd)
* 功能：继电器控制驱动
* 参数：控制命令
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void relay_control(unsigned char cmd)
{ 
  if(cmd & 0x01)
    GPIO_SetBits(RELAY_PORT,RELAY1_PIN);
  else
    GPIO_ResetBits(RELAY_PORT,RELAY1_PIN);
  if(cmd & 0x02)
    GPIO_SetBits(RELAY_PORT,RELAY2_PIN);
  else
    GPIO_ResetBits(RELAY_PORT,RELAY2_PIN);
}