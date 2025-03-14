/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "FAN.h"

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
  
  fan_pwm_init(100000-1,84-1);                                  //84M/84=1Mhz�ļ���Ƶ��,1usһ�μ���������100ms������
  lcd_init(FAN);                                                //LCD��ʼ��
}

#define ARRAY(X)  (sizeof(X)/sizeof(X[0]))
static char fan_pwm[]={0,50,100};                               //�о�һЩ��Ҫ��PWMֵ
/*********************************************************************************************
* ���ƣ�main()
* ���ܣ�������
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
int main(void)
{
  char count=0; 
  char temp[100]={0};
  hardware_init();
  
  sprintf(temp,"����PWM:%3d%%   \r\n",fan_pwm[count]);          //��ʾ��ʼֵ
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);
  printf(temp);
  while(1){
    u8 key=KEY_Scan(0);                                         //����а������������Ƶ��PWM���
    if(key){                                 
      count++;                                                  //���·��ȵ�PWM�б��е�����
      if(count>=ARRAY(fan_pwm))count=0;                         //������Χ����λ����
      //LCD��ʾ��ʾ��Ϣ
      sprintf(temp,"����PWM:%3d%%   \r\n",fan_pwm[count]);      //������Ϣ������ʾ
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);    
      fan_pwm_control(fan_pwm[count]);                          //����PWM��ռ�ձ�
      printf(temp);                                             //���ڴ�ӡ��ʾ��Ϣ
    } 
    led_app(10);
    delay_ms(10);
  }
}  