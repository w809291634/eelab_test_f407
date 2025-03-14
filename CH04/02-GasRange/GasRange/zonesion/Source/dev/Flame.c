/*********************************************************************************************
* �ļ���Flame.c
* ���ߣ�Lixm 2017.10.17
* ˵����������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "Flame.h"

#define FLAME_RCC     RCC_AHB1Periph_GPIOC
#define FLAME_PIN     GPIO_Pin_11
#define FLAME_GPIO    GPIOC

/*********************************************************************************************
* ���ƣ�flame_init()
* ���ܣ����洫������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void flame_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure={0};                      //����һ��GPIO_InitTypeDef���͵Ľṹ��
  RCC_AHB1PeriphClockCmd(FLAME_RCC, ENABLE);                    //����������ص�GPIO����ʱ��
  
  GPIO_InitStructure.GPIO_Pin = FLAME_PIN;                      //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������ŵ��������Ϊ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //��������ģʽΪ����ģʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //��������Ϊ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //������������Ϊ2MHz
  
  GPIO_Init(FLAME_GPIO, &GPIO_InitStructure);                   //��ʼ��GPIO����
}

/*********************************************************************************************
* ���ƣ�unsigned char get_flame_status(void)
* ���ܣ���ȡ���洫����״̬
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_flame_status(void)
{ 
  if(GPIO_ReadInputDataBit(FLAME_GPIO,FLAME_PIN))
    return 0;
  else
    return 1;
}