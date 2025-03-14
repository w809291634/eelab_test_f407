/*********************************************************************************************
* �ļ���FAN.c
* ���ߣ�Lixm 2017.10.17
* ˵����������������
* �޸ģ�fuyou 2018.8.25 �������ȿ�������
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "FAN.h"

/*********************************************************************************************
* ���ƣ�fan_init()
* ���ܣ����ȴ�������ʼ��
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void fan_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;                          //����һ��GPIO_InitTypeDef���͵Ľṹ��
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);         //�������ȴ�������ص�GPIO����ʱ��
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                     //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������ŵ��������Ϊ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //��������ģʽΪ���ģʽ
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //��������Ϊ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //������������Ϊ2MHz
  
  GPIO_Init(GPIOB, &GPIO_InitStructure);                        //��ʼ��GPIO����
  GPIO_ResetBits(GPIOB,GPIO_Pin_10);
}

/*********************************************************************************************
* ���ƣ�void fan_control(unsigned char cmd)
* ���ܣ����ȿ�������
* ��������������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void fan_control(unsigned char cmd)
{ 
  if(cmd & 0x01)
    GPIO_SetBits(GPIOB,GPIO_Pin_10);
  else
    GPIO_ResetBits(GPIOB,GPIO_Pin_10);
}

static u32 cycle;                                       // ���ֵ��ҪС��100������ռ�ձȲ�׼ȷ
/*********************************************************************************************
* ���ƣ�fan_pwm_init()
* ���ܣ����ȴ�����PWM��ʼ��  PB10 ���� TIM2����CH3
* ������arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void fan_pwm_init(u32 arr,u32 psc)
{		 					 
  cycle=arr+1;                                          // ��������ռ�ձȡ���Ҫ��1
  //�˲������ֶ��޸�IO������
  GPIO_InitTypeDef GPIO_InitStructure={0};
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
  TIM_OCInitTypeDef  TIM_OCInitStructure={0};

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  	  //TIM2ʱ��ʹ��    
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ��PORTFʱ��	

  GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_TIM2); //GPIOB10 ����Ϊ��ʱ��2

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;             //GPIOB10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;          //���ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	  //�ٶ�100MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;        //���츴�����
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;          //����
  GPIO_Init(GPIOB,&GPIO_InitStructure);                 //��ʼ��PB10
    
  TIM_TimeBaseStructure.TIM_Prescaler=psc;              //��ʱ����Ƶ,��ʱ��2���ص�APB1��Ϊ84MHZ����������Ƶ��1MHZ������Ϊ83
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
  TIM_TimeBaseStructure.TIM_Period=arr;                 //�Զ���װ��ֵ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
  TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);       //��ʼ����ʱ��2

  //��ʼ��TIM2 Channel3 PWMģʽ	 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;      //�������
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);                      //��ʼ��ͨ��3

  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);             //ʹ��TIM2��CCR3�ϵ�Ԥװ�ؼĴ���

  TIM_ARRPreloadConfig(TIM2,ENABLE);                            //ARPEʹ�� 
  TIM_Cmd(TIM2, ENABLE);                                        //ʹ��TIM2								  
} 

/*********************************************************************************************
* ���ƣ�fan_pwm_control
* ���ܣ�����PWM��������
* ������pwm ռ�ձ� 0-100
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void fan_pwm_control(uint32_t pwm)
{ 
  uint32_t _pwm=cycle/100*pwm;
  TIM_SetCompare3(TIM2,_pwm);	                                      //�޸ıȽ�ֵ���޸�ռ�ձ�
}