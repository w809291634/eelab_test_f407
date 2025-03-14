/*********************************************************************************************
* �ļ���main.c
* ���ߣ�Liyw 2018.1.17
* ˵����Ƕ��ʽ�ӿڼ�����Ŀʵѵ��
*       ��ͳ��ȼ������û�еʽ��ƣ��ڷ���ͷ�ֹú��й©�϶�����ȱ�ݡ�����Ҫ���
*       һ�������������ȼ��й©���������С��ü������ܹ�ͨ����ť������
*       ����̨���أ�ȼ�����򿪣�ͨ����ť���ƻ���С�����Ϊ4����С���л𡢸߻�
*       ��𣩣�����̨�򿪵�û�м�⵽�л���ͬʱȼ��й©ʱ���������źţ�ͬʱȼ����
*       �رա�LCD��Ļ����ʾ�������״̬������С�������⡢ȼ�����ͳ���logo��
*       K4:��Ϊȼ������ܿ���
*       ������:����������ʾ��
*       D1\D2\D3\D4:���浵λ��ʾ
*       RGB:ȼ�������Ч��ʾ
*       K1:���浵λ��
*       K2:���浵λ��
*       �̵���1:ģ����Ȼ���ܷ�
*       LCD��Ļ:��������Ϣ��ʾ����
* �޸ģ�
* ע�ͣ�ϵͳ������Ҫ�ܹ���⵽�������޷��ṩ����ɽ�main.c�ļ���get_flame_status()����ע�͵�
*********************************************************************************************/
#include "extend.h"                                             //�⺯��ͷ�ļ�
#include "led.h"                                                //led��ͷ�ļ�
#include "delay.h"                                              //ϵͳ��ʱͷ�ļ�
#include "timer.h"                                              //��ʱ��ͷ�ļ�
#include "usart.h"                                              //���ڿ��ļ�
#include "rgb.h"                                                //RGB��ͷ�ļ�
#include "key.h"                                                //����ͷ�ļ�
#include "buzzer.h"                                             //������ͷ�ļ�
#include "relay.h"                                              //�̵���ͷ�ļ�
#include "lcd.h"                                                //LCD����ͷ�ļ�
#include "MP-4.h"                                               //ȼ����⴫����ͷ�ļ�
#include "Flame.h"                                              //���洫����ͷ�ļ�

/*********************************************************************************************
* �����궨��
*********************************************************************************************/
#define GAS_THRESHOLD           2000                            //ȼ��Ũ����ֵ
#define PUTOUT_SCREEN_DELAY     5000                            //Ϩ��������ֵ

/*********************************************************************************************
* ϵͳ��־λ
*********************************************************************************************/
uint8_t rgb_twinkle;                                            //RGB����˸ִ�б�־λ
uint8_t gas_check;                                              //ȼ�����ִ�б�־λ
uint8_t fire_effect;                                            //����ͼ��֡�л���־λ

/*********************************************************************************************
* ��Ļ��ʾ�洢����ȫ�ֱ���
* ����ͼƬC�ļ�ʹ��emwin BmpCvtST.exe������ɣ��򿪺󱣴�ΪC��ʽ��
* ע��:
*       1��ѡ�������ʽ����Ϊ��High color (565] red and blue swapped
*       2����Ҫ�޸���������ɵ�C�ļ����������޸ģ�ÿ��ͼƬ��������������
*       ǰ����Ԫ�ر�ʾͼƬ���ؿ�Ⱥ͸߶ȣ���Ҫ�Լ��ֶ����䡣
*********************************************************************************************/
extern unsigned short _acflame_off[];                           //����ر�
extern unsigned short _acflame_on_1[];                          //���濪��1
extern unsigned short _acflame_on_2[];                          //���濪��2
extern unsigned short _acflame_on_3[];                          //���濪��3
extern unsigned short _acflame_on_4[];                          //���濪��4
extern unsigned short _acflame_big_on[];                        //��𿪹ؿ�
extern unsigned short _acflame_big_off[];                       //��𿪹ع�
extern unsigned short _acflame_high_on[];                       //�߻𿪹ؿ�
extern unsigned short _acflame_high_off[];                      //�߻𿪹ع�
extern unsigned short _acflame_medium_on[];                     //�л𿪹ؿ�
extern unsigned short _acflame_medium_off[];                    //�л𿪹ع�
extern unsigned short _acflame_small_on[];                      //С�𿪹ؿ�
extern unsigned short _acflame_small_off[];                     //С�𿪹ع�
extern unsigned short _actotal_switch_on[];                     //�ܿ��ؿ�
extern unsigned short _actotal_switch_off[];                    //�ܿ��ع�
extern unsigned short _acgas_leakage_t[];                       //ȼ��й©
extern unsigned short _acgas_leakage_f[];                       //ȼ��δй©
extern unsigned short _acidentification[];                      //ȼ������ַ�
extern unsigned short _acaux_logo[];                            //AUX logo��ʶ

