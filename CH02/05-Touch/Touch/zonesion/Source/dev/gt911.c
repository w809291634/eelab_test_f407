#include "gt911.h"
#include "iic4.h"
#include "delay.h"
#include "stdio.h"

GT911_point_t GT911Point[GT911_TOUCH_NUM] = {{0}};      // ������λ��Ϣ�洢
unsigned char GT911_pointNum = 0, touchFlag = 0;
void (*GT911_irq_cb)(void);                             //Ӧ�ò�Ļص�����

// �������ų�ʼ��
static void GT911_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct={0};
  RCC_AHB1PeriphClockCmd(GT911_RST_CLK | GT911_EXTI_CLK, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = GT911_RST_PIN ;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
  GPIO_Init(GT911_RST_GPIO, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = GT911_EXTI_PIN;
  GPIO_Init(GT911_EXTI_GPIO, &GPIO_InitStruct);
}

// ����оƬ�жϳ�ʼ��
static void GT911_exti_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct={0};
  EXTI_InitTypeDef   EXTI_InitStructure={0};
  NVIC_InitTypeDef   NVIC_InitStructure={0};
  
  // GPIO ��ʼ��
  RCC_AHB1PeriphClockCmd(GT911_EXTI_CLK, ENABLE);
  GPIO_InitStruct.GPIO_Pin = GT911_EXTI_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
  GPIO_Init(GT911_EXTI_GPIO, &GPIO_InitStruct); 
  
  // EXTI ��ʼ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);        //ʹ��SYSCFGʱ��
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource2); //PG2 ���ӵ��ж���2

  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;           //�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        //�Ͻ��ش��� 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     //ʹ��
  EXTI_Init(&EXTI_InitStructure);                               //��������������

  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;              //�ⲿ�жϺ�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;  //��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;         //�����ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);   
}

// �ⲿ�жϷ�����
void EXTI2_IRQHandler(void)
{
  touchFlag = 1;
  if(GT911_irq_cb != NULL) GT911_irq_cb();                      //��Ӧ�ò�����лص�
	EXTI_ClearITPendingBit(EXTI_Line2);
}	

// д�Ĵ���
static int GT911_writeReg(unsigned short reg, unsigned char *data, unsigned char len)
{
  unsigned char ret = 0;
  I2C4_Start();
  I2C4_WriteByte(GT911_CMD_WR);
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg >> 8);       // Register_H
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg & 0xFF);     // Register_L
  if(I2C4_WaitAck())return -1;
  for(unsigned char i=0; i<len; i++)
  {
    I2C4_WriteByte(data[i]); 
    if(I2C4_WaitAck())return -1;
    ret++;  
  }
  I2C4_Stop();
  return ret;
}

// ���Ĵ���
static int GT911_readReg(unsigned short reg, unsigned char *data, unsigned char len)
{
  /* �趨���Ĵ����׵�ַ */
  I2C4_Start();	
  I2C4_WriteByte(GT911_CMD_WR);  
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg >> 8);       // Register_H
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg & 0xFF);     // Register_L
  if(I2C4_WaitAck())return -1;
//  I2C4_Stop();

  /* ��ȡ���� */
  I2C4_Start(); 
  I2C4_WriteByte(GT911_CMD_RD); 
  if(I2C4_WaitAck())return -1;
  for(unsigned char i=0; i<len; i++){
    data[i] = I2C4_ReadByte(i == (len - 1) ? 0 : 1);
  } 
  I2C4_Stop();	
  return len;
}

// ���ú���
int GT911_cfg(void)
{
  unsigned char cfgBuf[14] = {0};
  GT911_writeReg(GT911_CTRL_REG, cfgBuf, 1);    // д������
  GT911_readReg(GT911_PID_REG, cfgBuf, 14);     // ��ȡPID
  if(cfgBuf[0] == 0x39 && cfgBuf[1] == 0x31 && cfgBuf[2] == 0x31)
    return 0;
  return -1;
}

// GT911����ɨ�躯��
void GT911_scan(void)
{
  if(touchFlag == 0) return ;
  
  unsigned char pointBuf[GT911_TOUCH_NUM * 8] = {0};    // �洢��λ��Ϣ
  unsigned char info = 0;
  
  GT911_readReg(GT911_GSTID_REG, &info, 1);
  GT911_pointNum = info & 0x0F;                         // ��ȡ������
  if(GT911_pointNum > GT911_TOUCH_NUM)
    GT911_pointNum = GT911_TOUCH_NUM;
  
  /* �д����� */
  if(info > 0x80)
  {
    GT911_readReg(GT911_GSTID_REG + 1, pointBuf, GT911_pointNum * 8);
    for(unsigned char i=0; i<GT911_pointNum; i++){
      GT911Point[i].id = pointBuf[i*8];
      GT911Point[i].x = 854 - (pointBuf[4+(i*8)]<<8 | pointBuf[3+(i*8)]);
      GT911Point[i].y = (pointBuf[2+(i*8)]<<8 | pointBuf[1+(i*8)]);
      GT911Point[i].size = (pointBuf[6+(i*8)]<<8 | pointBuf[5+(i*8)]);
    }
  }
  /* û�д�����ʱΪ���ֵ */
  else if(info == 0x80)
  {
    for(unsigned char i=0; i<GT911_TOUCH_NUM; i++){
      GT911Point[i].x = 0xFFFF;
      GT911Point[i].y = 0xFFFF;
      GT911Point[i].size = 0;
    }
    GT911_pointNum = 0;
  }
  
  info = 0;
  GT911_writeReg(GT911_GSTID_REG, &info, 1);
  touchFlag = 0;
}

// GT911���س�ʼ������
int GT911_init(void)
{
  GT911_gpio_init();
  I2C4_IO_Init();
  delay_ms(10);     // �ȴ�
  
  // �趨��ַΪ 0x28/0x29
  GT911_RST_L;
  GT911_EXTI_H;
  delay_ms(1);      // ����10us
  GT911_RST_H;
  delay_ms(6);      // ����5ms
  
  GT911_exti_init();// EXTI תΪ����̬
  delay_ms(50);     // ��ʱһ��ʱ���ٷ���������Ϣ
  return GT911_cfg();
}
