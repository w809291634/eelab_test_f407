/*********************************************************************************************
* 文件：usart.c
* 作者：zonesion 2017.02.17
* 说明：串口驱动程序
* 修改：Chenkm 2017.02.17 添加注释
* 注释：
*
*********************************************************************************************/
/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <string.h>
#include "stdio.h"
#include "usart.h"

/*********************************************************************************************
* 全局变量
*********************************************************************************************/
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;                                             //接收状态标记	
u8 USART_RX_BUF[USART_REC_MAX];     		                        //接收缓冲,最大USART_REC_LEN个字节.

/*********************************************************************************************
* 名称：fputc
* 功能：将usart1映射到printf函数
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
int fputc(int ch, FILE *f)
{ 	
  USART_ClearFlag(USART1, USART_FLAG_TC);
  USART1->DR = ch;
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
  return ch;
}

/*********************************************************************************************
* 名称：usart_init
* 功能：usart1初始化
* 参数：bound波特率
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void usart_init(unsigned int bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure={0};
  USART_InitTypeDef USART_InitStructure={0};
  NVIC_InitTypeDef NVIC_InitStructure={0};	
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);     	//使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);    	//使能USART1时钟 
  //串口1对应引脚复用映射
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);  	//GPIOA9复用为USART1
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 	//GPIOA10复用为USART1	
  //USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;  	//GPIOA9与GPIOA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;             	//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   	//速度50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;           	//推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             	//上拉
  GPIO_Init(GPIOA,&GPIO_InitStructure);                    	//初始化PA9，PA10
  //USART1 初始化设置
  USART_InitStructure.USART_BaudRate = bound;                   //波特率设置
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;        //一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;           //无奇偶校验位
  //无硬件数据流控制
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  //收发模式
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);                     //根据上述配置初始化串口1	
  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;             //串口1中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;       //抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;	        //子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	        //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);	                        //根据指定的参数初始化VIC寄存器、
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                //开启串口1接收中断
  USART_Cmd(USART1, ENABLE);                                    //使能串口1 	
}

/*********************************************************************************************
* 名称：USART1_IRQHandler
* 功能：串口中断处理函数
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void USART1_IRQHandler(void)                	           
{
  u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_MAX-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   		 
  } 
}

/*********************************************************************************************
* 名称：usart_send
* 功能：串口1发送数据
* 参数：s待发送的数据指针，len待发送的数据长度
* 返回：
* 修改：
* 注释：
*********************************************************************************************/ 
void usart_send(unsigned char *s,unsigned char len){
  for(unsigned char i = 0;i < len;i++)
  {
    USART_SendData(USART1, *(s+i));
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE ) == RESET);
  }
}