/*********************************************************************************************
* ����ͼ��λ��
*********************************************************************************************/
#define TOTAL_SWITCH_X  ((int)(LCD_WIDTH*0.05f))                //�ܿ���ͼ����ʼλ��
#define TOTAL_SWITCH_Y  ((int)(LCD_HEIGHT*0.05f))

#define AUX_LOGO_X      ((int)(LCD_WIDTH*0.8f))                 //AUXͼ����ʼλ��
#define AUX_LOGO_Y      ((int)(LCD_HEIGHT*0.05f))

#define FLAME_X         ((int)(LCD_WIDTH*0.31f))                //����ͼ����ʼλ��
#define FLAME_Y         ((int)(LCD_HEIGHT*0.17f))

#define FLAME_S_X       ((int)(LCD_WIDTH*0.31f))                //����"С"�ַ�ͼ����ʼλ��
#define FLAME_S_Y       ((int)(LCD_HEIGHT*0.57f))
#define FLAME_M_X       ((int)(LCD_WIDTH*0.41f))                //����"��"�ַ�ͼ����ʼλ��
#define FLAME_M_Y       ((int)(LCD_HEIGHT*0.57f))
#define FLAME_H_X       ((int)(LCD_WIDTH*0.51f))                //����"��"�ַ�ͼ����ʼλ��
#define FLAME_H_Y       ((int)(LCD_HEIGHT*0.57f))
#define FLAME_B_X       ((int)(LCD_WIDTH*0.61f))                //����"��"�ַ�ͼ����ʼλ��
#define FLAME_B_Y       ((int)(LCD_HEIGHT*0.57f))

#define IDENTIF_X       ((int)(LCD_WIDTH*0.05f))                //���ı�ʶͼ����ʼλ��
#define IDENTIF_Y       ((int)(LCD_HEIGHT*0.825f))

#define GAS_LEAKAGE_X   ((int)(LCD_WIDTH*0.26f))                //ȼ��й¶��ʶͼ����ʼλ��
#define GAS_LEAKAGE_Y   ((int)(LCD_HEIGHT*0.833f))

