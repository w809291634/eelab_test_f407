#ifndef __KEY_H__
#define __KEY_H__
/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "extend.h"

/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define KEY_PORT                GPIOB                           //�궨�尴���˿�ΪGPIOB
#define KEY_CLK                 RCC_AHB1Periph_GPIOB            //�궨�尴��ʱ��ΪRCC_AHB1Periph_GPIOB 
#define K1_PIN                  GPIO_Pin_12                     //�궨��K1�ܽ�ΪGPIO_Pin_12
#define K2_PIN                  GPIO_Pin_13                     //�궨��K2�ܽ�ΪGPIO_Pin_13
#define K3_PIN                  GPIO_Pin_14                     //�궨��K3�ܽ�ΪGPIO_Pin_14
#define K4_PIN                  GPIO_Pin_15                     //�궨��K4�ܽ�ΪGPIO_Pin_15

#define KEY1                    PBin(12)
#define KEY2                    PBin(13)
#define KEY3                    PBin(14)
#define KEY4                    PBin(15)

#define KEY_DOWN                0                               //�궨�尴������״̬ΪDOWN
#define KEY_UP                  1                               //�궨�尴���ɿ�״̬ΪUP

#define K1_PRES                 0x01                            //�궨��K1���ֱ��
#define K2_PRES                 0x02                            //�궨��K2���ֱ��
#define K3_PRES                 0x04                            //�궨��K3���ֱ��
#define K4_PRES                 0x08                            //�궨��K4���ֱ��

#define K1                      K1_PRES
#define K2                      K2_PRES
#define K3                      K3_PRES
#define K4                      K4_PRES

/*********************************************************************************************
* ��������
*********************************************************************************************/
void key_init(void);                                            //������ʼ������
char get_key_status(void);                                      //��ȡ����������״̬
signed char key_status(char num);                               //����״̬��ѯ����
u8 KEY_Scan(u8 mode);                                           //����״̬ɨ�躯��
#endif /*__KEY_H_*/