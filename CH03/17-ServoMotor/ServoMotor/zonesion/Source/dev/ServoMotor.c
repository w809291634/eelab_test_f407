/*********************************************************************************************
* �ļ���ServoMotor.c
* ���ߣ�Lixm 2017.10.17
* ˵�����̵�����������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "ServoMotor.h"
#include <math.h> 

#define TIM_RCC           RCC_APB1Periph_TIM2
#define TIMX              TIM2

#define TIM_CHX_AF        GPIO_AF_TIM2
#define TIM_CHX_RCC       RCC_AHB1Periph_GPIOA
#define TIM_CHX_GPIO      GPIOA
#define TIM_CHX_PIN       GPIO_Pin_5
#define TIM_CHX_PINSOURCE GPIO_PinSource5

/*********************************************************************************************
* ���ƣ�ServoMotor_init()
* ���ܣ����PWM��ʼ��  PA5/TIM2_CH1
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void ServoMotor_init(u32 arr,u32 psc)
{
  //�˲������ֶ��޸�IO������
  GPIO_InitTypeDef GPIO_InitStructure={0};
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure={0};
  TIM_OCInitTypeDef  TIM_OCInitStructure={0};

  RCC_AHB1PeriphClockCmd(TIM_CHX_RCC, ENABLE);
  RCC_APB1PeriphClockCmd(TIM_RCC,ENABLE);  	                    //TIMʱ��ʹ��    
  
  GPIO_PinAFConfig(TIM_CHX_GPIO,TIM_CHX_PINSOURCE,TIM_CHX_AF); 

  GPIO_InitStructure.GPIO_Pin = TIM_CHX_PIN;                     //PIN
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //���ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	          //�ٶ�100MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //���츴�����
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //����
  GPIO_Init(TIM_CHX_GPIO,&GPIO_InitStructure);                  //��ʼ��
  
  TIM_TimeBaseStructure.TIM_Prescaler=psc;                      //��ʱ����Ƶ,�˴���ʱ�����ص�APB1��Ϊ84MHZ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;     //���ϼ���ģʽ
  TIM_TimeBaseStructure.TIM_Period=arr;                         //�Զ���װ��ֵ
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
  TIM_TimeBaseInit(TIMX,&TIM_TimeBaseStructure);                //��ʼ����ʱ��

  //��ʼ��TIM4 Channel1 PWMģʽ	 
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     //�������
  TIM_OC1Init(TIMX, &TIM_OCInitStructure);                      //��ʼ��ͨ��1

  TIM_OC1PreloadConfig(TIMX, TIM_OCPreload_Enable);             //ʹ��TIM��CCR1�ϵ�Ԥװ�ؼĴ���

  TIM_ARRPreloadConfig(TIMX,ENABLE);                            //ARPEʹ�� 
  TIM_Cmd(TIMX, ENABLE);                                        //ʹ��TIM
}

/*********************************************************************************************
* ���ƣ�ServoPWM_control
* ���ܣ��ŷ�PWM��������
        20ms -- 20000 arrֵ -- 20000 us -- 1us��Ӧ1��arrֵ
        0.5ms -- 500 arrֵ   2.5%
        2.5ms -- 2500 arrֵ  12.5%
        ����Ϊ 20ms  0.5ms-2.5ms �ߵ�λռ�� ���ƶ��0-180��
* ������angle �Ƕ� 0-180
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#define OFFSET  1.0f                                            //����Ƕ�����ֵ
void Servo_control(u8 angle)
{ 
  u32 pwm = (u32)round((2000* angle)/180.0f);
  TIM_SetCompare1(TIMX,(500+pwm)/OFFSET);                       //�޸ıȽ�ֵ���޸�ռ�ձ�
}
