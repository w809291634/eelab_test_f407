/*********************************************************************************************
* �ļ���timer.c
* ���ߣ�chennian 2017.11.2
* ˵����
* ���ܣ���ʱ����������
* �޸ģ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "timer.h"

extern char led_status;  
/*********************************************************************************************
* ���ƣ�timer3_init
* ���ܣ���ʼ��TIM3
* ������period���Զ���װֵ�� prescaler��ʱ��Ԥ��Ƶ��
* ���أ���
* �޸ģ�
* ע�ͣ���ʱ�����ʱ����㷽��:Tout=((period+1)*(prescaler+1))/Ft us.
*       AHB Prescaler = 1;AHB��ʱ��HCLK=SYSCLK/1 = 168MHz;
*       TIM3������APB1�ϣ� APB1 Prescaler = 4��APB1��ʱ��Ƶ��PCLK1 = HCLK/4 = 42;Ft=2*PCLK1= 84MHz
*       Ft=��ʱ������Ƶ��,��λ:Mhz��
*********************************************************************************************/
void timer3_init(unsigned int period, unsigned short prescaler)//TIM_PeriodΪ16λ����
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;         //��ʱ������
  NVIC_InitTypeDef  NVIC_InitStructure;                   //�ж�����

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;         //TIM3�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;      //�����ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //ʹ���ж�

  NVIC_Init(&NVIC_InitStructure);			                    //�����������ó�ʼ���ж�

  TIM_TimeBaseStructure.TIM_Period = period;              //��������װֵ
  TIM_TimeBaseStructure.TIM_Prescaler = prescaler;        //Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӷָ�
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);         //���������ó�ʼ��TIM3

  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                //����ʱ��3�����ж�
  TIM_Cmd(TIM3, ENABLE);                                  //ʹ��TIM3
}


/*********************************************************************************************
* ���ƣ�TIM3_IRQHandler
* ���ܣ�TIM3�жϴ�����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update ) != RESET) {         //����жϱ�־������
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                 //����жϱ�־
    led_status = ~led_status;                                   //LED��״̬��־λ��ת
  }	
}