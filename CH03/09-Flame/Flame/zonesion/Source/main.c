/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "Flame.h"

/*********************************************************************************************
* ���ƣ�hardware_init()
* ���ܣ�Ӳ����ʼ��
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
static void hardware_init(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //����ϵͳ�ж����ȼ�����2
  usart_init(115200);                                           //���ڳ�ʼ��
  delay_init(168);                                              //��ʱ��ʼ��
  led_init();                                                   //LED��ʼ��
  key_init();                                                   //������ʼ��
  
  flame_init();                                                 //���洫������ʼ��
  lcd_init(FLAME);                                              //LCD��ʼ��
}

/*********************************************************************************************
* ���ƣ�main()
* ���ܣ�������
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
int main(void)
{
  unsigned char lastState = 0;                                  //�洢����״̬����
  unsigned char count = 0;
  hardware_init();
  if(get_flame_status())
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,"��⵽����  ",LCD_WIDTH,BLACK,WHITE); 
  else
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,"δ��⵽����  ",LCD_WIDTH,BLACK,WHITE); 
  while(1){
    if(get_flame_status()){                                     //��⵽����
      if(lastState == 0)
      {
        lastState = 1;
        printf("fire!\r\n");                                    //���ڴ�ӡ��ʾ��Ϣ
        //LCD��������
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"��⵽����  ",LCD_WIDTH,BLACK,WHITE); 
      }
    }
    else{                                                       //û�м�⵽����
      if(lastState == 1)
      {
        lastState = 0;
        printf("no fire!\r\n");                                 //���ڴ�ӡ��ʾ��Ϣ
        //LCD��������
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"δ��⵽����  ",LCD_WIDTH,BLACK,WHITE); 
      }
    }
    if(count++ >= 10)
    {
      count = 0;
      if(lastState == 0)
        printf("no fire!\r\n");                                 //���ڴ�ӡ��ʾ��Ϣ
      else
        printf("fire!\r\n");                                    //���ڴ�ӡ��ʾ��Ϣ
    }
    if(lastState == 0)                                          //����LED�Ƶ�״̬����LED�Ƶı仯
      led_setState(LED3_NUM,0);                                 //�ر�LED��
    else                                    
      led_setState(LED3_NUM,1);                                 //����LED��

    led_app(100);
    delay_ms(100);
  }
}  