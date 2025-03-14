/*********************************************************************************************
* �ļ�: led.c
* ���ߣ�liutong 2015.8.19
* ˵����
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
#include "led.h"

/*********************************************************************************************
* ָ�����ļ����������ò���
*********************************************************************************************/
#define LED1_RCC       RCC_AHB1Periph_GPIOG
#define LED2_RCC       RCC_AHB1Periph_GPIOF
#define LED3_RCC       RCC_AHB1Periph_GPIOF
#define LED4_RCC       RCC_AHB1Periph_GPIOF

#define LED1_GPIO      GPIOG
#define LED2_GPIO      GPIOF
#define LED3_GPIO      GPIOF
#define LED4_GPIO      GPIOF

#define LED1_PIN       GPIO_Pin_0
#define LED2_PIN       GPIO_Pin_15
#define LED3_PIN       GPIO_Pin_14
#define LED4_PIN       GPIO_Pin_13

/*********************************************************************************************
* ���ƣ�rgb_init
* ���ܣ���ʼ��RGB��Ӧ��GPIO
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void led_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};                    
  
  RCC_AHB1PeriphClockCmd(LED1_RCC | LED2_RCC |
                         LED3_RCC | LED3_RCC, ENABLE);          //ʹ��GPIOʱ��
  
  GPIO_InitStructure.GPIO_Pin = LED1_PIN;                       //ѡ������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //����ٶ�2MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //��������
  GPIO_Init(LED1_GPIO, &GPIO_InitStructure);                    //����������������
  
  GPIO_InitStructure.GPIO_Pin = LED2_PIN ;                      //ѡ������
  GPIO_Init(LED2_GPIO, &GPIO_InitStructure);                    //����������������
  
  GPIO_InitStructure.GPIO_Pin = LED3_PIN ;                      //ѡ������
  GPIO_Init(LED3_GPIO, &GPIO_InitStructure);                    //����������������

  GPIO_InitStructure.GPIO_Pin = LED4_PIN ;                      //ѡ������
  GPIO_Init(LED4_GPIO, &GPIO_InitStructure);                    //����������������
  
  led_turn_off(LED1_NUM);
  led_turn_off(LED2_NUM);
  led_turn_off(LED3_NUM);
  led_turn_off(LED4_NUM);
}

/*********************************************************************************************
* ���ƣ�led_turn_off
* ���ܣ������Ÿߵ�ƽ���رյ�
* ������cmd
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void led_turn_off(unsigned char cmd){
  if(cmd & LED1_NUM)
    GPIO_ResetBits(LED1_GPIO, LED1_PIN);                        //�����ŵ͵�ƽ,�ر�
  if(cmd & LED2_NUM)
    GPIO_ResetBits(LED2_GPIO, LED2_PIN);
  if(cmd & LED3_NUM)
    GPIO_ResetBits(LED3_GPIO, LED3_PIN);
  if(cmd & LED4_NUM)
    GPIO_ResetBits(LED4_GPIO, LED4_PIN);
}

/*********************************************************************************************
* ���ƣ�led_turn_on
* ���ܣ������ŵ͵�ƽ���򿪵�
* ������cmd
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void led_turn_on(unsigned char cmd){
  if(cmd & LED1_NUM)
    GPIO_SetBits(LED1_GPIO, LED1_PIN);                          //�����Ÿߵ�ƽ���򿪵�
  if(cmd & LED2_NUM)
    GPIO_SetBits(LED2_GPIO, LED2_PIN);
  if(cmd & LED3_NUM)
    GPIO_SetBits(LED3_GPIO, LED3_PIN);
  if(cmd & LED4_NUM)
    GPIO_SetBits(LED4_GPIO, LED4_PIN);
}

/*********************************************************************************************
* ���ƣ�get_led_status
* ���ܣ���ȡ�Ƶ�״̬
* ������
* ���أ�status����λ�ֱ��ʾ�Ƶ�״̬
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char get_led_status(void){
  unsigned char status = 0;
  if(GPIO_ReadOutputDataBit(LED1_GPIO, LED1_PIN))
    status |= LED1_NUM;
  else
    status &= ~LED1_NUM;
  
  if(GPIO_ReadOutputDataBit(LED2_GPIO, LED2_PIN))
    status |= LED2_NUM; 
  else
    status &= ~LED2_NUM;
  
  if(GPIO_ReadOutputDataBit(LED3_GPIO, LED3_PIN))
    status |= LED3_NUM;
  else
    status &= ~LED3_NUM;
  if(GPIO_ReadOutputDataBit(LED4_GPIO, LED4_PIN))
    status |= LED4_NUM;
  else
    status &= ~LED4_NUM;
  return status;                                                //����status
}

// ���� LED ��״̬
// cmd ������Ƶ�LED ��
// state ָ��Ϊ�µ�״̬
void led_setState(uint16_t cmd, unsigned char state)
{
  if(state)
  {
    if(cmd & LED1_NUM)
      led_turn_on(LED1_NUM);
    if(cmd & LED2_NUM)         
      led_turn_on(LED2_NUM);
    if(cmd & LED3_NUM)         
      led_turn_on(LED3_NUM);
    if(cmd & LED4_NUM)         
      led_turn_on(LED4_NUM);
  }
  else
  {
    if(cmd & LED1_NUM)
      led_turn_off(LED1_NUM);
    if(cmd & LED2_NUM)         
      led_turn_off(LED2_NUM);
    if(cmd & LED3_NUM)         
      led_turn_off(LED3_NUM);
    if(cmd & LED4_NUM)         
      led_turn_off(LED4_NUM);
  }
}

// LED ����
// ����λΪ1��ʾ����
void led_ctrl(uint16_t cmd)
{
  if(cmd & LED1_NUM)
    led_turn_on(LED1_NUM);
  else                         
    led_turn_off(LED1_NUM);
  if(cmd & LED2_NUM)         
    led_turn_on(LED2_NUM);
  else                         
    led_turn_off(LED2_NUM);
  if(cmd & LED3_NUM)         
    led_turn_on(LED3_NUM);
  else                         
    led_turn_off(LED3_NUM);
  if(cmd & LED4_NUM)         
    led_turn_on(LED4_NUM);
  else                         
    led_turn_off(LED4_NUM);
}

// cycle �ⲿѭ������
void led_app(u16 cycle)
{
  static u16 time;time++;
  static u8 ledon;
  if(cycle>1000)cycle=1000;
  if(time>=1000/cycle){
    time=0;
    ledon=!ledon;
    if(ledon) led_setState(LED4_NUM,1);
    else led_setState(LED4_NUM,0);
  }
}
