/*********************************************************************************************
* �ļ���Vibration.c
* ���ߣ�Lixm 2017.10.17
* ˵��������������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "Vibration.h"

#define VIBRATION_RCC     RCC_AHB1Periph_GPIOA
#define VIBRATION_GPIO    GPIOA
#define VIBRATION_PIN     GPIO_Pin_0

/*********************************************************************************************
* ���ƣ�vibration_init()
* ���ܣ��𶯴�������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void vibration_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure={0};                      //����һ��GPIO_InitTypeDef���͵Ľṹ��
  RCC_AHB1PeriphClockCmd(VIBRATION_RCC, ENABLE);                //��������ص�GPIO����ʱ��
  
  GPIO_InitStructure.GPIO_Pin = VIBRATION_PIN;                  //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������ŵ��������Ϊ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //��������ģʽΪ����ģʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //��������Ϊ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //������������Ϊ2MHz
  
  GPIO_Init(VIBRATION_GPIO, &GPIO_InitStructure);               //��ʼ��GPIO����
}

/*********************************************************************************************
* ���ƣ�unsigned char get_vibration_status(void)
* ���ܣ���ȡ�𶯴�����״̬
* ��������
* ���أ�1 ��ʾ���� 
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_vibration_status(void)                     
{  
  if(GPIO_ReadInputDataBit(VIBRATION_GPIO,VIBRATION_PIN))       //��ȡ�𶯴�������״̬
    return 0;
  else
    return 1;
}