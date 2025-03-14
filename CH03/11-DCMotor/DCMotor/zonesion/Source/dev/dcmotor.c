#include "dcmotor.h"

static vs32 encoder_count;
vu32 dcmotor_dir;
double dcmotor_speed;
#define REDUCTION_RATIO       150                               // ������ٱ�
#define PULSE_PERIOD          6                                 // ʹ�õ������������������һȦ��Լ6����
/*********************************************************************************************
* ����:dcmotor_init()
* ����:ֱ�������ʼ������
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void dcmotor_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure={0};                      //����һ��GPIO_InitTypeDef���͵Ľṹ��
  RCC_AHB1PeriphClockCmd(DCMOTOR_RCC1, ENABLE);                 //������ص�GPIO����ʱ��
  RCC_AHB1PeriphClockCmd(DCMOTOR_RCC2, ENABLE);
  
  // ��ʼ���������1
  GPIO_InitStructure.GPIO_Pin = DCMOTOR_PIN1;                   //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������ŵ��������Ϊ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //��������ģʽΪ���ģʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //��������������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //������������
  GPIO_Init(DCMOTOR_GPIO1, &GPIO_InitStructure);                //��ʼ��GPIO����
  
  // ��ʼ���������2
  GPIO_InitStructure.GPIO_Pin = DCMOTOR_PIN2;                   //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������ŵ��������Ϊ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //��������ģʽΪ���ģʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //��������������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //������������
  GPIO_Init(DCMOTOR_GPIO2, &GPIO_InitStructure);                //��ʼ��GPIO����
  
  DCMOTOR_STOP;
  encoder_init(1000,8400-1);                                   //(8400)0.1ms�������ڣ�0.1*1000=0.1s���
}

/*********************************************************************************************
* ����:encoder_init()
* ����:ֱ�������������ʼ������ ʹ��PB1��ȡ�������������ֵ,��ʱ����Ϊһ���ٶ�ʱ��
* ����:arr ��ʱ�� arr�Ĵ���ֵ psr Ԥ��Ƶֵ
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void encoder_init(u32 arr,u32 psc)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};
  NVIC_InitTypeDef   NVIC_InitStructure={0};
  EXTI_InitTypeDef   EXTI_InitStructure={0};
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
  
  // ��ʱ����ʼ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);           //TIMʱ��ʹ��   
  
  TIM_TimeBaseStructure.TIM_Prescaler=psc;                      //��ʱ����Ƶ,��ʱ��3���ص�APB1��Ϊ84MHZ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;     //���ϼ���ģʽ
  TIM_TimeBaseStructure.TIM_Period=arr;                         //�Զ���װ��ֵ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);                //��ʼ����ʱ��
  TIM_ARRPreloadConfig(TIM3,ENABLE);                            //ARPEʹ�� 
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                      //����ʱ�������ж�
  TIM_Cmd(TIM3, ENABLE);                                        //ʹ��TIM
  
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;                 //��ʱ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;    //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00;           //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
  // GPIO ��ʼ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);         //ʹ��GPIOʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;                     //��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //����
  GPIO_Init(GPIOB, &GPIO_InitStructure);                        //��ʼ��GPIO
  
  // EXTI ��ʼ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);        //ʹ��SYSCFGʱ��
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1); //PB1 ���ӵ��ж���1
  
  EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;           //�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        //�½��ش��� 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     //ʹ��
  EXTI_Init(&EXTI_InitStructure);                               //��������������
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;              //�ⲿ�жϺ�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;  //��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;         //�����ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);                               //���������ó�ʼ��
}

/*********************************************************************************************
* ����:EXTI1_IRQHandler()
* ����:�ⲿ�ж�1�������
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void EXTI1_IRQHandler(void)
{
  static char last_dir;
  if(last_dir!=dcmotor_dir){
    last_dir=dcmotor_dir;
    encoder_count=0;
  }
  if(dcmotor_dir) encoder_count++;
  else encoder_count--;
  EXTI_ClearITPendingBit(EXTI_Line1); //���LINE0�ϵ��жϱ�־λ 
}

/*********************************************************************************************
* ����:TIM3_IRQHandler()
* ����:��ʱ��3�жϷ�����  
* ����:��
* ����:��
* �޸�:
* ע��:
*********************************************************************************************/
void TIM3_IRQHandler(void)
{
  double count;
  if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)  //����ж�
  {
    count = encoder_count*1000/100;             //��ʱ��100ms��������ﵥλ ����/s
    //count = encoder_count*60*1000/100;        //����/min
    encoder_count =0;
    dcmotor_speed = count/REDUCTION_RATIO/PULSE_PERIOD;  //�����������ʵ��ת�� r/s
  }
  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}
