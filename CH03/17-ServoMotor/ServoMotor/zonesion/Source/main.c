/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "ServoMotor.h"

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
  
  ServoMotor_init(20000,84-1);                                  //�ŷ������ʼ��,84M/84=1mhz,20msPWM��
  lcd_init(SERVOMOTOR);                                         //LCD��ʼ��
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
  s16 angle=0;                                                  //�洢��ǰ���ƽǶ�
  s16 last_angle=-1;                                            //�洢��һ�ο��ƽǶ�
  char temp[100]={0};                                           //����ת���洢
  hardware_init();                                              //Ӳ����ʼ��
  while(1){
    u8 key =  KEY_Scan(0);                                      //������ֵɨ��
    if(key == K1_PRES) angle = angle+30;
    else if(key == K2_PRES) angle = angle-30;                   //ͨ���������ƽǶ�
    if(angle > 180) angle=180;                                  //��Χ����
    if(angle < 0) angle=0;
    if(angle!=last_angle){
      last_angle = angle;
      Servo_control(angle);                                     //���ƶ����һ���Ƕ�
      // ������ʾ
      sprintf(temp,"����Ƕ�:%d��   \r\n",angle);                //������ʾ
      printf(temp);
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);
    }

    led_app(10);
    delay_ms(10);
  }
}  