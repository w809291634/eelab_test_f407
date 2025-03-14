/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "grating.h"

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
  
  grating_init();                                               //�����դ��������ʼ��
  lcd_init(GRATING);                                            //LCD��ʼ��
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
  unsigned char lastState = 0;                                  // ��դ��һ��״̬
  unsigned char num = 0;
  hardware_init();
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                "δ��⵽�ڵ�  \nD3����  ",LCD_WIDTH,BLACK,WHITE);
  while(1){
    if(get_grating_status() == 1){                              //��⵽�ڵ�
      if(lastState == 0)
      {
        led_setState(LED3_NUM,1);                               //����LED��
        printf("Grating!\r\n");                                 //���ڴ�ӡ��ʾ��Ϣ
        //LCD��ʾ�����Ϣ 
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                      "��⵽�ڵ�  \nD3����  ",LCD_WIDTH,BLACK,WHITE);
        lastState = 1;                                          //��¼״̬
        num = 0;                                                //������0
      }
    }
    else{                                                       //û�м�⵽�ڵ�
      num ++;                                                   //��������
      if(num >= 30)                                             //����ڵ�
      {
        num = 0;                                                //������0
        printf("no Grating!\r\n");                              //���ڴ�ӡ��ʾ��Ϣ
      }
      if(lastState == 1)
      {
        //LCD��ʾ�����Ϣ
        led_setState(LED3_NUM,0);                               //Ϩ��LED��
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                      "δ��⵽�ڵ�  \nD3����  ",LCD_WIDTH,BLACK,WHITE);
        printf("no Grating!\r\n");                              //���ڴ�ӡ��ʾ��Ϣ
        lastState = 0;
      }
    }
    
    led_app(100);
    delay_ms(100);                                              //��ʱ100����
  }
}  