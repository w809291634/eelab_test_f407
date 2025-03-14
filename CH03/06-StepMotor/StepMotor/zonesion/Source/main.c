/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "stepmotor.h"

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
  
  stepmotor_init();                                             //���������ʼ��
  lcd_init(STEPMOTOR);                                          //LCD��ʼ��
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
  unsigned char stepmotor_flag = 0;                             //���沽�����״̬����
  static unsigned char last_stepmotor_flag;                     //������һ�α���ֵ
  hardware_init();
  while(1){
    u8 key=KEY_Scan(0);
    if(key==K1_PRES){
      if((stepmotor_flag & 0x01) == 0){                         //�жϲ��������ת
        stepmotor_flag = 0x03;                                  //������ת
        //LCD��ʾ��ʾ��Ϣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"���������ת",LCD_WIDTH,BLACK,WHITE);   
        printf("stepmotor forward!\r\n");                       //���ڴ�ӡ��ʾ��Ϣ
      }
      else{
        stepmotor_flag = 0;                                 		//ֹͣ
        //LCD��ʾ��ʾ��Ϣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"�������ֹͣ",LCD_WIDTH,BLACK,WHITE);  
        printf("stepmotor stop!\r\n");                          //���ڴ�ӡ��ʾ��Ϣ
      }
    }
    if(key==K2_PRES){
      if((stepmotor_flag & 0x01) == 0){                         //�жϲ��������ת
        stepmotor_flag = 0x01;                                  //���·�ת
        //LCD��ʾ��ʾ��Ϣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"���������ת",LCD_WIDTH,BLACK,WHITE);  
        printf("stepmotor reverse!\r\n");                       //���ڴ�ӡ��ʾ��Ϣ
      }
      else{
        stepmotor_flag = 0;                                 		//ֹͣ
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"�������ֹͣ",LCD_WIDTH,BLACK,WHITE);  
        printf("stepmotor stop!\r\n");                          //���ڴ�ӡ��ʾ��Ϣ
      } 
    }
    if(last_stepmotor_flag!=stepmotor_flag){
      last_stepmotor_flag=stepmotor_flag;
      stepmotor_control(stepmotor_flag);                        //����״̬�������Ʋ������
    }
    
    led_app(10);
    delay_ms(10);
  }
}  