/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "gt911.h"

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
  
  GT911_init();                                                 //��������ʼ��
  lcd_init(TOUCH);                                              //LCD��ʼ��
}

/*********************************************************************************************
* ���ƣ�clean_lcd_info()
* ���ܣ�����LCD����ʾ�Ĵ�������Ϣ
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
static void clean_lcd_info(void)
{
  static char last_num=0xff;                                    //�����Ϣ
  if(last_num!=GT911_pointNum){
    last_num = GT911_pointNum;
    switch(GT911_pointNum){
      /* ����������Ϣ */
      case 0:LCD_Fill(8+32*2,REF_POS+32+5,LCD_WIDTH,32*3+5*2,WHITE);break;
      /* ����ڶ���������Ϣ�ʹ����һ�е���ʾ */
      case 1:
      LCD_Fill(LCD_WIDTH/2+32,REF_POS+32+5,LCD_WIDTH/2,32,WHITE);
      LCD_Fill(8+32*2,REF_POS+32*2+5*2,LCD_WIDTH,32*2+5*1,WHITE);break;
      case 2:
      LCD_Fill(8+32*2,REF_POS+32*2+5*2,LCD_WIDTH,32*2+5*1,WHITE);break;
      /* �����������Ϣ�ʹ���ڶ��е���ʾ */
      case 3:
      LCD_Fill(LCD_WIDTH/2+32,REF_POS+32*2+5*2,LCD_WIDTH/2,32,WHITE);
      LCD_Fill(8+32*2,REF_POS+32*3+5*3,LCD_WIDTH,32*1+5,WHITE);break;
      case 4:
      LCD_Fill(8+32*2,REF_POS+32*3+5*3,LCD_WIDTH,32*1+5,WHITE);break;
      /* �����д��� */
      case 5:break;
    }
  }
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
  u8 count=0;
  u8 ledon=0;
  char buf[100];
  hardware_init();                                              //��ʼ��
  while(1){
    GT911_scan();
    /* ����LCD��ʾ��Ϣ */
    for(int i=0;i<GT911_pointNum;i++){                          //����ÿ�����������Ϣ
      snprintf(buf,100,"ID%d x:%3d y:%3d size:%2d\r\n",
               GT911Point[i].id,GT911Point[i].x,
               GT911Point[i].y,GT911Point[i].size);
      // printf(buf);                                           //��ӡ��Ϣ
      if(i==0)                                                  //��ʾIDΪ0�Ĵ�����
        LCDShowFont32(8+32*2,REF_POS+32+5,buf,LCD_WIDTH,BLACK,WHITE);
      else if(i==1)                                             //��ʾIDΪ1�Ĵ�����
        LCDShowFont32(LCD_WIDTH/2+32,REF_POS+32+5,buf,LCD_WIDTH,BLACK,WHITE);
      else if(i==2)                                             //��ʾIDΪ2�Ĵ�����
        LCDShowFont32(8+32*2,REF_POS+32*2+5*2,buf,LCD_WIDTH,BLACK,WHITE);
      else if(i==3)                                             //��ʾIDΪ3�Ĵ�����
        LCDShowFont32(LCD_WIDTH/2+32,REF_POS+32*2+5*2,buf,LCD_WIDTH,BLACK,WHITE);
      else if(i==4)                                             //��ʾIDΪ4�Ĵ�����
        LCDShowFont32(8+32*2,REF_POS+32*3+5*3,buf,LCD_WIDTH,BLACK,WHITE);
    }
    /* ����֮ǰLCD����ʾ����Ϣ */
    clean_lcd_info();
    
    count++;
    if(count%10==0){                                            //ϵͳ����ָʾ��
      ledon=!ledon;
      if(ledon) led_setState(LED4_NUM,1);
      else led_setState(LED4_NUM,0);
    }
    delay_ms(10);                                               //��ʱ10����
  }
}  