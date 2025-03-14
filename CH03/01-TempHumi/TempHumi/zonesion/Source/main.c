/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "htu21d.h"

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
  
  htu21d_init();                                                //��������ʼ��
  lcd_init(TEMPHUMI);                                           //LCD��ʼ��
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
  int Original_Temp = 0;
  int Original_Humi = 0;
  float Temp = 0.0;
  float Humi = 0.0;
  char temp[100]={0};
  hardware_init();
  while(1){
    Original_Temp=htu21d_read_temp();                           //��ȡ��ǰ�¶�ԭʼ����
    Original_Humi=htu21d_read_humi();                           //��ȡ��ǰʪ��ԭʼ����
    Temp = (float)Original_Temp/1000;
    Humi = (float)Original_Humi/1000;
    printf("ԭʼ�¶�:%.2f��\r\n",Temp);                          //���ڴ�ӡ�¶�ԭʼ����
    printf("ԭʼʪ��:%.2f\r\n",Humi);                            //���ڴ�ӡʪ��ԭʼ����
    sprintf(temp,"�¶�:%.2f��   \nʪ��:%.2f%%rh  ",Temp,Humi);   //���¶�ʪ��ԭʼ����ת��Ϊ�ַ����洢��temp������
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);    
    
    led_app(1000);
    delay_ms(1000);
  }
}  
