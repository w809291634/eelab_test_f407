/*********************************************************************************************
* �ļ���Relay.c
* ���ߣ�Lixm 2017.10.17
* ˵�����̵�����������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "Relay.h"

/*********************************************************************************************
* ���ƣ�relay_init()
* ���ܣ��̵�����������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void relay_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;                          //����һ��GPIO_InitTypeDef���͵Ľṹ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);         //�����̵�����ص�GPIO����ʱ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;        //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������ŵ��������Ϊ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //��������ģʽΪ���ģʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //��������Ϊ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //������������Ϊ2MHz
  
  GPIO_Init(GPIOE, &GPIO_InitStructure);                        //��ʼ��GPIO����
  GPIO_ResetBits(GPIOE,GPIO_Pin_2 | GPIO_Pin_3);
}

/*********************************************************************************************
* ���ƣ�void relay_control(unsigned char cmd)
* ���ܣ��̵�����������
* ��������������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void relay_control(unsigned char cmd)
{ 
  if(cmd & 0x01)
    GPIO_SetBits(GPIOE,GPIO_Pin_2);
  else
    GPIO_ResetBits(GPIOE,GPIO_Pin_2);
  if(cmd & 0x02)
    GPIO_SetBits(GPIOE,GPIO_Pin_3);
  else
    GPIO_ResetBits(GPIOE,GPIO_Pin_3);
}