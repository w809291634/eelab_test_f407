/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "vibration.h"

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
  usart_init(115200);                                           //串口初始化
  delay_init(168);                                              //延时初始化
  led_init();                                                   //LED初始化
  key_init();                                                   //按键初始化
  
  vibration_init();                                             //振动传感器初始化
  lcd_init(VIBRATION);                                          //LCD初始化
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
  unsigned char vibration_status = 0;                          //存储振动状态变量
  unsigned int count = 0;                                      //无振动计数
  hardware_init();
  while(1){
    if(get_vibration_status() == 1){                            //检测到振动
      if(vibration_status == 0){                                //振动传感器状态发生改变                       
        led_setState(LED3_NUM,1);                               //点亮LED灯
        printf("Vibration!\r\n");                               //串口打印提示信息
        vibration_status = 1;                                   //更新振动传感器状态
        //LCD显示信息
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"检测到振动   \nD3：开",LCD_WIDTH,BLACK,WHITE); 
      }
      count = 0;                                                //计数清0
    }
    else{                                                       //没有检测到振动
      count ++;                                                 //计数自增
      if(count > 20000)                                         //判断是否停止振动
      {
        count =  0;                                             //计数清0
        led_setState(LED3_NUM,0);                               //熄灭LED灯
        printf("no Vibration!\r\n");                            //串口打印提示信息
        vibration_status = 0;                                   //更新振动传感器状态
        //LCD显示信息
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"未检测到振动   \nD3：关",LCD_WIDTH,BLACK,WHITE);
      }
    }
    
    led_app_ex(100);
    delay_us(100);                                              //延时100微秒
  }
}  