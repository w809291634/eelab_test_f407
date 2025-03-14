#include "gt911.h"
#include "iic4.h"
#include "delay.h"
#include "stdio.h"

GT911_point_t GT911Point[GT911_TOUCH_NUM] = {{0}};      // 触摸点位信息存储
unsigned char GT911_pointNum = 0, touchFlag = 0;
void (*GT911_irq_cb)(void);                             //应用层的回调函数

// 触摸引脚初始化
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

// 触摸芯片中断初始化
static void GT911_exti_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct={0};
  EXTI_InitTypeDef   EXTI_InitStructure={0};
  NVIC_InitTypeDef   NVIC_InitStructure={0};
  
  // GPIO 初始化
  RCC_AHB1PeriphClockCmd(GT911_EXTI_CLK, ENABLE);
  GPIO_InitStruct.GPIO_Pin = GT911_EXTI_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
  GPIO_Init(GT911_EXTI_GPIO, &GPIO_InitStruct); 
  
  // EXTI 初始化
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);        //使能SYSCFG时钟
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource2); //PG2 连接到中断线2

  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;           //中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        //上降沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     //使能
  EXTI_Init(&EXTI_InitStructure);                               //按上述参数配置

  NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;              //外部中断号
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;  //抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;         //子优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);   
}

// 外部中断服务函数
void EXTI2_IRQHandler(void)
{
  touchFlag = 1;
  if(GT911_irq_cb != NULL) GT911_irq_cb();                      //有应用层则进行回调
	EXTI_ClearITPendingBit(EXTI_Line2);
}	

// 写寄存器
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

// 读寄存器
static int GT911_readReg(unsigned short reg, unsigned char *data, unsigned char len)
{
  /* 设定读寄存器首地址 */
  I2C4_Start();	
  I2C4_WriteByte(GT911_CMD_WR);  
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg >> 8);       // Register_H
  if(I2C4_WaitAck())return -1;
  I2C4_WriteByte(reg & 0xFF);     // Register_L
  if(I2C4_WaitAck())return -1;
//  I2C4_Stop();

  /* 读取数据 */
  I2C4_Start(); 
  I2C4_WriteByte(GT911_CMD_RD); 
  if(I2C4_WaitAck())return -1;
  for(unsigned char i=0; i<len; i++){
    data[i] = I2C4_ReadByte(i == (len - 1) ? 0 : 1);
  } 
  I2C4_Stop();	
  return len;
}

// 配置函数
int GT911_cfg(void)
{
  unsigned char cfgBuf[14] = {0};
  GT911_writeReg(GT911_CTRL_REG, cfgBuf, 1);    // 写入配置
  GT911_readReg(GT911_PID_REG, cfgBuf, 14);     // 读取PID
  if(cfgBuf[0] == 0x39 && cfgBuf[1] == 0x31 && cfgBuf[2] == 0x31)
    return 0;
  return -1;
}

// GT911触控扫描函数
void GT911_scan(void)
{
  if(touchFlag == 0) return ;
  
  unsigned char pointBuf[GT911_TOUCH_NUM * 8] = {0};    // 存储点位信息
  unsigned char info = 0;
  
  GT911_readReg(GT911_GSTID_REG, &info, 1);
  GT911_pointNum = info & 0x0F;                         // 获取点数量
  if(GT911_pointNum > GT911_TOUCH_NUM)
    GT911_pointNum = GT911_TOUCH_NUM;
  
  /* 有触摸点 */
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
  /* 没有触摸点时为最大值 */
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

// GT911触控初始化函数
int GT911_init(void)
{
  GT911_gpio_init();
  I2C4_IO_Init();
  delay_ms(10);     // 等待
  
  // 设定地址为 0x28/0x29
  GT911_RST_L;
  GT911_EXTI_H;
  delay_ms(1);      // 大于10us
  GT911_RST_H;
  delay_ms(6);      // 大于5ms
  
  GT911_exti_init();// EXTI 转为输入态
  delay_ms(50);     // 延时一段时间再发送配置信息
  return GT911_cfg();
}
