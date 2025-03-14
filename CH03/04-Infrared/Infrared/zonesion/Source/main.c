/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "Infrared.h"

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
  
  infrared_init();                                              //������⴫������ʼ��
  lcd_init(INFRARED);                                           //LCD��ʼ��
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
  unsigned char last_status = 0;                                //�洢�������״̬����
  unsigned count1 = 0, count2 = 0;                              //count1��ϵͳʱ�������count2����������ʱ�����
  char temp[100]={0};
  hardware_init();
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,"������  \nD3����  ",LCD_WIDTH,BLACK,WHITE);
  while(1){
    if(get_infrared_status() == 1) {
      if(last_status == 0) {
        led_setState(LED3_NUM,1);                               //����LED��                                
        printf("human!\r\n");                                   //���ڴ�ӡ��ʾ��Ϣ
        //LCD��ʾ������⴫����������Ϣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"������  \nD3����  ",LCD_WIDTH,BLACK,WHITE);
      }
      count2 = 0;
      last_status = 1;
    } else {
      if(last_status == 1) {
        count2 = 0;
        last_status = 2;
      } else if(last_status == 2 && count2 >= 50) {             //����5��û��⵽����Ϊ�뿪
        led_setState(LED3_NUM,0);                               //Ϩ��LED��
        printf("no human!\r\n");                                //���ڴ�ӡ��ʾ��Ϣ
        //LCD��ʾ������⴫����������Ϣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"������  \nD3����  ",LCD_WIDTH,BLACK,WHITE);
        last_status = 0;
      }
      count2++;
    }
    if(count1++ >= 10) {
      count1 = 0;
      if(last_status == 0) {
        printf("no human!\r\n");
      } else {
        printf("human!\r\n");
      }
    }
    led_app(100);
    delay_ms(100);
  }
}  