/*********************************************************************************************
* �ļ���screen.c
* ���ߣ�Liyw 2018.1.22
* ˵����
* ���ܣ���Ļ��������
* �޸ģ�wanghao ����ע�ͣ�����ͼ��λ�õĺ꣬�����LCD�ߴ���أ�����ͼ��Ŵ�ļ��㷽�� 2024.2.5
* ע�ͣ�
*********************************************************************************************/
#include "screen.h"
#include "ili9806.h"

/*********************************************************************************************
* ��Ļ��ʾ�洢����ȫ�ֱ���
* ����ͼƬC�ļ�ʹ��emwin BmpCvtST.exe������ɣ��򿪺󱣴�ΪC��ʽ��
* ע��:
*       1��ѡ�������ʽ����Ϊ��High color (565] red and blue swapped
*       2����Ҫ�޸���������ɵ�C�ļ����������޸ģ�ÿ��ͼƬ��������������
*       ǰ����Ԫ�ر�ʾͼƬ���ؿ�Ⱥ͸߶ȣ���Ҫ�Լ��ֶ����䡣
*********************************************************************************************/
extern uint16_t _actotal_switch_on[];
extern uint16_t _actotal_switch_off[];
extern uint16_t _acwashing_logo[];
extern uint16_t _acsoaking[];
extern uint16_t _acwashing[];
extern uint16_t _acrinseing[];
extern uint16_t _acdehydration[];
extern uint16_t _acall_mode[];
extern uint16_t _acs_num_0[];
extern uint16_t _acs_num_1[];
extern uint16_t _acs_num_2[];
extern uint16_t _acs_num_3[];
extern uint16_t _acs_num_4[];
extern uint16_t _acs_num_5[];
extern uint16_t _acs_num_6[];
extern uint16_t _acs_num_7[];
extern uint16_t _acs_num_8[];
extern uint16_t _acs_num_9[];
extern uint16_t _acb_num_0[];
extern uint16_t _acb_num_1[];
extern uint16_t _acb_num_2[];
extern uint16_t _acb_num_3[];
extern uint16_t _acb_num_4[];
extern uint16_t _acb_num_5[];
extern uint16_t _acb_num_6[];
extern uint16_t _acb_num_7[];
extern uint16_t _acb_num_8[];
extern uint16_t _acb_num_9[];
extern uint16_t _acb_point[]; 
extern uint16_t _acsoaking_1[];
extern uint16_t _acwashing_1[];
extern uint16_t _acrinseing_1[];
extern uint16_t _acdehydration_1[];
extern uint16_t _acall_mode_1[];
extern uint16_t _acamount_of_water[];
extern uint16_t _acunit[];

/*********************************************************************************************
* ����ͼ����ʾλ��
*********************************************************************************************/
/* �Ŵ��� */
#define MAGNIFICATION       2                                   //ͼ��ķŴ���

/* �̶�ͼ����ʾλ�� */
#define WASHING_LOGO_X      ((int)(LCD_WIDTH*0.8f))             //����Ӧ��LOGOͼ��X����
#define WASHING_LOGO_Y      ((int)(LCD_HEIGHT*0.0625f))

#define ICON_WIDTH          ((int)(28*MAGNIFICATION))           //28��ʾͼ������ؿ��
#define ICON_SPACING        ((int)((LCD_WIDTH-5*ICON_WIDTH)/6)) //�Զ����㹦��ͼ��֮��ļ����5��ͼ�꣬6�����
#define SOAKING_1_X         ((int)(ICON_WIDTH*0+ICON_SPACING*1))//����"����"����ͼ��X����
#define SOAKING_1_Y         ((int)(LCD_HEIGHT*0.73f))

#define WASHING_1_X         ((int)(ICON_WIDTH*1+ICON_SPACING*2))//����"ϴ��"����ͼ��X����
#define WASHING_1_Y         SOAKING_1_Y

#define RINSEING_1_X        ((int)(ICON_WIDTH*2+ICON_SPACING*3))//����"��ϴ"����ͼ��X����
#define RINSEING_1_Y        SOAKING_1_Y

#define DEHYDRATION_1_X     ((int)(ICON_WIDTH*3+ICON_SPACING*4))//����"��ˮ"����ͼ��X����
#define DEHYDRATION_1_Y     SOAKING_1_Y

#define ALLMODE_1_X         ((int)(ICON_WIDTH*4+ICON_SPACING*5-26))//����"ȫӦ��"����ͼ��X����
#define ALLMODE_1_Y         SOAKING_1_Y

