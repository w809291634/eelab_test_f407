/*********************************************************************************************
* �ļ���usart.c
* ���ߣ�zonesion 2017.02.17
* ˵����������������
* �޸ģ�Chenkm 2017.02.17 ���ע��
* ע�ͣ�
*
*********************************************************************************************/
/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include <string.h>
#include "stdio.h"
#include "usart.h"

/*********************************************************************************************
* ȫ�ֱ���
*********************************************************************************************/
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;                                             //����״̬���	
u8 USART_RX_BUF[USART_REC_MAX];     		                        //���ջ���,���USART_REC_LEN���ֽ�.

/*********************************************************************************************
* ���ƣ�fputc
* ���ܣ���usart1ӳ�䵽printf����
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
int fputc(int ch, FILE *f)
{ 	
  USART_ClearFlag(USART1, USART_FLAG_TC);
  USART1->DR = ch;
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
  return ch;
}

/*********************************************************************************************
* ���ƣ�usart_init
* ���ܣ�usart1��ʼ��
* ������bound������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void usart_init(unsigned int bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure={0};
  USART_InitTypeDef USART_InitStructure={0};
  NVIC_InitTypeDef NVIC_InitStructure={0};	
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);     	//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);    	//ʹ��USART1ʱ�� 
  //����1��Ӧ���Ÿ���ӳ��
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);  	//GPIOA9����ΪUSART1
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 	//GPIOA10����ΪUSART1	
  //USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;  	//GPIOA9��GPIOA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;             	//���ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//�ٶ�50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           	//���츴�����
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             	//����
  GPIO_Init(GPIOA,&GPIO_InitStructure);                    	//��ʼ��PA9��PA10
  //USART1 ��ʼ������
  USART_InitStructure.USART_BaudRate = bound;                   //����������
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //�ֳ�Ϊ8λ���ݸ�ʽ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;        //һ��ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;           //����żУ��λ
  //��Ӳ������������
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  //�շ�ģʽ
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);                     //�����������ó�ʼ������1	
  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;             //����1�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;       //��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;	        //�����ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	        //IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure);	                        //����ָ���Ĳ�����ʼ��VIC�Ĵ�����
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                //��������1�����ж�
  USART_Cmd(USART1, ENABLE);                                    //ʹ�ܴ���1 	
}

/*********************************************************************************************
* ���ƣ�USART1_IRQHandler
* ���ܣ������жϴ�����
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void USART1_IRQHandler(void)                	           
{
  u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		if((USART_RX_STA&0x8000)==0)//����δ���
		{
			if(USART_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//���մ���,���¿�ʼ
				else USART_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_MAX-1))USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
  } 
}

/*********************************************************************************************
* ���ƣ�usart_send
* ���ܣ�����1��������
* ������s�����͵�����ָ�룬len�����͵����ݳ���
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/ 
void usart_send(unsigned char *s,unsigned char len){
  for(unsigned char i = 0;i < len;i++)
  {
    USART_SendData(USART1, *(s+i));
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE ) == RESET);
  }
}