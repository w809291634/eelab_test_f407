/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "vibration.h"

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
  
  vibration_init();                                             //�񶯴�������ʼ��
  lcd_init(VIBRATION);                                          //LCD��ʼ��
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
  unsigned char vibration_status = 0;                          //�洢��״̬����
  unsigned int count = 0;                                      //���񶯼���
  hardware_init();
  while(1){
    if(get_vibration_status() == 1){                            //��⵽��
      if(vibration_status == 0){                                //�񶯴�����״̬�����ı�                       
        led_setState(LED3_NUM,1);                               //����LED��
        printf("Vibration!\r\n");                               //���ڴ�ӡ��ʾ��Ϣ
        vibration_status = 1;                                   //�����񶯴�����״̬
        //LCD��ʾ��Ϣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"��⵽��   \nD3����",LCD_WIDTH,BLACK,WHITE); 
      }
      count = 0;                                                //������0
    }
    else{                                                       //û�м�⵽��
      count ++;                                                 //��������
      if(count > 20000)                                         //�ж��Ƿ�ֹͣ��
      {
        count =  0;                                             //������0
        led_setState(LED3_NUM,0);                               //Ϩ��LED��
        printf("no Vibration!\r\n");                            //���ڴ�ӡ��ʾ��Ϣ
        vibration_status = 0;                                   //�����񶯴�����״̬
        //LCD��ʾ��Ϣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"δ��⵽��   \nD3����",LCD_WIDTH,BLACK,WHITE);
      }
    }
    
    led_app_ex(100);
    delay_us(100);                                              //��ʱ100΢��
  }
}  