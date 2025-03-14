/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "lock.h"

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
  
  lock_init();                                                  //初始化门锁
  lcd_init(LOCK);                                               //LCD初始化
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
  u32 count=0;
  char open=0;
  hardware_init();                                              //硬件初始化
  while(1){
    u8 key = KEY_Scan(0);                                       //键值扫描
    if(key==K1_PRES){
      open = !open;                                             //标志位取反
      if(open){
        LOCK_OPEN;                                              //打开门锁
        printf("门锁打开\r\n");
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"门锁打开",LCD_WIDTH,BLACK,WHITE); 
      }else{
        LOCK_CLOSE;                                             //关闭门锁
        printf("门锁关闭\r\n");
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"门锁关闭",LCD_WIDTH,BLACK,WHITE); 
      }
    }else if (key==0 && open) {
        count++;
        if(count>300){
            count=0;
            open=0;
            LOCK_CLOSE;                                             //关闭门锁
            printf("门锁关闭\r\n");
            LCDShowFont32(8+32*2,REF_POS+32+SPACING,"门锁关闭",LCD_WIDTH,BLACK,WHITE);
        }
    }
    
    led_app(10);
    delay_ms(10);
  }
}  
