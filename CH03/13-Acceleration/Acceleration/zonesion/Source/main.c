/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "lis3dh.h"

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
  
  lcd_init(ACCELERATION);                                       //LCD��ʼ��
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
  float accx,accy,accz;                                         //����x,y,z,����
  char buff[100];                                               //��ʾ��������
  hardware_init();
  if(lis3dh_init() == 0){                                       //���ᴫ������ʼ��
    printf("lis3dh ok!\r\n");                                   //���ڴ�ӡ
    //LCD��ʾ��ʾ��Ϣ
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,"lis3dh ok!",LCD_WIDTH,BLACK,WHITE);
  }
  else{
    printf("lis3dh error!\r\n");                                //���ڴ�ӡ
    //LCD��ʾ��ʾ��Ϣ
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,"lis3dh error!",LCD_WIDTH,BLACK,WHITE);
  }
  while(1){
    get_lis3dhInfo(&accx,&accy,&accz);                          //�����ᴫ������ȡx,y,z����
    printf("accx:%.1f N/Kg accy:%.1f N/Kg accz:%.1f N/Kg\r\n",accx,accy,accz); //���ڴ�ӡ
    snprintf(buff,100,"x�᣺%.1f N/Kg  y�᣺%.1f N/Kg   \nz�᣺%.1f N/Kg  ",accx,accy,accz); 
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,buff,LCD_WIDTH,BLACK,WHITE);
    
    led_app(1000);
    delay_ms(1000);
  }
}  