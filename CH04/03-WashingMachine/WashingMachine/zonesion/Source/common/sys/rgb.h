/*********************************************************************************************
* �ļ�: rgb.c
* ���ߣ�liutong 2015.8.19
* ˵����
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#ifndef _RGB_H_
#define _RGB_H_
/*********************************************************************************************
*ͷ�ļ�
**********************************************************************************************/
#include "extend.h"
/*********************************************************************************************
*�궨��
**********************************************************************************************/
#define LEDR    0X10                                            //�궨�������ֱ��
#define LEDG    0X20                                            //�궨���̵����ֱ��
#define LEDB    0X40                                            //�궨���������ֱ��

#define ON      0
#define OFF     1

#define DR(a)                   if(a == ON) GPIO_WriteBit(RGB_R_GPIO, RGB_R_PIN, Bit_RESET);  \
                                else  GPIO_WriteBit(RGB_R_GPIO, RGB_R_PIN, Bit_SET)

#define DG(a)                   if(a == ON) GPIO_WriteBit(RGB_G_GPIO, RGB_G_PIN, Bit_RESET);  \
                                else  GPIO_WriteBit(RGB_G_GPIO, RGB_G_PIN, Bit_SET)

#define DB(a)                   if(a == ON) GPIO_WriteBit(RGB_B_GPIO, RGB_B_PIN, Bit_RESET);  \
                                else  GPIO_WriteBit(RGB_B_GPIO, RGB_B_PIN, Bit_SET)
                                  
/*********************************************************************************************
*��������
**********************************************************************************************/
void rgb_init(void);                                            //RGB���ų�ʼ��
void rgb_turn_on(unsigned char rgb);                            //��RGB����
void rgb_turn_off(unsigned char rgb);                           //��RGB����
unsigned char get_rgb_status(void);                             //��ȡRGB��ǰ��״̬
void rgb_ctrl(uint8_t cfg);                                     //rgb���ƺ���
#endif 