#define AMOUNT_OF_WATER_X   ((int)(LCD_WIDTH*0.73f))            //����"ˮ��"����ͼ��X����
#define AMOUNT_OF_WATER_Y   ((int)(LCD_HEIGHT*0.86f))

#define UNIT_X              ((int)(LCD_WIDTH*0.91f))            //���õ�λ"L"ͼ��X����
#define UNIT_Y              AMOUNT_OF_WATER_Y

/* ģʽ��̬ͼ����ʾλ�� */
#define TOTAL_SWITCH_X      ((int)(LCD_WIDTH*0.03f))            //���ÿ���ͼ��X����
#define TOTAL_SWITCH_Y      ((int)(LCD_HEIGHT*0.03f))

#define SOAKING_X           SOAKING_1_X                         //����"����"ͼ��X����
#define SOAKING_Y           (SOAKING_1_Y-35*MAGNIFICATION)

#define WASHING_X           WASHING_1_X                         //����"ϴ��"ͼ��X����
#define WASHING_Y           SOAKING_Y-15

#define RINSEING_X          RINSEING_1_X                        //����"��ϴ"ͼ��X����
#define RINSEING_Y          SOAKING_Y

#define DEHYDRATION_X       DEHYDRATION_1_X                     //����"��ˮ"ͼ��X����
#define DEHYDRATION_Y       SOAKING_Y+10

#define ALLMODE_X           ((int)(ICON_WIDTH*4+ICON_SPACING*5))//����"ȫ����"ͼ��X����
#define ALLMODE_Y           SOAKING_Y

/* ʱ���Һλ��̬ͼ����ʾλ�� */
#define TIMER_H_X           ((int)(LCD_WIDTH*0.3f))             //����ʱ��"Сʱ"ͼ��X����
#define TIMER_H_Y           ((int)(LCD_HEIGHT*0.241f))

#define TIMER_MH_X          ((int)(LCD_WIDTH*0.49f))            //����ʱ��"����"�ĸ�λͼ��X����
#define TIMER_MH_Y          TIMER_H_Y

#define TIMER_ML_X          ((int)(LCD_WIDTH*0.625f))           //����ʱ��"����"�ĵ�λͼ��X����
#define TIMER_ML_Y          TIMER_H_Y

#define TIMER_POINT_X       ((int)(LCD_WIDTH*0.43f))            //����ʱ��"��"��ͼ��X����
#define TIMER_POINT_Y       ((int)(LCD_HEIGHT*0.275f))

#define LEVEL_NUM_H_X       ((int)(LCD_WIDTH*0.82f))            //����ˮ����λͼ��X����
#define LEVEL_NUM_H_Y       AMOUNT_OF_WATER_Y

#define LEVEL_NUM_L_X       ((int)(LCD_WIDTH*0.87f))            //����ˮ����λͼ��X����
#define LEVEL_NUM_L_Y       LEVEL_NUM_H_Y

/*********************************************************************************************
* ���ƣ�show_logo
* ���ܣ���ʾ��¼����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void show_logo(void)
{
  /* ��ʾlogoͼ�� */
  LCD_FillData_ex(WASHING_LOGO_X, WASHING_LOGO_Y, _acwashing_logo[0], _acwashing_logo[1], _acwashing_logo ,MAGNIFICATION);

  /* ��ʾ�������� */
  LCD_FillData_ex(SOAKING_1_X, SOAKING_1_Y, _acsoaking_1[0], _acsoaking_1[1], _acsoaking_1 ,MAGNIFICATION);
  LCD_FillData_ex(WASHING_1_X, WASHING_1_Y, _acwashing_1[0], _acwashing_1[1], _acwashing_1 ,MAGNIFICATION);
  LCD_FillData_ex(RINSEING_1_X, RINSEING_1_Y, _acrinseing_1[0], _acrinseing_1[1], _acrinseing_1 ,MAGNIFICATION);
  LCD_FillData_ex(DEHYDRATION_1_X, DEHYDRATION_1_Y, _acdehydration_1[0], _acdehydration_1[1], _acdehydration_1 ,MAGNIFICATION);
  LCD_FillData_ex(ALLMODE_1_X, ALLMODE_1_Y, _acall_mode_1[0], _acall_mode_1[1], _acall_mode_1 ,MAGNIFICATION);

  /* ��ʾˮ���͵�λͼ�� */
  LCD_FillData_ex(AMOUNT_OF_WATER_X, AMOUNT_OF_WATER_Y, _acamount_of_water[0], _acamount_of_water[1], _acamount_of_water ,MAGNIFICATION);
  LCD_FillData_ex(UNIT_X, UNIT_Y, _acunit[0], _acunit[1], _acunit ,MAGNIFICATION);
}

