/*********************************************************************************************
* �ļ���iic.c
* ���ߣ�zonesion
* ˵����iic��������
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "iic3.h"
#include "stm32f4xx.h"
#include "delay.h"
/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define I2C_GPIO                GPIOF
#define I2C_CLK                 RCC_AHB1Periph_GPIOF
#define PIN_SCL                 GPIO_Pin_9
#define PIN_SDA                 GPIO_Pin_10  

#define SDA_R                   GPIO_ReadInputDataBit(I2C_GPIO,PIN_SDA)
/*********************************************************************************************
* ���ƣ�iic_init()
* ���ܣ�I2C��ʼ������
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};
  RCC_AHB1PeriphClockCmd(I2C_CLK, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = PIN_SCL | PIN_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);  
}

/*********************************************************************************************
* ���ƣ�sda_out()
* ���ܣ�����SDAΪ���
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sda_out(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};
  GPIO_InitStructure.GPIO_Pin = PIN_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);  
}

/*********************************************************************************************
* ���ƣ�sda_in()
* ���ܣ�����SDAΪ����
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void sda_in(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};
  GPIO_InitStructure.GPIO_Pin = PIN_SDA;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);  
}

/*********************************************************************************************
* ���ƣ�iic_start()
* ���ܣ�I2C��ʼ�ź�
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_start(void)
{
  sda_out();
  GPIO_SetBits(I2C_GPIO,PIN_SDA);                               //����������
  GPIO_SetBits(I2C_GPIO,PIN_SCL);                               //����ʱ����
  delay_us(2);                                                  //��ʱ
  GPIO_ResetBits(I2C_GPIO,PIN_SDA);                             //����������
  delay_us(2);                                                  //��ʱ
  GPIO_ResetBits(I2C_GPIO,PIN_SCL);                             //ǯסI2C���ߣ�׼�����ͻ�������� 
}

/*********************************************************************************************
* ���ƣ�iic_stop()
* ���ܣ�I2Cֹͣ�ź�
* ��������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_stop(void)
{
  sda_out();
  GPIO_ResetBits(I2C_GPIO,PIN_SDA);                             //����������
  GPIO_SetBits(I2C_GPIO,PIN_SCL);                               //����ʱ����
  delay_us(2);                                                  //��ʱ
  GPIO_SetBits(I2C_GPIO,PIN_SDA);                               //���������ߣ�����I2C���߽����ź�
  delay_us(2);                                                  //��ʱ
}

/*********************************************************************************************
* ���ƣ�iic_send_ack()
* ���ܣ�I2C����Ӧ���ź�
* ������ack��Ϊ0������ACKӦ��  Ϊ1��������ACKӦ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void iic_send_ack(char ack)
{
  GPIO_ResetBits(I2C_GPIO,PIN_SCL);                             //����ʱ����
  sda_out();
  if(ack)
    GPIO_SetBits(I2C_GPIO,PIN_SDA);                             //������ACKӦ��
  else
    GPIO_ResetBits(I2C_GPIO,PIN_SDA);                           //����ACKӦ��
  delay_us(2);                                                  //��ʱ 
  GPIO_SetBits(I2C_GPIO,PIN_SCL);                               //����ʱ����
  delay_us(2);                                                  //��ʱ
  GPIO_ResetBits(I2C_GPIO,PIN_SCL);                             //����ʱ����
}

/*********************************************************************************************
* ���ƣ�iic_recv_ack()
* ���ܣ�I2C����Ӧ��
* ��������
* ���أ�Ӧ���ź�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char iic_recv_ack(void)
{
  unsigned char CY = 0;
  sda_in();
  GPIO_SetBits(I2C_GPIO,PIN_SDA);                               //����������
  GPIO_SetBits(I2C_GPIO,PIN_SCL);                               //����ʱ���� 
  delay_us(2);                                                  //��ʱ
  CY = SDA_R;                                                   //��Ӧ���ź�
  GPIO_ResetBits(I2C_GPIO,PIN_SCL);                             //�ͷ����� 
  return CY;
}

/*********************************************************************************************
* ���ƣ�iic_write_byte()
* ���ܣ�I2Cдһ���ֽ����ݣ�����ACK����NACK���Ӹߵ��ͣ����η���
* ������data -- Ҫд������
* ���أ�0���յ�Ӧ�� 1��û���յ�Ӧ��
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char iic_write_byte(unsigned char data)
{
  unsigned char i;
  sda_out();
  GPIO_ResetBits(I2C_GPIO,PIN_SCL);                             //����ʱ����
  for(i = 0;i < 8;i++){                                         
    if(data & 0x80)                                             //�����߲���
      GPIO_SetBits(I2C_GPIO,PIN_SDA);  	                                                
    else
      GPIO_ResetBits(I2C_GPIO,PIN_SDA); 
    delay_us(2);                                                //��ʱ
    GPIO_SetBits(I2C_GPIO,PIN_SCL);                             //���SDA�ȶ�������SCL���������أ��ӻ���⵽��������ݲ���
    delay_us(2);                                                //��ʱ
    data <<= 1;                                                 //��������һλ
    GPIO_ResetBits(I2C_GPIO,PIN_SCL);                           //����ʱ����
  } 
  /* ����Ӧ�� */
  sda_in();
  GPIO_SetBits(I2C_GPIO,PIN_SDA);                               //����������
  GPIO_SetBits(I2C_GPIO,PIN_SCL);                               //����ʱ����
  delay_us(2);                                                  //��ʱ�ȴ��ӻ�Ӧ��
  if(SDA_R)return 1;                                           //SDAΪ�ߣ��յ�NACK
  else{                                                        //SDAΪ�ͣ��յ�ACK
    GPIO_ResetBits(I2C_GPIO,PIN_SCL);                           //�ͷ�����                  
    return 0;
  }
}

/*********************************************************************************************
* ���ƣ�iic_read_byte()
* ���ܣ�I2C��һ���ֽ����ݣ�����ACK����NACK���Ӹߵ��ͣ����η��͡�
* ������ack��Ϊ0������ACKӦ��  Ϊ1��������ACKӦ��Ӧ���ź�Ϊ�͵�ƽʱ���涨Ϊ��ЧӦ��λ
* ���أ�data����ȡ���ֽ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
unsigned char iic_read_byte(unsigned char ack)
{  
  unsigned char i,data = 0;
  sda_in();                                     // ����������Ϊ����ģʽ
  for(i=0;i<8;i++ )
  {
    GPIO_ResetBits(I2C_GPIO,PIN_SCL);           // ����ʱ���ߣ���ʱ������������
    delay_us(2);                                // �ȴ����ݾ���
    GPIO_SetBits(I2C_GPIO,PIN_SCL);             // ����ʱ���ߣ������ߴ�ʱ�ȶ�
    data<<=1;
    if(SDA_R)data++;                            // ��ȡ����
    delay_us(2);                                // ���ݱ���ʱ��
  }			
  iic_send_ack(ack);                            // ����Ӧ���ź�
  return data;
}

/*********************************************************************************************
* ���ƣ�delay()
* ���ܣ���ʱ
* ������t -- ����ʱ��
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void delay(unsigned int t)                                      //��ʱ����
{
  unsigned char i;
  while(t--){
    for(i = 0;i < 200;i++);
  }					   
}
