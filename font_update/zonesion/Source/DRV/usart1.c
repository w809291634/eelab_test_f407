#include "extend.h"
#include "stm32f4xx.h"
#include "usart1.h"	
#include "stdio.h"

/**************************user config*************************/
// �������̨���Դ��ڵ�Ӳ��
// ����1 ��������
#define USARTx              USART1
#define USARTx_IRQn         USART1_IRQn
#define USARTx_GPIO_RCC     RCC_AHB1Periph_GPIOA
#define USARTx_RCC_CMD_FUN  RCC_APB2PeriphClockCmd
#define USARTx_RCC          RCC_APB2Periph_USART1
#define USARTx_TX_GPIO      GPIOA
#define USARTx_TX_PIN_NUM   GPIO_PinSource9
#define USARTx_TX_PIN       GPIO_Pin_9

#define USARTx_RX_GPIO      GPIOA
#define USARTx_RX_PIN_NUM   GPIO_PinSource10
#define USARTx_RX_PIN       GPIO_Pin_10

#define USARTx_RINGBUF_SIZE                 (256)     

/**************************user config*************************/

// shell �ն�ʹ�ñ���
shellinput_t shell_1;
static char shell_ringbuf[USARTx_RINGBUF_SIZE]={0};
static volatile unsigned short Read_Index,Write_Index;

char usart1_mode=0;             // ���ڵ������� ����ָ��  0�����ݵ���shell����̨  1�����ݵ���IAP����

//��ʼ��IO ����1 
//bound:������
void shell_hw_init(u32 bound)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure={0};
  USART_InitTypeDef USART_InitStructure={0};
  NVIC_InitTypeDef NVIC_InitStructure={0};

  RCC_AHB1PeriphClockCmd(USARTx_GPIO_RCC,ENABLE);    //ʹ��GPIOAʱ��
  USARTx_RCC_CMD_FUN(USARTx_RCC,ENABLE);              //ʹ��USARTʱ��

  //����1��Ӧ���Ÿ���ӳ��
  GPIO_PinAFConfig(USARTx_TX_GPIO,USARTx_TX_PIN_NUM,GPIO_AF_USART1); //GPIOA9����ΪUSART
  GPIO_PinAFConfig(USARTx_RX_GPIO,USARTx_RX_PIN_NUM,GPIO_AF_USART1); //GPIOA10����ΪUSART

  //USART �˿�����
  GPIO_InitStructure.GPIO_Pin = USARTx_TX_PIN | USARTx_RX_PIN; //GPIOA9��GPIOA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
  GPIO_Init(USARTx_TX_GPIO,&GPIO_InitStructure); //��ʼ��PA9��PA10

  //USART ��ʼ������
  USART_InitStructure.USART_BaudRate = bound;//����������
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USARTx, &USART_InitStructure); //��ʼ������

	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);//��������ж�

	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USARTx_IRQn;			  // ����1�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1; // ��ռ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		    // �����ȼ�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    // IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure);	
  
  USART_Cmd(USARTx, ENABLE);  //ʹ�ܴ���1 
}

// �������
void usart1_flush(void)
{
  Read_Index=Write_Index;
}

// ����1�ķ��ͺ���
// �����ַ�
void usart1_put(const char data)
{
  USART_ClearFlag(USARTx, USART_FLAG_TC);
  USARTx->DR = data;
  while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

// ����1�ķ��ͺ���
// �����ַ���
void usart1_puts(const char * strbuf, unsigned short len)
{
  while(len--)
  {
    USART_ClearFlag(USARTx, USART_FLAG_TC);
    USARTx->DR = *strbuf;
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
    strbuf++ ;
  }
}

/* ��ȡ��������ʣ��δ��ȡ���� */
// data ��ȡʣ������
// ringbuf ��ǰ���λ����� size ��������С 
// w_index r_index ��ǰ�������Ķ�д����
// ���� ��ȡ����
uint16_t UART_GetRemainDate(char* data, char* ringbuf, uint16_t size, uint16_t w_index, uint16_t r_index) 
{
  //��ȡʣ������
  if(w_index >= r_index) { 
    int len1 = w_index - r_index;
    memcpy(data,ringbuf + r_index,len1);
    return len1;
  } else {
    int len1=size-r_index;          // ����ĩβ������
    int len2=w_index;
    memcpy(data,ringbuf + r_index,len1);
    memcpy(data+len1,ringbuf,len2);
    return len1+len2;
  }
}

// �ӻ�������ȡһ���ֽ�
// ע�⣺���ݴ�������shell_app_cycleֻ��ѡ����һ
int usart1_getchar(uint8_t* data)
{
  if(Write_Index!=Read_Index){
    *data=shell_ringbuf[Read_Index];
    Read_Index = (Read_Index+1)% USARTx_RINGBUF_SIZE;   // ��ȡ������1
    return 1;
  }else return 0;
}

// shell����̨��ȡ��������
void shell_app_cycle()
{
#define SHELL_DATA_PROCESS          {shell_input(&shell_1, temp, data_len);\
                                      Read_Index = (Read_Index+data_len)% USARTx_RINGBUF_SIZE;}
  if(Write_Index!=Read_Index){
    /* ȡ���λ�����ʣ������ */
    char temp[USARTx_RINGBUF_SIZE]={0};
    char flag=0x1B;             // ������������ַ�
    uint16_t data_len=UART_GetRemainDate(temp,shell_ringbuf,USARTx_RINGBUF_SIZE,Write_Index,Read_Index);

    /* ���ݴ��� */
    if(strchr(temp,flag)){
      if(strlen(temp)>2)SHELL_DATA_PROCESS;
    }
    else SHELL_DATA_PROCESS
  }
}

//����1�жϷ������
void USART1_IRQHandler(void)
{
  if(USART_GetITStatus( USARTx, USART_IT_RXNE ) == SET )
  {	
    shell_ringbuf[Write_Index] = (char)USART_ReceiveData(USARTx);
    Write_Index++;
    Write_Index = Write_Index % USARTx_RINGBUF_SIZE;
  }	
} 

//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
  usart1_put(ch);
  return ch;
}