/*********************************************************************************************
* ���ƣ�show_switch
* ���ܣ���ʾ����״̬
* ������status ״̬
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void show_switch(uint8_t status)
{
  if(!status) LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y, _actotal_switch_on[0], _actotal_switch_on[1], _actotal_switch_on, MAGNIFICATION);
  else LCD_FillData_ex(TOTAL_SWITCH_X, TOTAL_SWITCH_Y,  _actotal_switch_off[0], _actotal_switch_off[1], _actotal_switch_off, MAGNIFICATION);
}

/*********************************************************************************************
* ���ƣ�show_soak()
* ���ܣ���ʾ����ͼ��
* ������status ��ʾ״̬
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void show_soak(uint8_t status)
{
  uint16_t x, y;
  x = SOAKING_X;
  y = SOAKING_Y;
  if(!status) LCD_FillData_ex(x, y, _acsoaking[0], _acsoaking[1], _acsoaking, MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acsoaking[0]*MAGNIFICATION, y + _acsoaking[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* ���ƣ�show_wash()
* ���ܣ���ʾϴ��ͼ��
* ������status ��ʾ״̬
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void show_wash(uint8_t status)
{
  uint16_t x, y;
  x = WASHING_X;
  y = WASHING_Y;
  if(!status) LCD_FillData_ex(x, y, _acwashing[0], _acwashing[1], _acwashing, MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acwashing[0]*MAGNIFICATION, y + _acwashing[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* ���ƣ�show_rinse()
* ���ܣ���ʾ��ϴͼ��
* ������status ��ʾ״̬
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void show_rinse(uint8_t status)
{
  uint16_t x, y;
  x = RINSEING_X;
  y = RINSEING_Y;
  if(!status) LCD_FillData_ex(x, y, _acrinseing[0], _acrinseing[1], _acrinseing, MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acrinseing[0]*MAGNIFICATION, y + _acrinseing[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* ���ƣ�show_dehyd()
* ���ܣ���ʾ��ˮͼ��
* ������status ��ʾ״̬
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void show_dehyd(uint8_t status)
{
  uint16_t x, y;
  x = DEHYDRATION_X;
  y = DEHYDRATION_Y;
  if(!status) LCD_FillData_ex(x, y, _acdehydration[0], _acdehydration[1], _acdehydration, MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acdehydration[0]*MAGNIFICATION, y + _acdehydration[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* ���ƣ�show_all_mode()
* ���ܣ���ʾȫ����ͼ��
* ������status ��ʾ״̬
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void show_all_mode(uint8_t status)
{
  uint16_t x, y;
  x = ALLMODE_X;
  y = ALLMODE_Y;
  if(!status) LCD_FillData_ex(x, y, _acall_mode[0], _acall_mode[1], _acall_mode, MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acall_mode[0]*MAGNIFICATION, y + _acall_mode[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* ���ƣ�show_b_num
* ���ܣ���ʾ��ŵ�����
* ������x����ʾ��X��ʼ���� y����ʾ��Y��ʼ���� num:Ҫ��ʾ����ֵ
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void show_b_num(uint16_t x, uint16_t y, uint8_t num)
{
  LCD_FillColor(x, y, x + 31*MAGNIFICATION, y + 52*MAGNIFICATION, BLACK);
  switch(num){
  case 0:
    LCD_FillData_ex(x, y, _acb_num_0[0], _acb_num_0[1], _acb_num_0 ,MAGNIFICATION);
    break;
  case 1:
    LCD_FillData_ex(x + 23*MAGNIFICATION, y, _acb_num_1[0], _acb_num_1[1], _acb_num_1 ,MAGNIFICATION);
    break;
  case 2:
    LCD_FillData_ex(x, y, _acb_num_2[0], _acb_num_2[1], _acb_num_2 ,MAGNIFICATION);
    break;
  case 3:
    LCD_FillData_ex(x, y, _acb_num_3[0], _acb_num_3[1], _acb_num_3 ,MAGNIFICATION);
    break;
  case 4:
    LCD_FillData_ex(x, y, _acb_num_4[0], _acb_num_4[1], _acb_num_4 ,MAGNIFICATION);
    break;
  case 5:
    LCD_FillData_ex(x, y, _acb_num_5[0], _acb_num_5[1], _acb_num_5 ,MAGNIFICATION);
    break;
  case 6:
    LCD_FillData_ex(x, y, _acb_num_6[0], _acb_num_6[1], _acb_num_6 ,MAGNIFICATION);
    break;
  case 7:
    LCD_FillData_ex(x, y, _acb_num_7[0], _acb_num_7[1], _acb_num_7 ,MAGNIFICATION);
    break;
  case 8:
    LCD_FillData_ex(x, y, _acb_num_8[0], _acb_num_8[1], _acb_num_8 ,MAGNIFICATION);
    break;
  case 9:
    LCD_FillData_ex(x, y, _acb_num_9[0], _acb_num_9[1], _acb_num_9 ,MAGNIFICATION);
    break;
  }
}

/*********************************************************************************************
* ���ƣ�show_s_num
* ���ܣ���ʾС�ŵ�����
* ������x����ʾ��X��ʼ���� y����ʾ��Y��ʼ���� num:Ҫ��ʾ����ֵ
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
static void show_s_num(uint16_t x, uint16_t y, uint8_t num)
{
  LCD_FillColor(x, y, x + 13*MAGNIFICATION, y + 20*MAGNIFICATION, BLACK);
  switch(num){
  case 0:
    LCD_FillData_ex(x, y, _acs_num_0[0], _acs_num_0[1], _acs_num_0 ,MAGNIFICATION);
    break;
  case 1:
    LCD_FillData_ex(x + 9*MAGNIFICATION, y, _acs_num_1[0], _acs_num_1[1], _acs_num_1 ,MAGNIFICATION);
    break;
  case 2:
    LCD_FillData_ex(x, y, _acs_num_2[0], _acs_num_2[1], _acs_num_2 ,MAGNIFICATION);
    break;
  case 3:
    LCD_FillData_ex(x, y, _acs_num_3[0], _acs_num_3[1], _acs_num_3 ,MAGNIFICATION);
    break;
  case 4:
    LCD_FillData_ex(x, y, _acs_num_4[0], _acs_num_4[1], _acs_num_4 ,MAGNIFICATION);
    break;
  case 5:
    LCD_FillData_ex(x, y, _acs_num_5[0], _acs_num_5[1], _acs_num_5 ,MAGNIFICATION);
    break;
  case 6:
    LCD_FillData_ex(x, y, _acs_num_6[0], _acs_num_6[1], _acs_num_6 ,MAGNIFICATION);
    break;
  case 7:
    LCD_FillData_ex(x, y, _acs_num_7[0], _acs_num_7[1], _acs_num_7 ,MAGNIFICATION);
    break;
  case 8:
    LCD_FillData_ex(x, y, _acs_num_8[0], _acs_num_8[1], _acs_num_8 ,MAGNIFICATION);
    break;
  case 9:
    LCD_FillData_ex(x, y, _acs_num_9[0], _acs_num_9[1], _acs_num_9 ,MAGNIFICATION);
    break;
  }
}

/*********************************************************************************************
* ���ƣ�show_timer
* ���ܣ���ʾʣ��ʱ��
* ������hours��ʱ minute����
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void show_timer(uint8_t hours, uint8_t minute)
{
  uint8_t mh, ml;
  static uint8_t l_h, l_mh, l_ml;
  
  mh = minute / 10;
  ml = minute % 10;
  
  if((l_h != hours) || (hours == 0)) show_b_num(TIMER_H_X, TIMER_H_Y, hours);
  if((l_mh != mh) || (mh == 0))show_b_num(TIMER_MH_X, TIMER_MH_Y, mh);
  if((l_ml != ml) || (ml == 0))show_b_num(TIMER_ML_X, TIMER_ML_Y, ml);
  
  l_h = hours;
  l_mh = mh;
  l_ml = ml;
}

/*********************************************************************************************
* ���ƣ�show_point
* ���ܣ�ʵ��ʱ�ͷ�֮��ĵ�
* ������status ��ʾ����
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void show_point(uint8_t status)
{
  uint16_t x, y;
  x = TIMER_POINT_X;
  y = TIMER_POINT_Y;
  if(!status) LCD_FillData_ex(x, y, _acb_point[0], _acb_point[1], _acb_point ,MAGNIFICATION);
  else LCD_FillColor(x, y, x + _acb_point[0]*MAGNIFICATION, y + _acb_point[1]*MAGNIFICATION, BLACK);
}

/*********************************************************************************************
* ���ƣ�show_level
* ���ܣ���ʾҺλ
* ������level ʵʱҺλ
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void show_level(uint8_t level)
{
  uint8_t lh, ll;
  static uint8_t l_lh, l_ll;
  
  lh = level / 10;
  ll = level % 10;
  
  if((l_lh != lh) || (lh == 0)) show_s_num(LEVEL_NUM_H_X, LEVEL_NUM_H_Y, lh);
  if((l_ll != ll) || (lh == 0)) show_s_num(LEVEL_NUM_L_X, LEVEL_NUM_L_Y, ll);
  
  l_lh = lh;
  l_ll = ll;
}
