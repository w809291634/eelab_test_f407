/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "lock.h"

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
  
  lock_init();                                                  //��ʼ������
  lcd_init(LOCK);                                               //LCD��ʼ��
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
  u32 count=0;
  char open=0;
  hardware_init();                                              //Ӳ����ʼ��
  while(1){
    u8 key = KEY_Scan(0);                                       //��ֵɨ��
    if(key==K1_PRES){
      open = !open;                                             //��־λȡ��
      if(open){
        LOCK_OPEN;                                              //������
        printf("������\r\n");
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"������",LCD_WIDTH,BLACK,WHITE); 
      }else{
        LOCK_CLOSE;                                             //�ر�����
        printf("�����ر�\r\n");
        LCDShowFont32(8+32*2,REF_POS+32+SPACING,"�����ر�",LCD_WIDTH,BLACK,WHITE); 
      }
    }else if (key==0 && open) {
        count++;
        if(count>300){
            count=0;
            open=0;
            LOCK_CLOSE;                                             //�ر�����
            printf("�����ر�\r\n");
            LCDShowFont32(8+32*2,REF_POS+32+SPACING,"�����ر�",LCD_WIDTH,BLACK,WHITE);
        }
    }
    
    led_app(10);
    delay_ms(10);
  }
}  
