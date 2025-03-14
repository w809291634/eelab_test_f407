/*********************************************************************************************
* �ļ�: led.c
* ���ߣ�liutong 2015.8.19
* ˵����RGB_R->PG8,
        RGB_G->PG7,
        RGB_B->PG6
* ���ܣ�RGB��������
* �޸ģ�
*********************************************************************************************/
/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "rgb.h"

/*********************************************************************************************
* ָ�����ļ����������ò���
*********************************************************************************************/
#define RGB_R_RCC       RCC_AHB1Periph_GPIOG
#define RGB_G_RCC       RCC_AHB1Periph_GPIOG
#define RGB_B_RCC       RCC_AHB1Periph_GPIOG

#define RGB_R_GPIO      GPIOG
#define RGB_G_GPIO      GPIOG
#define RGB_B_GPIO      GPIOG

#define RGB_R_PIN       GPIO_Pin_8
#define RGB_G_PIN       GPIO_Pin_7
#define RGB_B_PIN       GPIO_Pin_6

/*********************************************************************************************
* ���ƣ�rgb_init
* ���ܣ���ʼ��RGB��Ӧ��GPIO
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void rgb_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};                    
  
  RCC_AHB1PeriphClockCmd(RGB_R_RCC | RGB_G_RCC |
                         RGB_B_RCC , ENABLE);                   //ʹ��GPIOʱ��
  
  GPIO_InitStructure.GPIO_Pin = RGB_R_PIN;                      //ѡ������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //����ٶ�2MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //��������
  GPIO_Init(RGB_R_GPIO, &GPIO_InitStructure);                   //����������������
  
  GPIO_InitStructure.GPIO_Pin = RGB_G_PIN ;                     //ѡ������
  GPIO_Init(RGB_G_GPIO, &GPIO_InitStructure);                   //����������������
  
  GPIO_InitStructure.GPIO_Pin = RGB_B_PIN ;                     //ѡ������
  GPIO_Init(RGB_B_GPIO, &GPIO_InitStructure);                   //����������������
  
  GPIO_SetBits(RGB_R_GPIO, RGB_R_PIN );
  GPIO_SetBits(RGB_G_GPIO, RGB_G_PIN );
  GPIO_SetBits(RGB_B_GPIO, RGB_B_PIN );
}

/*********************************************************************************************
* ���ƣ�turn_off
* ���ܣ������Ÿߵ�ƽ���رյ�
* ������rgb
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void rgb_turn_off(unsigned char rgb){
  if(rgb & LEDR)
    GPIO_SetBits(RGB_R_GPIO, RGB_R_PIN);                        //�����Ÿߵ�ƽ,�ر�
  if(rgb & LEDG)
    GPIO_SetBits(RGB_G_GPIO, RGB_G_PIN);
  if(rgb & LEDB)
    GPIO_SetBits(RGB_B_GPIO, RGB_B_PIN);
}

/*********************************************************************************************
* ���ƣ�turn_on
* ���ܣ������ŵ͵�ƽ���򿪵�
* ������rgb
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void rgb_turn_on(unsigned char rgb){
  if(rgb & LEDR)
    GPIO_ResetBits(RGB_R_GPIO, RGB_R_PIN);                      //�����ŵ͵�ƽ���򿪵�
  if(rgb & LEDG)
    GPIO_ResetBits(RGB_G_GPIO, RGB_G_PIN);
  if(rgb & LEDB)
    GPIO_ResetBits(RGB_B_GPIO, RGB_B_PIN);
}

/*********************************************************************************************
* ���ƣ�get_rgb_status
* ���ܣ���ȡ�Ƶ�״̬
* ������
* ���أ�status����λ�ֱ��ʾ�Ƶ�״̬
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_rgb_status(void){
  unsigned char status = 0;
  if(GPIO_ReadOutputDataBit(RGB_R_GPIO, RGB_R_PIN))
    status |= LEDR;
  else
    status &= ~LEDR;
  
  if(GPIO_ReadOutputDataBit(RGB_G_GPIO, RGB_G_PIN))
    status |= LEDG; 
  else
    status &= ~LEDG;
  
  if(GPIO_ReadOutputDataBit(RGB_B_GPIO, RGB_B_PIN))
    status |= LEDB;
  else
    status &= ~LEDB;
  return status;                                                //����status
}

/*********************************************************************************************
* ���ƣ�void rgb_ctrl(uint8_t cfg)
* ���ܣ�RGB���ƺ���
* ������cfg:RGB�Ŀ���λ������λ�ֱ����һյ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void rgb_ctrl(uint8_t cfg)
{
  uint8_t set1, set2, set3;
  set1 = cfg & 0x01;
  set2 = (cfg & 0x02) >> 1;
  set3 = (cfg & 0x04) >> 2;
  
  DR(!set1);
  DG(!set2);
  DB(!set3);
}
