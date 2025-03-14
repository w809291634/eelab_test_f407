/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "key.h"
#include "rgb.h"
#include "led.h"

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
  key_init();                                                   //������ʼ��
  rgb_init();                                                   //RGB�Ƴ�ʼ��
  led_init();                                                   //LED�Ƴ�ʼ��
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
  char status = 0;                                              //����һ����ʾ״̬�ı���
  hardware_init();
  while(1){
    u8 key=KEY_Scan(0);                                         //������ֵɨ��
    if(key){
      status++;                                                 //״̬������һ
      if (status>3)                                             //״̬�������Ϊ3
        status=0;                                               //״̬��������
    }

    switch(status){    
      case 0: 
        turn_off(LEDR);                                          //�ر�LEDR
        turn_off(LEDG);                                          //�ر�LEDG
        turn_off(LEDB);                                          //�ر�LEDB
        led_turn_off(LED1_NUM);                                  //�ر�LED1
        led_turn_off(LED2_NUM);                                  //�ر�LED2
        led_turn_off(LED3_NUM);                                  //�ر�LED3
        break;
      case 1:turn_on(LEDR);led_turn_on(LED1_NUM);break;         //����LEDR��LED1
      case 2:turn_on(LEDG);led_turn_on(LED2_NUM);break;         //����LEDG��LED2
      case 3:turn_on(LEDB);led_turn_on(LED3_NUM);break;         //����LEDB��LED3
      default:status=0;break;                                   //status����
    }
  }
}  
