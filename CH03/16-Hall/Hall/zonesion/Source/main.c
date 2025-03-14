/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "hall.h"

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
  
  hall_init();                                                  //������������ʼ��
  lcd_init(HALL);                                               //LCD��ʼ��
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
  unsigned char hall_status = 0;                                //�洢����״̬����
  hardware_init();
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                "δ��⵽�ų�  \nD3����  ",LCD_WIDTH,BLACK,WHITE);
  while(1){
    if(get_hall_status() == 1){                                 //��⵽�ų�
      if(hall_status == 0){                                     //����������״̬�����ı�
        led_setState(LED3_NUM,1);;                              //����LED��
        printf("hall!\r\n");                                    //���ڴ�ӡ��ʾ��Ϣ
        hall_status = 1;                                        //�����񶯴�����״̬
        //LCD��ʾ��Ϣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                      "��⵽�ų�  \nD3����  ",LCD_WIDTH,BLACK,WHITE);
      }
    }
    else{                                                       //û�м�⵽�ų�
      if(hall_status == 1){                                     //�ر�LED��
        led_setState(LED3_NUM,0);;                              //Ϩ��LED��
        printf("no hall!\r\n");                                 //���ڴ�ӡ��ʾ��Ϣ
        hall_status = 0;                                        //���»���������״̬
        //LCD��ʾ��Ϣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,
                      "δ��⵽�ų�  \nD3����  ",LCD_WIDTH,BLACK,WHITE);
      }
    }
    
    led_app_ex(10);
    delay_us(10);                                               //��ʱ10΢��
  }
}  