#include "key.h"
#include "delay.h"

// 初始化
void key_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure ={0};                      //定义一个GPIO_InitTypeKef类型的结构体
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
* 功能：按键管脚初始化
* 参数：无
* 返回：key_status
* 修改：
*********************************************************************************************/
char get_key_status(void)
{
  char key_status = 0;
  if(GPIO_ReadInputDataBit(KEY_PORT,K1_PIN) == 0)               //判断PB12引脚电平状态
    key_status |= K1_PRES;                                      //低电平key_status bit0位置1
  if(GPIO_ReadInputDataBit(KEY_PORT,K2_PIN) == 0)              //判断PB13引脚电平状态
    key_status |= K2_PRES;                                      //低电平key_status bit1位置1
  if(GPIO_ReadInputDataBit(KEY_PORT,K3_PIN) == 0)               //判断PB14引脚电平状态
    key_status |= K3_PRES;                                      //低电平key_status bit2位置1
  if(GPIO_ReadInputDataBit(KEY_PORT,K4_PIN) == 0)               //判断PB15引脚电平状态
    key_status |= K4_PRES;                                      //低电平key_status bit3位置1
  return key_status;
}

//按键处理函数
//返回按键值
//mode: 0,不支持连续按; 1,支持连续按;
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_count(1000);//去抖动 
		key_up=0;
		if(KEY1==0)return K1_PRES;
		else if(KEY2==0)return K2_PRES;
		else if(KEY3==0)return K3_PRES;
		else if(KEY4==0)return K4_PRES;
	}else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	    
 	return 0;// 无按键按下
}
