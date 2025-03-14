/*********************************************************************************************
* 文件：key.c
* 作者：Liyw 2017.5.30
* 说明：
* 功能：按键驱动代码
* 修改：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "key.h"
#include "delay.h"

/*********************************************************************************************
* 名称：key_init
* 功能：按键初始化
* 参数：无
* 返回：无
* 修改：无
* 注释：
*********************************************************************************************/
void key_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure ={0};                     //定义一个GPIO_InitTypeKef类型的结构体
  RCC_AHB1PeriphClockCmd(KEY_CLK, ENABLE);                      //开启KEY相关的GPIO外设时钟
  
  GPIO_InitStructure.GPIO_Pin   = K1_PIN | K2_PIN | 
                                  K3_PIN | K4_PIN;              //选择要控制的GPIO引脚
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //设置引脚的输出类型为推挽
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;                 //设置引脚模式为输入模式
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                 //设置引脚为上拉模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //设置引脚速率为2MHz
  
  GPIO_Init(KEY_PORT, &GPIO_InitStructure);                     //初始化GPIO配置
}

/*********************************************************************************************
* 名称：get_key_status
* 功能：获取按键的引脚状态
* 参数：无
* 返回：key_status
* 修改：
*********************************************************************************************/
char get_key_status(void)
{
  char key_status = 0;
  if(GPIO_ReadInputDataBit(KEY_PORT,K1_PIN) == 0)               //判断K1_PIN引脚电平状态
    key_status |= K1_PRES;                                      //低电平key_status bit0位置1
  if(GPIO_ReadInputDataBit(KEY_PORT,K2_PIN) == 0)               //判断K2_PIN引脚电平状态
    key_status |= K2_PRES;                                      //低电平key_status bit1位置1
  if(GPIO_ReadInputDataBit(KEY_PORT,K3_PIN) == 0)               //判断K3_PIN引脚电平状态
    key_status |= K3_PRES;                                      //低电平key_status bit2位置1
  if(GPIO_ReadInputDataBit(KEY_PORT,K4_PIN) == 0)               //判断K4_PIN引脚电平状态
    key_status |= K4_PRES;                                      //低电平key_status bit3位置1
  return key_status;
}

/*********************************************************************************************
* 名称：key_status
* 功能：按键管脚初始化
* 参数：num 获取具体的按键编号
* 返回：该按键的状态
* 修改：无
* 注释：
*********************************************************************************************/
signed char key_status(char num)
{
  if(num == K1)
    return GPIO_ReadInputDataBit(KEY_PORT,K1_PIN);              //返回K1的状态

  if(num == K2)
    return GPIO_ReadInputDataBit(KEY_PORT,K2_PIN);              //返回K2的状态
  
  if(num == K3)
    return GPIO_ReadInputDataBit(KEY_PORT,K3_PIN);              //返回K3的状态
  
  if(num == K4)
    return GPIO_ReadInputDataBit(KEY_PORT,K4_PIN);              //返回K4的状态
  
  return -1;
}

/*********************************************************************************************
* 名称：u8 KEY_Scan(u8 mode)
* 功能：按键处理函数
* 参数：mode: 0,不支持连续按; 1,支持连续按;
* 返回：返回按键值
* 修改：
* 注释：注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
*********************************************************************************************/
u8 KEY_Scan(u8 mode)
{
  static u8 key_up=1;                                           //按键按松开标志
  if(mode)key_up=1;                                             //支持连按
  if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
  {
    delay_ms(10);                                               //去抖动 
    key_up=0;
    if(KEY1==0)return K1_PRES;
    else if(KEY2==0)return K2_PRES;
    else if(KEY3==0)return K3_PRES;
    else if(KEY4==0)return K4_PRES;
  }else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1;
  return 0;                                                     // 无按键按下
}
