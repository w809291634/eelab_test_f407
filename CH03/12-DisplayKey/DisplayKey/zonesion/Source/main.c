/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "vk16k33.h"

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
  
  vk16k33_init();                                               //��ʼ��vk16k33
  lcd_init(DISPLAYKEY);                                         //LCD��ʼ��
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
  s32 num=0;
  u32 last_num=0;
  char buf[100]={0};
  hardware_init();
  while(1){
    u8 key=vk16k33_get_keyval();                                //��ȡ��ť��ֵ
    switch(key){                                                //��ͬ����������ͬЧ��
      case IICKEY_UP:num++;break;
      case IICKEY_LEFT:num=num+100;break;
      case IICKEY_DOWN:num--;break;
      case IICKEY_RIGHT:num=num-100;break;
      case IICKEY_CENTER:{                                      //�м䰴����������ܵ���ʾ
        if(onoff)display_off();
        else display_on();
      }break;
    }
    if(num>9999)num=9999;                                       //��ֵ��Χ����
    if(num<=0)num=0;
    
    if(last_num!=num || key){                                   //������ʾ
      if(last_num!=num)
        segment_display(num);                                   //��ֵ�仯������ʾ�������仯������
      last_num=num;
      //LCD��ʾ��Ϣ
      snprintf(buf,100,"�������ֵ:%04d     \n������ֵ:%d  \r\n",num,key);
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,buf,LCD_WIDTH,BLACK,WHITE);
      printf(buf);                                              //���ڴ�ӡ��ʾ��Ϣ
    }

    led_app(30);
    delay_ms(30);
  }
}  