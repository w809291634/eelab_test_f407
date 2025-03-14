/*********************************************************************************************
* ��ʱ��ͷ�ļ�
*********************************************************************************************/
#include "timer.h"

/*********************************************************************************************
* ���ƣ�timer_init()
* ���ܣ���ʱ����ʼ������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void timer_init(void)
{
  TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;                //��ʼ��������ʱ���ṹ�� 
  NVIC_InitTypeDef NVIC_InitStructure;                          //��ʼ���ж��������ýṹ��
 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);          //��ʼ����ʱ��ʱ��
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	        //��ʼ���ж����ȼ�����Ϊ����0	
  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;               //�����ж���ΪTIM7_IRQ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     //��Ӧ���ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;            //��ռ���ȼ�Ϊ3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //ʹ���ж�
  NVIC_Init(&NVIC_InitStructure);                               //��ʼ���ж����ò���
  //�����жϴ���ʱ��Ϊ10MS
  TIM_BaseInitStructure.TIM_Period = 1000 - 1;                  //���ü�������Ϊ1000
  TIM_BaseInitStructure.TIM_Prescaler = 840 - 1;                //����Ԥ��Ƶϵ��Ϊ840

  TIM_TimeBaseInit(TIM7, &TIM_BaseInitStructure);               //��ʼ��������ʱ��TIM7
    
  TIM_ClearFlag(TIM7, TIM_FLAG_Update);                         //���TIM7�����־λ
  TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);                      //ʹ��TIM7�ж�
  
  TIM_Cmd(TIM7, ENABLE);                                        //ʹ��TIM7
}

/*********************************************************************************************
* ���ƣ�TIM7_IRQHandler()
* ���ܣ���ʱ��TIM7�жϷ�����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
extern uint8_t rgb_twinkle;                                     //RGB����˸ִ�б�־λ
extern uint8_t gas_check;                                       //ȼ�����ִ�б�־λ
extern uint8_t fire_effect;                                     //����ͼ����˸ִ�б�־λ
uint8_t count_led = 0;                                          //RGB����˸��ʱ
uint8_t count_gas = 0;                                          //ȼ�������ʱ
uint8_t count_fire = 0;                                         //����ͼ����˸ִ����ʱ

void TIM7_IRQHandler(void)
{
  if(TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET){            //����жϱ�־λ��λ
    //rgb����˸ִ�б�־λ����
    if((count_led % 10) == 9){                                  //RGB�Ƽ�ʱ�Ƿ���100MS
      rgb_twinkle = 1;                                          //��־λ��λ
      count_led = 0;                                            //��������
    }else count_led ++;                                         //������һ
    //ȼ�����ִ�б�־λ����
    if((count_gas % 100) == 99){                                //ȼ�������ʱ�Ƿ�1S
      count_gas = 0;                                            //��������
      gas_check = 1;                                            //��־λ��λ
    }else  count_gas ++;                                        //������һ
    //����ͼ����˸��ִ�б�־λ����
    if((count_fire % 5) == 4){                                  //����ͼ����˸�Ƿ�50ms
      fire_effect = 1;                                          //��־λ��λ
      count_fire = 0;                                           //��������
    }else count_fire ++;                                        //������һ
    
    TIM_ClearITPendingBit(TIM7 , TIM_IT_Update);                //��ն�ʱ��TIM7��־λ
  }
}
