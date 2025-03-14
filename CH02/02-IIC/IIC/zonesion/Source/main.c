/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "pcf8563.h"

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
  
  PCF8563_init();
  lcd_init(IIC);                                                //LCD��ʼ��
}

/*********************************************************************************************
* ���ƣ�check_time_vaild()
* ���ܣ����ʱ�����Ч��
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
void check_time_vaild(calendar_t* time){
  if(time->second>59){
    time->second=0;
    time->minute++;
  }
  if(time->minute>59){
    time->minute=0;
    time->hour++;
  }
  if(time->hour>23){
    time->hour=0;
    time->day++;
  }
  if(time->day>31){
    time->day=1;
    time->month++;
  }
  if(time->month>12){
    time->month=1;
    time->year++;
  }
}

/*********************************************************************************************
* ���ƣ�main()
* ���ܣ�������
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
calendar_t test_time={.year=23,
                  .month=11,
                  .day=17,
                  .hour=10,
                  .minute=4,
                  .second=23};                                  //��ʼ������ʱ���������
int main(void)
{
  char count=100;                                               //�����γ�1S��ѭ������
  char state=0;                                                 //״̬���Ƿ�ˢ��
  char init=1;                                                  //����LCD��ʾ�״ε��趨ʱ��
  char temp[100]={0};
  u8 key_value=0;                                               //���水����ֵ
  calendar_t time={0};
  hardware_init();                                              //Ӳ����ʼ��
  while(1){
    key_value=KEY_Scan(0);
    /* ����RTC�趨ʱ�� */
    if(key_value || init){
      init=0;
      switch(key_value){
        case K1_PRES:test_time.minute++;break;                  //����1�����趨ʱ�������1
        case K2_PRES:test_time.hour++;break;                    //����2�����趨ʱ��ʱ����1
        case K3_PRES:test_time.day++;break;                     //����3�����趨ʱ��������1
        case K4_PRES:{                                          //����4����RTC�趨ʱ��
          if(PCF8563_settime(&test_time)){
            snprintf(temp,100,"%s","PCF8563_settime success!   \r\n");
            printf(temp);
            LCDShowFont32(8+32*2,REF_POS+32*3+SPACING*2+5,      //LCD��ʾ����ʱ��ɹ�
                  temp,LCD_WIDTH,BLACK,WHITE); 
          }else{
            snprintf(temp,100,"%s","PCF8563_settime fail!     \r\n");
            printf(temp);
            LCDShowFont32(8+32*2,REF_POS+32*3+SPACING*2+5,      //LCD��ʾ����ʱ��ʧ��
                          temp,LCD_WIDTH,BLACK,WHITE); 
          }
          state=1;                                              //���״̬���Ѿ�ˢ�£�������Ҫ���
        }break;
      }
      /* ��ȡ�趨ʱ�� */
      check_time_vaild(&test_time);                             //����趨ʱ��
      snprintf(temp,100,"�趨ʱ��:%04d/%02d/%02d %02d:%02d:%02d",//����LCD����ʾ�ı�
             test_time.year+2000,test_time.month,test_time.day,test_time.hour,test_time.minute,test_time.second);
      LCDShowFont32(8+32*2,REF_POS+32+5,                        //LCD��ʾ�ı�
                    temp,LCD_WIDTH,BLACK,WHITE); 
    }

    /* ��ʾ��ǰRTCʵʱʱ�� �� ״̬�� */
    if(count>=100 && PCF8563_gettime(&time)){
      count=0;
      /* ���״̬��LCD�ı� */
      if(state){
        state++;
        if(state>3){
          state=0;
          LCDShowFont32(8+32*2,REF_POS+32*3+SPACING*2+5,        //LCD״̬�������ı�
                    "                         ",LCD_WIDTH,BLACK,WHITE); 
        }
      }
      /* ��ȡʵ��ʱ�� */
      snprintf(temp,100,"ʵ��ʱ��:%04d/%02d/%02d %02d:%02d:%02d",//����LCD����ʾ�ı�
             time.year+2000,time.month,time.day,time.hour,time.minute,time.second);

      time.week = CountWeek(time.year+2000, time.month, time.day);
      switch(time.week){
        case 0:strcat(temp," Sunday    \r\n");break;
        case 1:strcat(temp," Monday    \r\n");break;
        case 2:strcat(temp," Tuesday    \r\n");break;
        case 3:strcat(temp," Wednesday    \r\n");break;
        case 4:strcat(temp," Thursday    \r\n");break;
        case 5:strcat(temp," Friday    \r\n");break;
        case 6:strcat(temp," Saturday    \r\n");break;
        default:strcat(temp," error    \r\n");break;
      }
      LCDShowFont32(8+32*2,REF_POS+32*2+SPACING+5,              //LCD��ʾ�ı�
                    temp,LCD_WIDTH,BLACK,WHITE); 
      printf(temp);
    }
    
    count++;
    led_app(10);
    delay_ms(10);                                               //��ʱ
  }
}  
