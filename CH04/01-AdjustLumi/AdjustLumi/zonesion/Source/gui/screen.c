/*********************************************************************************************
* �ļ�: screen.c
* ���ߣ�zonesion 2016.12.22
* ˵����
* ���ܣ���Ļ��������
* �޸ģ�wanghao  ������غ궨�壬������ֲ����������ö�٣�����ͼ��Ŵ�ӿ�      2024.2.2
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
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
extern uint16_t _aclibrary_logo[];
extern uint16_t _acmode_handler_on[];
extern uint16_t _acmode_handler_off[];
extern uint16_t _acmode_smart_on[];
extern uint16_t _acmode_smart_off[];
extern uint16_t _acnum_0[];
extern uint16_t _acnum_1[];
extern uint16_t _acnum_2[];
extern uint16_t _acnum_3[];
extern uint16_t _acnum_4[];
extern uint16_t _acnum_5[];
extern uint16_t _acnum_6[];
extern uint16_t _acnum_7[];
extern uint16_t _acnum_8[];
extern uint16_t _acnum_9[];
extern uint16_t _acLX[]; 
extern uint16_t _acKLX[];
extern uint16_t _acfill_light_0[];
extern uint16_t _acfill_light_1[];
extern uint16_t _acfill_light_2[];
extern uint16_t _acfill_light_3[];
extern uint16_t _acfill_light_4[];

/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define LIBRARY_LOGO_X            ((int)(LCD_WIDTH*0.8f))       // ͼ���ͼ����ʼ����X
#define LIBRARY_LOGO_Y            ((int)(LCD_HEIGHT*0.1f))
  
#define LX_UNIT_X                 ((int)(LCD_WIDTH*0.85f))      // ���նȵ�λͼ����ʼ����X
#define LX_UNIT_Y                 ((int)(LCD_HEIGHT*0.55f))
  
#define FILL_LIGHT_X              ((int)(LCD_WIDTH*0.1f))       // ����ͼ����ʼ����X
#define FILL_LIGHT_Y              ((int)(LCD_HEIGHT*0.1f))
  
#define POSTNUM_X                 ((int)(LCD_WIDTH*0.6f))       // ������ʾ��ʼ����X
#define POSTNUM_Y                 ((int)(LCD_HEIGHT*0.5f))
#define MAGNIFICATION             2                             // �Ŵ���
#define POST_NUM_WIDTH            (21*MAGNIFICATION)            // һ������ռ�õĴ������ؼ�����������ּ�ļ��

#define MODE_MANUAL_X             ((int)(LCD_WIDTH*0.15f))      // �ֶ�ģʽ��ʼ����X
#define MODE_MANUAL_Y             ((int)(LCD_HEIGHT*0.8f))
#define MODE_AUTO_X               ((int)(LCD_WIDTH*0.65f))      // �Զ�ģʽ��ʼ����X
#define MODE_AUTO_Y               ((int)(LCD_HEIGHT*0.8f))

/*********************************************************************************************
* ö��
*********************************************************************************************/
typedef enum {                                                  // ��ֵ
  POST_NUM_0 = 0,
  POST_NUM_1,
  POST_NUM_2,
  POST_NUM_3,
  POST_NUM_4,
  POST_NUM_5,
  POST_NUM_6,
  POST_NUM_7,
  POST_NUM_8,
  POST_NUM_9,
  POST_NONE,
}post_num;

typedef enum {                                                  // ������λ
  INDIVIDUAL = 1,
  TEN,
  HUNDRED,
  THOUSAND,
  MYRIAD,
}post_unit;

