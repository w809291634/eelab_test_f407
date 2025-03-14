#include "extend.h"
#include "sccb.h"
#include "delay.h"

//初始化SCCB接口 
void SCCB_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(SCCB_SCL_LCK|SCCB_SDA_LCK, ENABLE);//使能时钟

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
  GPIO_InitStructure.GPIO_Pin = SCCB_SCL_PIN;
  GPIO_Init(SCCB_SCL_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = SCCB_SDA_PIN;
  GPIO_Init(SCCB_SDA_GPIO, &GPIO_InitStructure);
  
  SCCB_SDA_OUT;
  SCCB_SCL_H;
  SCCB_SDA_H;
}

//SCCB起始信号
//当时钟为高的时候,数据线的高到低,为SCCB起始信号
//在激活状态下,SDA和SCL均为低电平
void SCCB_Start(void)
{
  SCCB_SDA_H;     //数据线高电平	   
  SCCB_SCL_H;	    //在时钟线高的时候数据线由高至低
  delay_us(2);
  SCCB_SDA_L;
  delay_us(2);
  SCCB_SCL_L;	    //数据线恢复低电平，单操作函数必要
}

//SCCB停止信号
//当时钟为高的时候,数据线的低到高,为SCCB停止信号
//空闲状况下,SDA,SCL均为高电平
void SCCB_Stop(void)
{
  SCCB_SDA_L;
  delay_us(2);
  SCCB_SCL_H;
  delay_us(2); 
  SCCB_SDA_H;
  delay_us(2);
}

//产生NA信号
void SCCB_No_Ack(void)
{
  delay_us(2);
  SCCB_SDA_H;
  SCCB_SCL_H;
  delay_us(2);
  SCCB_SCL_L;
  delay_us(2);
  SCCB_SDA_L;
  delay_us(2);
}

//SCCB,写入一个字节
//返回值:0,成功;1,失败. 
u8 SCCB_WR_Byte(u8 dat)
{
  u8 j,res;	 
  for(j=0;j<8;j++)      //循环8次发送数据
  {
    if(dat&0x80)SCCB_SDA_H;	
    else SCCB_SDA_L;
    dat<<=1;
    delay_us(2);
    SCCB_SCL_H;
    delay_us(2);
    SCCB_SCL_L;
  }
  SCCB_SDA_IN;          //设置SDA为输入 
  delay_us(2);
  SCCB_SCL_H;           //接收第九位,以判断是否发送成功
  delay_us(2);
  if(SCCB_SDA_R)res=1;  //SDA=1发送失败，返回1
  else res=0;           //SDA=0发送成功，返回0
  SCCB_SCL_L;
  SCCB_SDA_OUT;         //设置SDA为输出    
  return res;  
}

//SCCB 读取一个字节
//在SCL的上升沿,数据锁存
//返回值:读到的数据
u8 SCCB_RD_Byte(void)
{
  u8 temp=0,j;    
  SCCB_SDA_IN;		//设置SDA为输入  
  for(j=8;j>0;j--) 	//循环8次接收数据
  {		     	  
    delay_us(2);
    SCCB_SCL_H;
    temp=temp<<1;
    if(SCCB_SDA_R)temp++;   
    delay_us(2);
    SCCB_SCL_L;
  }	
  SCCB_SDA_OUT;		//设置SDA为输出    
  return temp;
}

//写寄存器
//返回值:0,成功;1,失败.
u8 SCCB_WR_Reg(u8 reg,u8 data)
{
  u8 res=0;
  SCCB_Start();   //启动SCCB传输
  if(SCCB_WR_Byte(SCCB_ID))res=1;	//写器件ID	  
  delay_us(2);
  if(SCCB_WR_Byte(reg))res=1;		//写寄存器地址	  
  delay_us(2);
  if(SCCB_WR_Byte(data))res=1; 	//写数据	 
  SCCB_Stop();
  return res;
}

//读寄存器
//返回值:读到的寄存器值
u8 SCCB_RD_Reg(u8 reg)
{
  u8 val=0;
  SCCB_Start(); 				//启动SCCB传输
  SCCB_WR_Byte(SCCB_ID);		//写器件ID	  
  delay_us(2);	 
  SCCB_WR_Byte(reg);			//写寄存器地址	  
  delay_us(2);	  
  SCCB_Stop();   
  delay_us(2);	   
  //设置寄存器地址后，才是读
  SCCB_Start();
  SCCB_WR_Byte(SCCB_ID|0X01);	//发送读命令	  
  delay_us(2);
  val=SCCB_RD_Byte();		 	//读取数据
  SCCB_No_Ack();
  SCCB_Stop();
  return val;
}
