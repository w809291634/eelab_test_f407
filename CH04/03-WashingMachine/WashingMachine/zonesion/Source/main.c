/*********************************************************************************************
* �ļ���main.c
* ���ߣ�Liyw 2018.1.22
* ˵����Ƕ��ʽ�ӿڼ�����Ŀʵѵ��  
*       ���ϴ�»����ܶ�����������������������ڶ�����˼������Դ�˷ѡ������һ���
*       ��������ϴ�»���ϴ�»�ӵ�е����ܿ��أ�ģʽѡ�񰴼�ʱ�����ð���������������ģ
*       ʽѡ���п������ý��ݡ�ϴ�¡���ˮ��ȫ���ܡ��ڽ��ݿ���ʱ��עˮ��ŷ���עˮ���
*       ��ر�Ȼ����ݡ�ϴ��ģʽʱ�����ϴ��ģʽ��������ϴ��ɺ�򿪷�ˮ��ŷ���ˮ����
*       ��ˮģʽʱ��ˮ��ŷ�ֱ�Ӵ򿪣����ȫ������ת��ȫ����ģʽʱ���ǰ������̽��ݡ�ϴ
*       �¡���ˮ����ɡ������ڰ���ʱ��������������ʾ��LCD��Ļ�ϻ���ʾϴ�»�����ģʽ��
*       ��ʱ�͵�ǰ����״̬����
*       K1:ϴ�»��ܿ���
*       K2:ϴ��ģʽѡ�񰴼�
*       K3:ϴ��ˮλ����
*       K4:ϴ����������
*       ������:����������ʾ��
*       rgb�ƣ�ϵͳ����ָʾ��
*       �̵���1:עˮ���ƿ���
*       �̵���2:��ˮ���ƿ���
*       ���������ϴ�»����
*       LCD��Ļ:ϴ�»���Ϣ��ʾ����
* �޸ģ�
* ע�ͣ�ϵͳ����ʱʹ�õ��Ǳ�׼��1S��ʱ�����з��ӻ���ʵ��Ч�������ԣ��ɽ�334�е�120��119����Ϊ12��11.
*********************************************************************************************/
#include "extend.h"                                             //�⺯��ͷ�ļ�
#include "delay.h"                                              //��ʱ����ͷ�ļ�
#include "lcd.h"                                                //��Ļ��ʾͷ�ļ�
#include "led.h"                                                //led������ͷ�ļ�
#include "rgb.h"                                                //rgb������ͷ�ļ�
#include "relay.h"                                              //�̵�������ͷ�ļ�
#include "buzzer.h"                                             //����������ͷ�ļ�
#include "key.h"                                                //��������ͷ�ļ�
#include "stepmotor.h"                                          //�����������ͷ�ļ�
#include "timer.h"                                              //��ʱ������ͷ�ļ�
#include "screen.h"                                             //����ͷ�ļ�
#include "usart.h"                                              //����ͷ�ļ�

/*********************************************************************************************
* �����궨��
*********************************************************************************************/
#define EFFECTIVE_NUM           100                             //���Iѭ�������Ч����
#define MODE_NUM                0x10                            //��Чģʽ��
#define WATER_LEVEL             7                               //ˮλ�ȼ�����

#define FORWARD                 0x03                            //�����ת����ָ��
#define RAVERSAL                0x01                            //�����ת����ָ��
#define MOTOR_STOP              0x00                            //���ָֹͣ��

#define SOAK_TIM                20                              //����ʱ������
#define CLEAN_TIM               10                              //��ϴʱ������
#define DEHYD_TIM               5                               //��ˮʱ������
    
#define SOAK_TIME               SOAK_TIM                        //����ʱ��
#define WASH_TIME               (CLEAN_TIM * 2)                 //ϴ��ʱ��
#define RINS_TIME               (CLEAN_TIM * 3)                 //Ưϴʱ��
#define DEHY_TIME               DEHYD_TIM                       //��ˮʱ��
//ȫ���ܺ�ʱʱ��
#define TOTAL_TIME              (SOAK_TIME + WASH_TIME + RINS_TIME + DEHY_TIME * 4)

#define PUTOUT_SCREEN_DELAY     10000                           //Ϩ��������ֵ,��λms

/*********************************************************************************************
* ϵͳ��־λ
*********************************************************************************************/
uint8_t rgb_twinkle;                                            //RGB����˸ִ�б�־λ
uint8_t mode_twinkle;                                           //ģʽͼ����˸ִ�б�־λ
uint8_t tim_second;                                             //���ź�ʱ��ִ�б�־λ

