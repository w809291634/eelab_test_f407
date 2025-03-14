/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "FAN.h"

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
  
  fan_pwm_init(100000-1,84-1);                                  //84M/84=1Mhz的计数频率,1us一次计数，这里100ms的周期
  lcd_init(FAN);                                                //LCD初始化
}

#define ARRAY(X)  (sizeof(X)/sizeof(X[0]))
static char fan_pwm[]={0,50,100};                               //列举一些需要的PWM值
/*********************************************************************************************
* 名称：main()
* 功能：主函数
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
int main(void)
{
  char count=0; 
  char temp[100]={0};
  hardware_init();
  
  sprintf(temp,"风扇PWM:%3d%%   \r\n",fan_pwm[count]);          //显示初始值
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);
  printf(temp);
  while(1){
    u8 key=KEY_Scan(0);                                         //如果有按键触发，控制电机PWM输出
    if(key){                                 
      count++;                                                  //更新风扇的PWM列表中的索引
      if(count>=ARRAY(fan_pwm))count=0;                         //超出范围，复位索引
      //LCD显示提示信息
      sprintf(temp,"风扇PWM:%3d%%   \r\n",fan_pwm[count]);      //汇总信息用于显示
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);    
      fan_pwm_control(fan_pwm[count]);                          //控制PWM的占空比
      printf(temp);                                             //串口打印提示信息
    } 
    led_app(10);
    delay_ms(10);
  }
}  