/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "MP-4.h"

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
  
  combustiblegas_init();                                        //������⴫������ʼ��
  lcd_init(COMBUSTIBLEGAS);                                     //LCD��ʼ��
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
  unsigned int combustiblegas = 0;                           	  //�洢��ȼ����״̬����
  char temp[100]={0};
  hardware_init();
  while(1){
    combustiblegas = get_combustiblegas_data();                 //��ȡ��ȼ����״̬
    printf("combustiblegas:%d\r\n",combustiblegas);             //���ڴ�ӡ��ʾ��Ϣ
    sprintf(temp,"��ȼ����Ũ��:%5d   ",combustiblegas);         //��ӿ�ȼ����״̬�����ַ��������ڻ���
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);    
    
    led_app(1000);
    delay_ms(1000);
  }
}  