/*********************************************************************************************
* ϵͳͨ������ 1��עˮ��2����ˮ��3�����ݣ�4��ϴ�ӣ�5����ˮ
*********************************************************************************************/
uint8_t soak_program[] = {1, 3, 2};                             //���ݹ�����������
uint8_t wash_program[] = {1, 4, 4, 2};                          //ϴ�¹�����������
uint8_t rins_program[] = {1, 4 ,2, 1, 4 ,2, 1, 4 ,2};           //��ϴ������������
uint8_t dehy_program[] = {2, 5};                                //��ˮ������������
//ȫ����ϴ����������
uint8_t total_program[]= {1, 3, 4, 4, 2, 5, 1, 4, 2, 5, 1, 4, 2, 5, 1, 4 ,2, 5};

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
  timer_init();                                                 //ϵͳʱ�ӽ��ĳ�ʼ��
  led_init();                                                   //LED��ʼ��
  rgb_init();                                                   //rgb�Ƴ�ʼ��
  key_init();                                                   //������ʼ��
  buzzer_init();                                                //��������ʼ��
  relay_init();                                                 //�̵�����ʼ��
  stepmotor_init();                                             //���������ʼ��
  
  lcd_init(NO_NAME);                                            //LCD��ʼ��
}

/*********************************************************************************************
* ���ƣ�main()
* ���ܣ�ʵѵ����Ŀ�߼�����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int main(void)
{
  uint16_t total_count = 0;                                     //�ܿ���ѭ������������
  uint8_t total_switch = 0;                                     //�ܿ���״̬��־λ
  uint8_t total_flag = 0;                                       //�ܿ����л���־λ
    
  uint16_t mode_count = 0;                                      //ģʽ���ܰ���ѭ������������
  uint8_t mode_select = 0;                                      //ģʽѡ���־λ
  uint8_t mode_status = 0;                                      //ģʽͼ��״̬��־λ
  uint8_t mode_temp = 0;                                        //ѡ��ģʽ�������
  uint8_t mode_flag = 0;                                        //ģʽ���������л���־λ
  uint8_t *mode_confirm;                                        //ϴ�»�����ִ��ָ��
  uint8_t mode_step = 0;                                        //ϴ�»�����ִ�ж����л���־λ
  uint8_t diffe_step = 0;                                       //ϴ�»���ǰִ�ж���״ָ̬ʾ��־λ
  
  uint16_t water_count = 0;                                     //ˮλ���ð�ťѭ������������
  uint8_t water_level = 0;                                      //ˮ������ѡ���־λ
  uint8_t level_flag = 0;                                       //ˮλ�����л���־λ
  
  uint16_t startup_count = 0;                                   //������ťѭ������������
  uint8_t startup_switch = 0;                                   //������ť״̬��־λ
  uint8_t startup_flag = 0;                                     //������ť״̬�л���־λ
  
  uint8_t rgb_level = 0;                                        //rgb��״̬�л���־λ
  
  uint8_t sec_twinkle = 0;                                      //��ͼ��˨˵��־λ
  
  uint16_t total_time = 0;                                      //������ʱ�����
  uint16_t diffe_time = 0;                                      //ϴ�»������ڶ���ʱ���¼����
  uint16_t timing_flag = 0;                                     //ϴ�»������л���־λ
  
  uint8_t work_flag = 0;                                        //ϴ�»�����ִ�б�־λ

  uint8_t end_flag = 0;                                         //ϴ�»�������ɱ�־λ
  
  uint8_t add_water_flag = 0;                                   //ϴ�»�עˮ��־λ
  uint8_t sub_water_flag = 0;                                   //ϴ�»���ˮ��־λ
  uint8_t soaking_flag = 0;                                     //���ݱ�־λ
  uint8_t washing_flag = 0;                                     //��ϴ��־λ
  uint8_t dehydration_flag = 0;                                 //��ˮ��־λ
  
  uint8_t motor_dir = 0;                                        //�������ת���Ʊ�־λ
  
  uint32_t shutdown_st = 0;                                     //Ϩ����ʼʱ��

  /*�豸Ӳ����ʼ��*/
  hardware_init();                                              //Ӳ����ʼ��
  
  /*�豸��ʼ״̬����*/
  RELAY1_CTRL(OFF);                                             //�ر�עˮ�̵���
  RELAY2_CTRL(OFF);                                             //�رշ�ˮ�̵���
  stepmotor_control(MOTOR_STOP);                                //�رյ��
  
  /*������Ϣ��ʼ��*/
  show_logo();                                                  //��ӡ�豸logoͼ��
  show_switch(OFF);                                             //��ӡϵͳ�ܿ���ͼ��
  show_soak(ON);                                                //��ӡ���ݹ���ͼ��
  show_wash(ON);                                                //��ӡϴ�¹���ͼ��
  show_rinse(ON);                                               //��ӡ��ϴ����ͼ��
  show_dehyd(ON);                                               //��ӡ��ˮ����ͼ��
  show_all_mode(ON);                                            //��ӡȫ����ͼ��
  show_timer(0, 00);                                            //��ӡϵͳִ��ʱ����Ϣ
  show_level(0);                                                //��ӡˮλֵ��Ϣ
  show_point(ON);                                               //��ӡ���ͼ��

  /*����������*/
  while(1){                                                     //��ѭ��
    /*�ܿ��ذ���״̬��⼰����״̬��ֵ*/
    if(key_status(K1) == 0) total_count ++;                     //ѭ����ⰴ������״̬������
    if(key_status(K1) && (total_count > EFFECTIVE_NUM)){        //������̧�𣬼�ⰴ������ʱ���������Ч��
      if(total_switch == 0) total_switch = 1;                   //����ܿ���Ϊ�ر�����
      else total_switch = 0;                                    //�����ܿ��ش�
      total_count = 0;                                          //�ܿ��ذ���ѭ������������
      buzzer_tweet();                                           //����������
      total_flag = 1;                                           //�ܿ��ذ���״̬�л���־λ��λ
    }

    /*ģʽѡ�񰴼�״̬��⼰ģʽ��ֵ*/
    if(total_switch && !work_flag){                             //�ܿ����Ƿ�򿪡�ϴ�³����Ƿ�ʼִ��
      if(key_status(K2) == 0) mode_count ++;                    //ѭ�����ģʽѡ�񰴼�����״̬������
      if(key_status(K2) && (mode_count > EFFECTIVE_NUM)){       //�������̧�𣬼�ⰴ������ʱ���������Ч��
        //���ģʽС���趨ģʽ��������ģʽ״̬Ϊ0����ôģʽ��һ
        if((mode_select >= MODE_NUM) || (mode_select == 0)) mode_select = 1;
        else mode_select <<= 1;                                 //����ģʽ����һλ
        mode_count = 0;                                         //����ѭ���������
        buzzer_tweet();                                         //����������
        mode_flag = 1;                                          //ģʽ�����л���־λ��λ
      }
    }
    /*ˮλ���ÿ���״̬��⼰ģʽ��ֵ*/
    if(total_switch && !work_flag){                             //�ܿ����Ƿ�����ϴ�³����Ƿ�����
      if(key_status(K3) == 0) water_count ++;                   //ѭ�����ˮλ���ð����Ƿ��²�����
      if(key_status(K3) && (water_count > EFFECTIVE_NUM)){      //�������̧�𣬼�ⰴ������ʱ���������Ч��
        if(water_level > WATER_LEVEL) water_level = 1;          //���ˮλ��־λ�����趨ˮλ����ˮλ״̬��־λ��λ
        else water_level ++;                                    //����ˮλֵ��һ
        water_count = 0;                                        //ˮλ���ð���ѭ������������
        buzzer_tweet();                                         //����������
        level_flag = 1;                                         //ˮλ״̬�л���־λ��λ
        show_level(water_level * 5);                            //��Ļ��ʾ����ˮλ��Ϣ
      }
    }
    /*ϴ�³����������ؼ�⼰ģʽ��ֵ*/
    if(total_switch){                                           //�ܿ����Ƿ��
      if(key_status(K4) == 0) startup_count ++;                 //������������Ƿ��²�����
      if(key_status(K4) && (startup_count > EFFECTIVE_NUM)){    //�������̧�𣬼�ⰴ������ʵ�ϲ������Ч��
        if(startup_switch == 0) startup_switch = 1;             //�����������״̬Ϊ0����״̬��λ     
        else startup_switch = 0;                                //�����������״̬��λ
        startup_count = 0;                                      //����ֵ����
        buzzer_tweet();                                         //����������
        startup_flag = 1;                                       //��������״̬�л���־λ��λ
      }
    }
    /*Ϩ������*/
    if(total_switch){                                           //����ܿ��ش�
      if(total_flag){                                           //����ܿ���״̬�л���־λ��λ
        LCD_BLCtrl(1);                                          //����Ļ��
      }
      shutdown_st = xtime();
    }else{
      if(xtime()-shutdown_st>PUTOUT_SCREEN_DELAY){
        shutdown_st = xtime();
        LCD_BLCtrl(0);                                          //�������ֵ�ﵽ���趨����ʱʱ���Ϩ��
      }
    }
    /*�ܿ���״̬�жϼ�ϵͳ����*/
    if(total_switch && !end_flag){                              //ϵͳ�ܿ����Ƿ�����ϴ�½�����־λ��λ    
      if(total_flag){                                           //����ܿ���״̬�л���־λ��λ
        mode_select = 0;                                        //ģʽѡ��λ
        water_level = 0;                                        //ˮλ���ø�λ
        startup_switch = 0;                                     //����������λ
        show_switch(ON);                                        //�ܿ���ͼ�����
      }                         
    }else{                                      
      if(total_flag || end_flag){                               //����ܿ��رգ�����������־λ��λ
        mode_select = 0;                                        //ģʽѡ��λ
        water_level = 0;                                        //ˮλ���ø�λ
        startup_switch = 0;                                     //�������ظ�λ
        total_time = 0;                                         //������ʱ������
        work_flag = 0;                                          //������־λ��λ
        rgb_ctrl(0);                                            //RGB��Ϩ��
        RELAY1_CTRL(OFF);                                       //עˮ��ŷ��ر�
        RELAY2_CTRL(OFF);                                       //��ˮ��ŷ��ر�
        stepmotor_control(MOTOR_STOP);                          //����������ƹر�
        show_timer(0, 00);                                      //ʱ����ʾ����
        show_level(0);                                          //ˮλ��ʾ����
        show_point(ON);                                         //�����ʾ
        show_soak(ON);                                          //����ͼ�곣��
        show_wash(ON);                                          //ϴ�±�־����
        show_rinse(ON);                                         //��ϴ��־����
        show_dehyd(ON);                                         //��ˮ��־����
        show_all_mode(ON);                                      //ȫ���ܱ�־����
        if(end_flag){                                           //�������ִ�н�����־��λ
          for(char i=0; i<8; i++){                              
            buzzer_tweet();                                     //����������8��
            delay_ms(1000);                                     //��ʱ1S
          }
          end_flag = 0;                                         //���������־λ��λ
        }else show_switch(OFF);                                 //����ϵͳ�ܿ���ͼ����ʾ�ر�
      }
    }   
    /*ϵͳ����RGBָʾ�ƿ���*/
    if(total_switch){                                           //�ؿ����Ƿ���
      if(rgb_twinkle){                                          //RGB����˸��ִ�б�־λ�Ƿ���λ
        rgb_ctrl(rgb_level % 7 + 1);                            //RGB����˸״̬���Ʊ�־λ
        rgb_level ++;                                           //RGB����˸���Ʊ�־λ��һ
        rgb_twinkle = 0;                                        //RGB����˸ִ�б�־λ���   
      }
    }
    /*��ȡϴ�³���ִ��ʱ��ͻ�ȡģʽ���ñ�־*/
    if(!work_flag){                                             //�������û��ִ��
      if(mode_flag || level_flag){                              //���ģʽ�л���־λ��ˮλ�л���־λ��λ
        switch(mode_select){                                    //�жϵ�ǰ���õ�ģʽ
        case 0x01:                                              //���Ϊ����ģʽ
          total_time = water_level * 2 + SOAK_TIME;             //�������ģʽִ����ʱ��
          mode_confirm = soak_program;                          //��ȡ����ģʽ��������
          break;                                                //����
        case 0x02:                                              //���Ϊģʽϴ��
          total_time = water_level * 2 + WASH_TIME;             //����ϴ��ģʽ��ʱ��
          mode_confirm = wash_program;                          //��ȡϴ��ģʽִ�в���
          break;                                                //����
        case 0x04:                                              //���Ϊ��ϴģʽ
          total_time = water_level * 6 + RINS_TIME;             //������ϴģʽ��ʱ��
          mode_confirm = rins_program;                          //��ȡ��ϴģʽ����
          break;                                                //����
        case 0x08:                                              //���Ϊ��ˮģʽ
          total_time = water_level + DEHY_TIME;                 //������ˮģʽ��ʱ��
          mode_confirm = dehy_program;                          //��ȡ��ˮģʽ����
          break;                                                //����
        case 0x10:                                              //���Ϊȫ����ģʽ
          total_time = water_level * 8 + TOTAL_TIME;            //����ȫ����ģʽ��ʱ��
          mode_confirm = total_program;                         //��ȡȫ����ģʽ����
          break;                                                //����
        }
      }
    }
    /*ϴ�»�����ͼ����ʾ����*/
    if(mode_twinkle && mode_select){                            //ͼ����˸ִ�б�־λ�Ƿ���λ��ģʽ�Ƿ�����
      mode_twinkle = 0;                                         //ͼ����˸��־λ��λ
      if(!mode_status){                                         //���ģʽ��ͼ��״̬��־λ��λ
        mode_temp = mode_select;                                //�洢��ǰϴ�»�����ģʽ
        mode_status = 1;                                        //ͼ��״̬��־λ��λ
        show_timer((total_time / 60), (total_time % 60));       //������ʾʱ��        
      }else mode_status = 0;                                    //ͼ��״̬��־λ��λ
      switch(mode_temp){                                        //ģʽ����״̬
      case 0x01:                                                //���Ϊ����ģʽ
        show_soak(mode_status);                                 //����ģʽͼ��ִ����˸
        break;                                                  //����
      case 0x02:                                                //���Ϊϴ��ģʽ
        show_wash(mode_status);                                 //ϴ��ģʽͼ��ִ����˸
        break;                                                  //����
      case 0x04:                                                //���Ϊ��ϴģʽ
        show_rinse(mode_status);                                //��ϴģʽͼ��ִ����˸
        break;                                                  //����
      case 0x08:                                                //���Ϊ��ˮģʽ
        show_dehyd(mode_status);                                //��ˮģʽͼ��ִ����˸
        break;                                                  //����
      case 0x10:                                                //���Ϊȫ����ģʽ
        show_all_mode(mode_status);                             //ȫ����ģʽͼ��ִ����˸
        break;                                                  //����
      }
    }
    /*ϴ�³��������жϺͱ�־λ����*/
    if(startup_switch){                                         //���������Ƿ��
      if(startup_flag){                                         //��������״̬��־λ�Ƿ���λ
        if(water_level && mode_select){                         //ˮλ��ģʽѡ���Ƿ�Ϊ��
          work_flag = 1;                                        //����ִ�б�־λ��λ
          timing_flag = 1;                                      //ʱ�������־λ��λ
        }else work_flag = 0;                                    //������ִ�б�־λ��λ
      }
    }else{
      if(startup_flag){                                         //�����������״̬��־λ��λ
        work_flag = 0;                                          //����ִ�б�־λ��λ
        RELAY1_CTRL(OFF);                                       //עˮ��ŷ��ر�
        RELAY1_CTRL(OFF);                                       //��ˮ��ŷ��ر�
        stepmotor_control(MOTOR_STOP);                          //����������ƹر�
      }
    }
    /*ϴ�»����źŶ���ִ��*/
    if(tim_second){                                             //���ź�ִ�б�־λ�Ƿ���λ
      if(work_flag){                                            //������־λ�Ƿ���λ
        tim_second = 0;                                         //���ź�ִ�б�־λ��λ
        show_point(ON);                                         //���ͼ�����
        sec_twinkle ++;                                         //����˸������һ
        if((sec_twinkle % 2) == 1){                             //�Ƿ�ﵽһ��
          if((sec_twinkle % 120) == 119){                       //�Ƿ�ﵽһ����
            if(total_time == 0){                                //ִ����ʱ���Ƿ�ִ�����
              work_flag = 0;                                    //������־λ��λ
              end_flag = 1;                                     //����������־λ��λ
            }else{
              total_time -= 1;                                  //����ʱ���һ
              diffe_time += 1;                                  //ϴ�¶���ʱ���һ
              timing_flag = 1;                                  //ʱ�������־λ��λ
            }
            sec_twinkle = 0;                                    //������������
          }
          show_point(OFF);                                      //���ͼ��Ϩ��
        }
      }
      else show_point(ON);                                      //�������ͼ�����
    }
    /*ϴ�»�ϴ�²���ִ�в���*/
    if(work_flag){                                              //���������־λ��λ
      if(timing_flag){                                          //���ʱ�������־λ��λ
        do{     
          mode_step = 0;                                        //ģʽ�����־λ��λ
          switch(*(mode_confirm + diffe_step)){                 //���ε�ǰϴ�»�Ҫִ�еĶ���
          case 1:                                               //���Ϊעˮ����
            if(diffe_time < water_level){                       //����ִ��ʱ���Ƿ�ﵽ�趨����ʱ��
              add_water_flag = 1;                               //עˮ��־λ��λ
            }else{
              add_water_flag = 0;                               //עˮ��־λ��λ
              diffe_time = 0;                                   //��ǰ������ʱ����
              diffe_step += 1;                                  //ģʽִ�ж�����һ
              mode_step = 1;                                    //ģʽ������λ
            }
            break;                                              //����
          case 2:                                               //���Ϊ��ˮ����
            if(diffe_time < water_level){                       //����ִ��ʱ���Ƿ�ﵽ�趨����ʱ��
              sub_water_flag = 1;                               //��ˮ��־λ��λ
            }else{
              sub_water_flag = 0;                               //��ˮ��־λ��λ
              diffe_time = 0;                                   //��ǰ������ʱ����
              diffe_step += 1;                                  //ģʽִ�ж�����һ
              mode_step = 1;                                    //ģʽ������λ
            }
            break;                                              //����
          case 3:                                               //���Ϊ���ݶ���
            if(diffe_time < SOAK_TIM){                          //����ִ��ʱ���Ƿ�ﵽ�趨����ʱ��
              soaking_flag = 1;                                 //���ݱ�׼λ��λ
            }else{
              soaking_flag = 0;                                 //���ݱ�־λ��λ
              diffe_time = 0;                                   //��ǰ������ʱ����
              diffe_step += 1;                                  //ģʽִ�ж�����һ
              mode_step = 1;                                    //ģʽ������λ
            }
            break;                                              //����
          case 4:                                               //���Ϊϴ�¶���
            if(diffe_time < CLEAN_TIM){                         //����ִ��ʱ���Ƿ�ﵽ�趨����ʱ��
              washing_flag = 1;                                 //ϴ�±�־λ��λ
            }else{
              washing_flag = 0;                                 //ϴ�±�־λ��λ
              diffe_time = 0;                                   //��ǰ������ʱ����
              diffe_step += 1;                                  //ģʽִ�ж�����һ
              mode_step = 1;                                    //ģʽ������λ
            }
            break;                                              //����
          case 5:                                               //���Ϊ��ˮ����
            if(diffe_time < DEHYD_TIM){                         //����ִ��ʱ���Ƿ�ﵽ�趨����ʱ��
              dehydration_flag = 1;                             //��ˮ������־λ��λ
            }else{
              dehydration_flag = 0;                             //��ˮ������־λ��λ
              diffe_time = 0;                                   //��ǰ������ʱ����
              diffe_step += 1;                                  //ģʽִ�ж�����һ
              mode_step = 1;                                    //ģʽ������λ
            }
            break;                                              //����
          }
        }while(mode_step);                                      //ģʽִ�б�־λ��λִ��
      }
    }
    /*ϴ�»����Ӳ���豸����*/
    if(work_flag){                                              //���������־λ��λ
      if(timing_flag){                                          //����������־λ��λ
        if(diffe_time == 0){                                    //�����ǰִ�ж���ִ�����
          RELAY1_CTRL(OFF);                                     //�ر�עˮ�̵���
          RELAY2_CTRL(OFF);                                     //�رշ�ˮ�̵���
          stepmotor_control(MOTOR_STOP);                        //�رղ������
        }

        if(add_water_flag) RELAY1_CTRL(ON);                     //���עˮ����עˮ�̵���
        
        if(sub_water_flag) RELAY2_CTRL(ON);                     //�����ˮ���򿪷�ˮ�̵���
        
        if(soaking_flag) stepmotor_control(MOTOR_STOP);         //������ݣ��رյ��
        
        if(washing_flag){                                       //���ϴ��
          if(motor_dir){                                        //�����־λ��λ
            stepmotor_control(FORWARD);                         //�����ת
            motor_dir = 0;                                      //��־λ��λ
          }else{
            stepmotor_control(RAVERSAL);                        //����ת
            motor_dir = 1;                                      //��־λ��λ
          }
        }
        
        if(dehydration_flag){                                   //�����ˮ
          stepmotor_control(FORWARD);                           //��ˮ�����
          RELAY2_CTRL(ON);
        }
      }
    }
    
    total_flag = 0;                                             //�ܿ��ر�־λ��λ
    mode_flag = 0;                                              //ģʽ�л���־λ��λ
    level_flag = 0;                                             //ˮλ�л���־λ��λ
    startup_flag = 0;                                           //����������־λ��λ
    timing_flag = 0;                                            //��ʱ�������־λ��λ
  }
}
