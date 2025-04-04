/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "key.h"
#include "rgb.h"
#include "iwdg.h"
#include "led.h"

/*********************************************************************************************
* 名称：hardware_init()
* 功能：硬件初始化
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
static void hardware_init(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //设置系统中断优先级分组2
  key_init();                                                   //按键初始化
  led_init();                                                   //led灯初始化
  rgb_init();                                                   //RGB灯初始化
}

/*********************************************************************************************
* 名称：main()
* 功能：主函数
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
int main(void)
{
  hardware_init();
  delay_count(50000);                                           //延时
  wdg_init(IWDG_Prescaler_32,1000);                             //预分频数32，重载值为1000，溢出时间为1S
  turn_on(LEDR);    
  led_turn_on(LED1_NUM);
  while(1){
    if(get_key_status()==K3_PREESED){                           //判断K3按键是否被按下 
      wdg_feed();                                               //喂狗程序
    }
    delay_count(500);                                           //延时
    turn_on(LEDG);                                              //G灯开
    led_turn_on(LED1_NUM);                                      //led1灯开
    delay_count(500);                                           //延时
    turn_off(LEDG);                                             //G灯关
    led_turn_off(LED1_NUM);                                     //led1灯关
  }
}  