/*********************************************************************************************
* ����:show_logo
* ����:��ʾ�����еľ�̬ͼ��
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void show_logo(void)
{
  LCD_FillData_ex(LIBRARY_LOGO_X, LIBRARY_LOGO_Y, _aclibrary_logo[0], _aclibrary_logo[1], _aclibrary_logo,2);
  LCD_FillData_ex(LX_UNIT_X, LX_UNIT_Y, _acLX[0], _acLX[1], _acLX,2);
}

/*********************************************************************************************
* ����:show_fill_light
* ����:��ʾ�����е�fill_lightͼ��
* ����:status ͼ�������
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void show_fill_light(uint8_t status)
{
  switch(status){
  case 0:
    LCD_FillData_ex(FILL_LIGHT_X,FILL_LIGHT_Y,_acfill_light_0[0],_acfill_light_0[1], _acfill_light_0,2);
    break;
  case 1:
    LCD_FillData_ex(FILL_LIGHT_X,FILL_LIGHT_Y,_acfill_light_1[0],_acfill_light_1[1], _acfill_light_1,2);
    break;
  case 2:
    LCD_FillData_ex(FILL_LIGHT_X,FILL_LIGHT_Y,_acfill_light_2[0],_acfill_light_2[1], _acfill_light_2,2);
    break;
  case 3:
    LCD_FillData_ex(FILL_LIGHT_X,FILL_LIGHT_Y,_acfill_light_3[0],_acfill_light_3[1], _acfill_light_3,2);
    break;
  case 4:
    LCD_FillData_ex(FILL_LIGHT_X,FILL_LIGHT_Y,_acfill_light_4[0],_acfill_light_4[1], _acfill_light_4,2);
    break;
  }
}

/*********************************************************************************************
* ����:show_post_num
* ����:��ָ��������λλ������ʾһ����ֵ
* ����:post     ��ʾλ�ã������λ��ʮλ
*      x        ��ʾ��ʼλ��
*      y        ��ʾ��ʼλ��
*      num      ������ֵ
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
static void show_post_num(post_unit post, uint16_t x, uint16_t y, post_num num)
{
  /* ���㵱ǰ������LCD����ʾλ�� */
  uint16_t dx = x + (5 - post) * POST_NUM_WIDTH;
  uint16_t dy = y;
  /* ����ͼ�������ֵ�ͷŴ���������������ʾ���� */
  LCD_FillColor(dx, dy-1, dx + 19*MAGNIFICATION, dy + 32*MAGNIFICATION, BLACK);
  switch(num){
  case POST_NUM_0:
    LCD_FillData_ex(dx, dy,_acnum_0[0],_acnum_0[1], _acnum_0,MAGNIFICATION);
    break;
  case POST_NUM_1:
    LCD_FillData_ex(dx + 15*MAGNIFICATION, y, _acnum_1[0],_acnum_1[1], _acnum_1,MAGNIFICATION);
    break;
  case POST_NUM_2:
    LCD_FillData_ex(dx, dy, _acnum_2[0],_acnum_2[1], _acnum_2,MAGNIFICATION);
    break;
  case POST_NUM_3:
    LCD_FillData_ex(dx + 1*MAGNIFICATION, dy, _acnum_3[0],_acnum_3[1], _acnum_3,MAGNIFICATION);
    break;
  case POST_NUM_4:
    LCD_FillData_ex(dx, dy, _acnum_4[0],_acnum_4[1], _acnum_4,MAGNIFICATION);
    break;
  case POST_NUM_5:
    LCD_FillData_ex(dx + 1*MAGNIFICATION, dy,_acnum_5[0],_acnum_5[1], _acnum_5,MAGNIFICATION);
    break;
  case POST_NUM_6:
    LCD_FillData_ex(dx, dy,_acnum_6[0],_acnum_6[1], _acnum_6,MAGNIFICATION);
    break;
  case POST_NUM_7:
    LCD_FillData_ex(dx, dy,_acnum_7[0],_acnum_7[1], _acnum_7,MAGNIFICATION);
    break;
  case POST_NUM_8:
    LCD_FillData_ex(dx, dy,_acnum_8[0],_acnum_8[1], _acnum_8,MAGNIFICATION);
    break;
  case POST_NUM_9:
    LCD_FillData_ex(dx, dy,_acnum_9[0],_acnum_9[1], _acnum_9,MAGNIFICATION);
    break;
  /* �������ֲ�������ʾ */
  default:
    break;
  }
}

/*********************************************************************************************
* ����:show_value
* ����:��LCD����ʾһ������
* ����:value    Ҫ��ʾ�ľ�����ֵ
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void show_value(uint16_t value)
{
  /* ʵ��Ϊ9����λ��ʾ0����������ʾ */
  if(value == 0){
    show_post_num(INDIVIDUAL, POSTNUM_X, POSTNUM_Y, POST_NUM_0);
    show_post_num(TEN, POSTNUM_X, POSTNUM_Y, POST_NONE);
    show_post_num(HUNDRED, POSTNUM_X, POSTNUM_Y, POST_NONE);
    show_post_num(THOUSAND, POSTNUM_X, POSTNUM_Y, POST_NONE);
    show_post_num(MYRIAD, POSTNUM_X, POSTNUM_Y, POST_NONE);
    return;
  }
  /* ��λ */
  if(value) show_post_num(INDIVIDUAL, POSTNUM_X, POSTNUM_Y, (post_num)(value % 10));
  else show_post_num(INDIVIDUAL, POSTNUM_X, POSTNUM_Y, POST_NONE);
  /* ʮλ */
  if(value / 10) show_post_num(TEN, POSTNUM_X, POSTNUM_Y, (post_num)((value % 100)/10));
  else show_post_num(TEN, POSTNUM_X, POSTNUM_Y, POST_NONE);
  /* ��λ */
  if(value / 100) show_post_num(HUNDRED, POSTNUM_X, POSTNUM_Y, (post_num)((value % 1000)/100));
  else show_post_num(HUNDRED, POSTNUM_X, POSTNUM_Y, POST_NONE);
  /* ǧλ */
  if(value / 1000) show_post_num(THOUSAND, POSTNUM_X, POSTNUM_Y, (post_num)((value % 10000)/1000));
  else show_post_num(THOUSAND, POSTNUM_X, POSTNUM_Y, POST_NONE);
  /* ��λ */
  if(value / 10000) show_post_num(MYRIAD, POSTNUM_X, POSTNUM_Y, (post_num)(value / 10000));
  else show_post_num(MYRIAD, POSTNUM_X, POSTNUM_Y, POST_NONE);
}

/*********************************************************************************************
* ����:show_mode
* ����:��LCD����ʾ��ǰģʽ
* ����:status     ��ǰģʽ
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void show_mode(uint8_t status)
{
  if(status){
    /* ��ʾ�ֶ�ģʽ */
    LCD_FillData_ex(MODE_MANUAL_X, MODE_MANUAL_Y, _acmode_handler_on[0], _acmode_handler_on[1], _acmode_handler_on,2);
    LCD_FillData_ex(MODE_AUTO_X, MODE_AUTO_Y,_acmode_smart_off[0], _acmode_smart_off[1], _acmode_smart_off,2);
  }else{
    /* ��ʾ�Զ�ģʽ */
    LCD_FillData_ex(MODE_MANUAL_X, MODE_MANUAL_Y, _acmode_handler_off[0], _acmode_handler_off[1], _acmode_handler_off,2);
    LCD_FillData_ex(MODE_AUTO_X, MODE_AUTO_Y,_acmode_smart_on[0], _acmode_smart_on[1], _acmode_smart_on,2);
  }
}