#define MAGNIFICATION   2                                       //ͼ��ķŴ���

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
  rgb_init();                                                   //RGB�Ƴ�ʼ��
  timer_init();                                                 //��ʱ��ʱ��Ƭ���ܳ�ʼ��
  buzzer_init();                                                //��������ʼ��
  relay_init();                                                 //�̵�����ʼ��
  combustiblegas_init();                                        //ȼ����������ʼ��
  flame_init();                                                 //���洫������ʼ��

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
  //����ϵͳ��������־λ
  uint8_t total_switch = 0;                                     //ȼ�����ܿ��ر�־λ
  uint8_t last_total_switch = 0;                                //������һ���ܿ��ر�־λ
  uint8_t key4_switch = 0;                                      //K4��������
  uint16_t count = 0;                                           //Ϩ����������
  uint8_t fire_level = 0;                                       //�����С��־λ
  uint8_t fire_set_last = 0;                                    //�����С��־λ�洢����
  uint32_t gas_concentration = 0;                               //ȼ��Ũ�ȴ洢����
  uint8_t fire_monitor = 0;                                     //���������洢����
  uint8_t safety_lock = 0;                                      //ϵͳ��ȫ����־λ
  uint8_t fire_film = 0;                                        //����ͼ��֡���Ʊ�־λ
  uint8_t rgb_level = 0;                                        //RGB����˸״̬���Ʊ�־λ
  //��ʼ��ϵͳ�����豸
  hardware_init();
  //��Ļ��ʼ״̬����
  LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y, _actotal_switch_off[0], _actotal_switch_off[1], _actotal_switch_off,MAGNIFICATION);//�����ܿ��عر�Ч��
  LCD_FillData_ex(AUX_LOGO_X, AUX_LOGO_Y, _acaux_logo[0], _acaux_logo[1], _acaux_logo,MAGNIFICATION);//����AUXͼ��λ��
  LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_off[0], _acflame_off[1], _acflame_off,MAGNIFICATION); //���ػ���ر�Ч��
  LCD_FillData_ex(FLAME_S_X, FLAME_S_Y ,_acflame_small_off[0], _acflame_small_off[1], _acflame_small_off,MAGNIFICATION);//�����»�ر�Ч��
  LCD_FillData_ex(FLAME_M_X, FLAME_M_Y ,_acflame_medium_off[0], _acflame_medium_off[1], _acflame_medium_off,MAGNIFICATION);//�����л�ر�Ч��
  LCD_FillData_ex(FLAME_H_X, FLAME_H_Y, _acflame_high_off[0], _acflame_high_off[1], _acflame_high_off,MAGNIFICATION);//���ظ߻�ر�Ч��
  LCD_FillData_ex(FLAME_B_X, FLAME_B_Y, _acflame_big_off[0], _acflame_big_off[1], _acflame_big_off,MAGNIFICATION);//���ش��ر�Ч��
  LCD_FillData_ex(IDENTIF_X, IDENTIF_Y, _acidentification[0], _acidentification[1], _acidentification,MAGNIFICATION);//����"ȼ�����"��ʶ
  
  //ϵͳ��ѭ����̨����
  while(1){                                                     //��ѭ��
    //��ⰲȫ�����ܿ���״̬��������ز�����ʾ�ܿ���Ч��
    if(safety_lock){                                            //��ⰲȫ���Ƿ�����
      if(!key_status(K4)){
        delay_ms(50);
        if(!key_status(K4)){
          key4_switch = 1 - key4_switch;
          while(!key_status(K4));
        }
      }
      if(key4_switch){                                          //���K4�����Ƿ�Ϊ����
        if(total_switch != 1){                                  //���ϵͳ�ܿ����Ƿ���λ
          buzzer_tweet();                                       //����������
          total_switch = 1;                                     //�ܿ��ؿ�
          RELAY1_CTRL(ON);                                      //�̵�����ȼ��������
          LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y, _actotal_switch_on[0], _actotal_switch_on[1], _actotal_switch_on,MAGNIFICATION);//�����ܿ��ؿ�Ч��
        }
      }else{                                                    //����
        if(total_switch == 1){                                  //����ܿ����Ƿ�ر�
          rgb_ctrl(0);                                          //RGB�ƹر�
          buzzer_tweet();                                       //����������
          total_switch = 0;                                     //�ܿ��عر�
          fire_level = 0;                                       //������ƹر�
          RELAY1_CTRL(OFF);                                     //�̵�����ȼ�������ر�
          LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y, _actotal_switch_off[0], _actotal_switch_off[1], _actotal_switch_off,MAGNIFICATION);//�����ܿ��عر�Ч��
        }
      }
    }
    
    //����ܿ��ش򿪺�ȼ�����Ƿ��������������ز���
    if(total_switch /*&& get_flame_status()*/){                 //�ж��ܿ����Ƿ�򿪣��Ƿ��������
      if(fire_monitor == 0) fire_level = 1;                     //�жϻ������Ƿ���λ�����û���Ϊ1��
      fire_monitor = 1;                                         //���û������־λΪ����
      last_total_switch=total_switch;                           //��¼��ǰ�ܿ���״̬
    }else{                                                      //����
      if(last_total_switch!=total_switch){                      //����ˢ��Ϊ�ܿ��عر�״̬
        last_total_switch=total_switch;
        LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_off[0], _acflame_off[1], _acflame_off,MAGNIFICATION);//���ػ���ر�Ч��
        LCD_FillData_ex(FLAME_S_X, FLAME_S_Y, _acflame_small_off[0], _acflame_small_off[1], _acflame_small_off,MAGNIFICATION);//�����»�ر�Ч��
        LCD_FillData_ex(FLAME_M_X, FLAME_M_Y, _acflame_medium_off[0], _acflame_medium_off[1], _acflame_medium_off,MAGNIFICATION);//�����л�ر�Ч��
        LCD_FillData_ex(FLAME_H_X, FLAME_H_Y, _acflame_high_off[0], _acflame_high_off[1], _acflame_high_off,MAGNIFICATION);//���ظ߻�ر�Ч��
        LCD_FillData_ex(FLAME_B_X, FLAME_B_Y, _acflame_big_off[0], _acflame_big_off[1], _acflame_big_off,MAGNIFICATION);//���ش��ر�Ч��
      }
      fire_level = 0;                                           //����ȼ�����Ϊ0�ر�
      fire_monitor = 0;                                         //�����źż���쳣
    }
    
    //��Ļ����Ч������
    if(fire_monitor && fire_effect){                            //�������Ƿ�����������ͼ��֡�л��Ƿ���λ
      switch(fire_film % 4){                                    //����֡��־λȡ��
      case 0:                                                   //����0ʱ
        LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_on_1[0], _acflame_on_1[1], _acflame_on_1,MAGNIFICATION);//���»��濪1
        break;                                                  //����
      case 1:                                                   //����1ʱ
        LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_on_2[0], _acflame_on_2[1], _acflame_on_2,MAGNIFICATION);//���»��濪2
        break;                                                  //����
      case 2:                                                   //����2ʱ
        LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_on_3[0], _acflame_on_3[1], _acflame_on_3,MAGNIFICATION);//���»��濪3
        break;                                                  //����
      case 3:                                                   //����3ʱ
        LCD_FillData_ex(FLAME_X, FLAME_Y, _acflame_on_4[0], _acflame_on_4[1], _acflame_on_4,MAGNIFICATION);//���»��濪4
        break;                                                  //����
      }    
      fire_effect = 0;                                          //��ջ���֡�л���־λ
      fire_film ++;                                             //����֡��־λ��һ
    }
    
    //RGB����˸Ч������
    if(total_switch && rgb_twinkle){                            //ϵͳ�ܿ����Ƿ�����RGB����˸��ִ�б�־λ�Ƿ���λ
      rgb_ctrl(rgb_level % 7 + 1);                              //RGB����˸״̬���Ʊ�־λ
      rgb_level ++;                                             //RGB����˸���Ʊ�־λ��һ
      rgb_twinkle = 0;                                          //RGB����˸ִ�б�־λ���        
    }
    
    //Ϩ����������
    if(total_switch || !safety_lock){                           //�ܿ����Ƿ�������ȫ���Ƿ��쳣
      LCD_BLCtrl(1);                                            //��Ļ����
      count = 0;                                                //Ϩ����ʱ�������
    }else{                                                      //����
      if(count == PUTOUT_SCREEN_DELAY){                         //Ϩ����ʱ�����Ƿ���Ϩ����ʱ��ֵһ��
        LCD_BLCtrl(0);                                          //��ĻϨ��
      }else count ++;                                           //����Ϩ����ʱ������һ
    }
    
    //����ȼ����ü���������
    if(total_switch && safety_lock && fire_monitor){            //�ܿ��أ���ȫ���أ��������Ƿ������
      if(!key_status(K1)){                                      //��ⰴ��1�Ƿ񱻰���
        buzzer_tweet();                                         //����������
        if(!key_status(K1)){                                    //��ⰴ���Ƿ�ȷʵ������
          while(!key_status(K1));                               //�ȴ�����̧��
          delay_ms(10);                                         //��ʱ10ms
          if(!(fire_level & 0x08)){                             //�����漶���Ƿ�������
            fire_level = (fire_level << 1) + 1;                 //����ȼ���һ
          }
        }
      }
      if(!key_status(K2)){                                      //��ⰴ��2�Ƿ񱻰���
        buzzer_tweet();                                         //����������
        if(!key_status(K2)){                                    //��ⰴ��2�Ƿ�ȷʵ������
          while(!key_status(K2));                               //�ȴ�����̧��
          delay_ms(10);                                         //��ʱ10ms
          if(fire_level & 0x02){                                //�����漶���Ƿ����ÿ�
            fire_level = (fire_level >> 1);                     //����ȼ���һ
          }
        }
      }
    }
    
    //ִ�л���ȼ��Ŀ���Ч��
    if(fire_level != fire_set_last){                            //�鿴�˴λ�������ֵ�Ƿ���ǰһ����ͬ
      
      led_control(fire_level);                                  //���ƻ���ȼ���ʾ��
      
      if((fire_level & 0x01) && (fire_level == 0x01)){          //�жϻ���ȼ��Ƿ�ΪС��
        LCD_FillData_ex(FLAME_S_X, FLAME_S_Y ,_acflame_small_on[0], _acflame_small_on[1], _acflame_small_on,MAGNIFICATION);//ִ��С��ť����
      }else LCD_FillData_ex(FLAME_S_X, FLAME_S_Y ,_acflame_small_off[0], _acflame_small_off[1], _acflame_small_off,MAGNIFICATION);//ִ��С��ťϨ��
      
      if((fire_level & 0x02) && (fire_level == 0x03)){          //�жϻ���ȼ��Ƿ�Ϊ�л�
        LCD_FillData_ex(FLAME_M_X, FLAME_M_Y, _acflame_medium_on[0], _acflame_medium_on[1], _acflame_medium_on,MAGNIFICATION);//ִ���л�ť����
      }else LCD_FillData_ex(FLAME_M_X, FLAME_M_Y, _acflame_medium_off[0], _acflame_medium_off[1], _acflame_medium_off,MAGNIFICATION);//ִ���л�ťϨ��
      
      if((fire_level & 0x04) && (fire_level == 0x07)){          //�жϻ���ȼ��Ƿ�Ϊ�߻�   
        LCD_FillData_ex(FLAME_H_X, FLAME_H_Y, _acflame_high_on[0], _acflame_high_on[1], _acflame_high_on,MAGNIFICATION);//ִ�и߻�ť����
      }else LCD_FillData_ex(FLAME_H_X, FLAME_H_Y, _acflame_high_off[0], _acflame_high_off[1], _acflame_high_off,MAGNIFICATION);//ִ�и߻�ťϨ��
      
      if((fire_level & 0x08) && (fire_level == 0x0f)){          //�жϻ���ȼ��Ƿ�Ϊ���   
        LCD_FillData_ex(FLAME_B_X, FLAME_B_Y, _acflame_big_on[0], _acflame_big_on[1], _acflame_big_on,MAGNIFICATION);//ִ�д��ť����
      }else LCD_FillData_ex(FLAME_B_X, FLAME_B_Y, _acflame_big_off[0], _acflame_big_off[1], _acflame_big_off,MAGNIFICATION);//ִ�д��ťϨ��
      
      fire_set_last = fire_level;                               //�洢�˴εĻ�����Ƶȼ�
    }
    
    //ȼ�����Ͳ�����ȡ
    if(gas_check){                                              //ȼ�����ִ�б�־λ�Ƿ���λ
      gas_concentration = get_combustiblegas_data();            //��ȡȼ��Ũ�ȼ��ֵ        
      printf("gas_concentration:%d\r\n",gas_concentration);
      gas_check = 0;                                            //���ȼ�����ִ�б�־λ
    }
    
    //ִ��ȼ��й©״�����
    if(gas_concentration > GAS_THRESHOLD){                      //ȼ��Ũ��ֵ�Ƿ񳬹�ȼ��Ũ����ֵ
      if(safety_lock != 0){                                     //��ⰲȫ���Ƿ�λ
        safety_lock = 0;                                        //��ȫ����λ
        total_switch = 0;                                       //�ܿ��عر�
        fire_level = 0;                                         //���漶ֵ0
        rgb_ctrl(0);                                            //RGB�ƹر�
        RELAY1_CTRL(OFF);                                       //�̵�����ȼ�������ر�
        LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y, _actotal_switch_off[0], _actotal_switch_off[1], _actotal_switch_off,MAGNIFICATION);//�����ܿ��عر�Ч�� 
      }
      buzzer_tweet();                                           //����������
      LCD_FillData_ex(GAS_LEAKAGE_X, GAS_LEAKAGE_Y, _acgas_leakage_t[0], _acgas_leakage_t[1], _acgas_leakage_t,MAGNIFICATION);//ȼ��й©��ʶ����
      delay_ms(50);                                             //��ʱ50MS
      LCD_FillData_ex(GAS_LEAKAGE_X, GAS_LEAKAGE_Y, _acgas_leakage_f[0], _acgas_leakage_f[1], _acgas_leakage_f,MAGNIFICATION);//ȼ��й©��ʶϨ��
    }else{
      if(safety_lock != 1){                                     //��ⰲȫ���Ƿ���λ
        safety_lock = 1;                                        //��ȫ����λ
        LCD_FillData_ex(GAS_LEAKAGE_X, GAS_LEAKAGE_Y, _acgas_leakage_f[0], _acgas_leakage_f[1], _acgas_leakage_f,MAGNIFICATION);//ȼ��й©��ʶϨ��
      }
    }
    //ѭ������ʱ
    delay_ms(1);                                                //��ʱ1MS
  }
}
