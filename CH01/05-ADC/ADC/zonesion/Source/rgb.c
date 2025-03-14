/*********************************************************************************************
* �ļ�: led.c
* ���ߣ�liutong 2015.8.19
* ˵����
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "rgb.h"
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
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC |
                         RCC_AHB1Periph_GPIOD , ENABLE);        //ʹ��GPIOʱ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;                     //ѡ������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //����ٶ�2MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);                        //����������������
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 ;                    //ѡ������
  GPIO_Init(GPIOC, &GPIO_InitStructure);                        //����������������
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;                    //ѡ������
  GPIO_Init(GPIOD, &GPIO_InitStructure);                        //����������������
  
  GPIO_SetBits(GPIOA, GPIO_Pin_4 );
  GPIO_SetBits(GPIOC, GPIO_Pin_5 );
  GPIO_SetBits(GPIOD, GPIO_Pin_6 );
}

/*********************************************************************************************
* ���ƣ�turn_off
* ���ܣ������Ÿߵ�ƽ���ر�rgb
* ������rgb
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void turn_off(unsigned char rgb){
  if(rgb & LEDR)
    GPIO_SetBits(GPIOA, GPIO_Pin_4);                            //�����Ÿߵ�ƽ���ر�RGB�Ƶĺ��
  if(rgb & LEDG)
    GPIO_SetBits(GPIOC, GPIO_Pin_5);                            //�����Ÿߵ�ƽ���ر�RGB�Ƶ��̵�
  if(rgb & LEDB)
    GPIO_SetBits(GPIOD, GPIO_Pin_6);                            //�����Ÿߵ�ƽ���ر�RGB�Ƶ�����
}

/*********************************************************************************************
* ���ƣ�turn_on
* ���ܣ������ŵ͵�ƽ����rgb
* ������rgb
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void turn_on(unsigned char rgb){
  if(rgb & LEDR)
    GPIO_ResetBits(GPIOA, GPIO_Pin_4);                          //�����ŵ͵�ƽ����RGB�Ƶĺ��
  if(rgb & LEDG)
    GPIO_ResetBits(GPIOC, GPIO_Pin_5);                          //�����ŵ͵�ƽ����RGB�Ƶ��̵�
  if(rgb & LEDB)
    GPIO_ResetBits(GPIOD, GPIO_Pin_6);                          //�����ŵ͵�ƽ����RGB�Ƶ�����
}
/*********************************************************************************************
* ���ƣ�get_rgb_status
* ���ܣ���ȡLED״̬
* ������
* ���أ�rgb_status��bit4-bit6�ֱ��ʾRGB�Ƶ�״̬
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_rgb_status(void){
  unsigned char rgb_status = 0;
  if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4))
    rgb_status |= LEDR;                                         //�ߵ�ƽ��rgb_status bit4��1
  else
    rgb_status &= ~LEDR;                                        //�͵�ƽ��rgb_status bit4��0
  
  if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_5))
    rgb_status |= LEDG;                                         //�ߵ�ƽ��rgb_status bit5��1
  else
    rgb_status &= ~LEDG;                                        //�͵�ƽ��rgb_status bit5��0
  
  if(GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6))
    rgb_status |= LEDB;                                         //�ߵ�ƽ��rgb_status bit6��1
  else
    rgb_status &= ~LEDB;                                        //�͵�ƽ��rgb_status bit6��0
  return rgb_status;                                            //����rgb_status
}
