/*********************************************************************************************
* 文件: ili9806.c
* 作者：zonesion 2016.12.22
* 说明：
* 功能：LCD驱动代码
* 修改：wanghao  增加适配接口技术综合案例的LCD驱动接口函数2024.2.2
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "extend.h"
#include "text.h"
#include "font.h"
#include "stdio.h"

#define DBG_TAG           "ili9806"
#define DBG_LVL           DBG_INFO
#include <mydbg.h>          // must after of DBG_LVL, DBG_TAG or other options

/* 5寸屏幕分辨率 854 * 480  */
/* LCD的画笔颜色和背景色 */
u16 POINT_COLOR=0x0000;	                                        //画笔颜色
u16 BACK_COLOR=0xFFFF;                                          //背景色 

_lcd_dev lcd_dev;

void LCD_delay(u32 nCount)
{
  u32 i;
  for(i=0;i<72000;i++)
    for(; nCount != 0; nCount--);
}

/*********************************************************************************************
* 名称：LCD_WR_REG
* 功能: 写寄存器函数
* 参数: regval  寄存器的值
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_WR_REG(u16 regval)
{
  *(__IO u16 *) (Bank1_LCD_C) = regval;
}

/*********************************************************************************************
* 名称：LCD_WR_DATA
* 功能: 写数据
* 参数: data  写入的值
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_WR_DATA(u16 data)
{
  *(__IO u16 *) (Bank1_LCD_D) = data;
}

/*********************************************************************************************
* 名称：LCD_RD_DATA
* 功能: 读数据
* 参数: 
* 返回：读到的值
* 修改：
* 注释：
*********************************************************************************************/
u16 LCD_RD_DATA(void)
{
  u16 data;
  data =*(__IO u16 *)Bank1_LCD_D;
  return data;
}

/*********************************************************************************************
* 名称：LCD_WriteRAM_Prepare
* 功能: 开始写GRAM
* 参数: 
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
  LCD_WR_REG(0x2C);                                             //开始写入GRAM
}

/*********************************************************************************************
* 名称：LCD_WriteReg
* 功能: 写寄存器
* 参数: 寄存器地址和数据
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_WriteReg(u16 regval, u16 data)
{
  LCD_WR_REG(regval);
  LCD_WR_DATA(data);
}

/*********************************************************************************************
* 名称：LCD_SetBackLight
* 功能: 设置LCD的背光亮度
* 参数: 背光等级,0~100.越大越亮.
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_SetBackLight(u32 pwm)
{
  TIM_SetCompare1(TIM1, pwm);
}

/*********************************************************************************************
* 名称：LCD_SetCursor
* 功能: 设定光标位置
* 参数: 横纵坐标值
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_SetCursor(u16 x, u16 y)	
{
  LCD_WR_REG(0X2A);                                             /* column address control set */
  LCD_WR_DATA(x>>8);                                            /* 先高8位，然后低8位 */
  LCD_WR_DATA(x&0xff);                                          /* 设置起始点和结束点*/

  LCD_WR_REG(0X2B);                                             /* page address control set */
  LCD_WR_DATA(y>>8);
  LCD_WR_DATA(y&0xff);
}

/*********************************************************************************************
* 名称：LCD_Set_Window
* 功能: 设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
* 参数: sx,sy:窗口起始坐标(左上角)，
        width,height:窗口宽度和高度,必须大于0!!
* 返回：
* 修改：
* 注释：窗体大小:width*height.
*********************************************************************************************/
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height)
{
  u16 twidth, theight;
  twidth = sx + width - 1;
  theight = sy + height - 1;

  LCD_WR_REG(0X2A);
  LCD_WR_DATA(sx >> 8);
  LCD_WR_DATA(sx & 0XFF);
  LCD_WR_DATA(twidth >> 8);
  LCD_WR_DATA(twidth & 0XFF);
  LCD_WR_REG(0X2B);
  LCD_WR_DATA(sy >> 8);
  LCD_WR_DATA(sy & 0XFF);
  LCD_WR_DATA(theight >> 8);
  LCD_WR_DATA(theight & 0XFF);
}

/*********************************************************************************************
* 名称：LCD_ReadPoint
* 功能: 读取某个点的颜色
* 参数: 横纵坐标值
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
u16 LCD_ReadPoint(u16 x, u16 y)
{
  u16 r=0,g=0,b=0,color =0;
  if(x>=lcd_dev.width||y>=lcd_dev.height)
    return 0;                                                   //超过了范围,直接返回
  LCD_SetCursor(x,y);
  LCD_WR_REG(0x2E);                                             //发送读GRAM指令
  r=LCD_RD_DATA();                                              //dummy Read
  LCD_delay(2);
  r=LCD_RD_DATA();                                              //实际坐标颜色
  LCD_delay(2);
  b=LCD_RD_DATA();
  g=r&0XFF;
  g<<=8;
  color = (((r>>11)<<11)|((g>>10)<<5)|(b>>11));
  return color;
}

/*********************************************************************************************
* 名称：LCD_DrawPoint
* 功能: 画点
* 参数: 横纵坐标值
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_DrawPoint(u16 x,u16 y)
{
  LCD_SetCursor(x,y);                                           //设置光标位置 
  LCD_WR_REG(0x2C);                                             //开始写入GRAM
  LCD_WR_DATA(POINT_COLOR);
}

/*********************************************************************************************
* 名称：LCD_Fast_DrawPoint 
* 功能: 快速画点
* 参数: 横纵坐标值
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color)
{
  LCD_WR_REG(0X2A);                                             /* column address control set */
  LCD_WR_DATA(x>>8);                                            /* 先高8位，然后低8位 */
  LCD_WR_DATA(x&0xff);                                          /* 设置起始点和结束点*/

  LCD_WR_REG(0X2B);                                             /* page address control set */
  LCD_WR_DATA(y>>8);
  LCD_WR_DATA(y&0xff);

  LCD_WR_REG(0x2C);                                             //开始写入GRAM
  LCD_WR_DATA(color); 
}

/*********************************************************************************************
* 名称：LCD_Scan_Dir
* 功能: 设置LCD扫描方向
* 参数: 
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_Scan_Dir(u8 dir)
{
  u16 regval=0;
  u16 dirreg=0;
  u16 temp; 
  if(lcd_dev.dir == 1)                                          //横屏显示
  {
    switch(dir)                                                 //方向转换
    {
    case 0:dir=6;break;
    case 1:dir=7;break;
    case 2:dir=4;break;
    case 3:dir=5;break;
    case 4:dir=1;break;
    case 5:dir=0;break;
    case 6:dir=3;break;
    case 7:dir=2;break;
    }
  } 

  switch(dir)
  {
  case L2R_U2D:                                                 //从左到右,从上到下
    regval|=(0<<7)|(0<<6)|(0<<5); 
    break;
  case L2R_D2U:                                                 //从左到右,从下到上
    regval|=(1<<7)|(0<<6)|(0<<5); 
    break;
  case R2L_U2D:                                                 //从右到左,从上到下
    regval|=(0<<7)|(1<<6)|(0<<5); 
    break;
  case R2L_D2U:                                                 //从右到左,从下到上
    regval|=(1<<7)|(1<<6)|(0<<5); 
    break;	 
  case U2D_L2R:                                                 //从上到下,从左到右
    regval|=(0<<7)|(0<<6)|(1<<5); 
    break;
  case U2D_R2L:                                                 //从上到下,从右到左
    regval|=(0<<7)|(1<<6)|(1<<5); 
    break;
  case D2U_L2R:                                                 //从下到上,从左到右
    regval|=(1<<7)|(0<<6)|(1<<5); 
    break;
  case D2U_R2L:                                                 //从下到上,从右到左
    regval|=(1<<7)|(1<<6)|(1<<5); 
    break;
  }
  dirreg=0X36;
  LCD_WriteReg(dirreg,regval);
  if((regval&0X20)||lcd_dev.dir==1)
  {
    if(lcd_dev.width<lcd_dev.height)                            //交换X,Y
    {
      temp=lcd_dev.width;
      lcd_dev.width=lcd_dev.height;
      lcd_dev.height=temp;
    }
  }
  else  
  {
    if(lcd_dev.width>lcd_dev.height)                            //交换X,Y
    {
      temp=lcd_dev.width;
      lcd_dev.width=lcd_dev.height;
      lcd_dev.height=temp;
    }
  }
  LCD_WR_REG(0x2A); 
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  LCD_WR_DATA((lcd_dev.width-1)>>8);
  LCD_WR_DATA((lcd_dev.width-1)&0XFF);
  LCD_WR_REG(0x2B); 
  LCD_WR_DATA(0);
  LCD_WR_DATA(0);
  LCD_WR_DATA((lcd_dev.height-1)>>8);
  LCD_WR_DATA((lcd_dev.height-1)&0XFF);
}

/*********************************************************************************************
* 名称：LCD_Display_Dir
* 功能: 设置LCD显示方向
* 参数: 0  竖屏     1横屏
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_Display_Dir(u8 dir)
{
  if(dir == 0 )	                                                //竖屏
  {
    lcd_dev.dir = 0;
    lcd_dev.width = 480;
    lcd_dev.height = 854;
  }
  else
  {
    lcd_dev.dir = 1;
    lcd_dev.width = 854;
    lcd_dev.height = 480;
  }
  LCD_Scan_Dir(L2R_U2D);                                        //默认扫描方向
}

/*********************************************************************************************
* 名称：LCD_GPIO_Config
* 功能: LCD引脚配置
* 参数:
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure={0};

  /* Enable the FSMC Clock and  lcd gpio clock base on FSMC */
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF, ENABLE);
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  /* LCD BL 背光开关控制引脚*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  /* FSMC控制LCD的16根数据线 FSMC-D0~D15: PD 0 1 8 9 10 14 15,  PE 7 8 9 10 11 12 13 14 15, */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
  /* 配置LCD的控制线 */
  /* PD4-FSMC_NOE  :LCD-RD  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  /* PD5-FSMC_NWE  :LCD-WR */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  /* PD7-FSMC_NE1  :LCD-CS */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);  
  /* PD13-FSMC_A18 :LCD-RS */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; 
  GPIO_Init(GPIOD, &GPIO_InitStructure);  
  
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);
  
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC); 
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC); 
  
  /* 打开背光 */
  GPIO_SetBits(GPIOF, GPIO_Pin_5);                              //  BL = 1
}

/*********************************************************************************************
* 名称：LCD_FSMC_Config
* 功能: FSMC模式配置
* 参数:
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
static void LCD_FSMC_Config(uint32_t FSMC_Bank)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
  FSMC_NORSRAMTimingInitTypeDef  writeTiming;
  
  /* 读时序 */
  readWriteTiming.FSMC_AddressSetupTime = 0x01;                 //地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*1
  readWriteTiming.FSMC_AddressHoldTime = 0x00;                  //地址保持时间（ADDHLD）模式A未用到
  readWriteTiming.FSMC_DataSetupTime = 0x08;                    //数据保存时间为1个HCLK，GD32F4至少设置1
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;          //模式A 

  /* 写时序 */
  writeTiming.FSMC_AddressSetupTime =0x01;                      //地址建立时间（ADDSET）为1个HCLK
  writeTiming.FSMC_AddressHoldTime = 0x00;                      //地址保持时间
  writeTiming.FSMC_DataSetupTime = 0x01;                        //数据保存时间为1个HCLK
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;              //模式A 

  FSMC_NORSRAMStructInit(&FSMC_NORSRAMInitStructure);
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank;                            // 这里我们使用NEx
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;// 不复用数据地址
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;            // FSMC_MemoryType_SRAM;  //SRAM   
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;  //存储器数据宽度为16bit   
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable; //不使能突发
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;          //不使能突发
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;              //不使能突发
  
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;  // 运行异步协议时不考虑 NWAIT 信号
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//NWAIT 信号在等待周期之前的一个数据周期有效
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable; //存储器写使能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;     //读写使用不同的时序
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;    //读写时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;            //写时序,使能扩展时有效

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);                 //初始化FSMC配置

  FSMC_NORSRAMCmd(FSMC_Bank, ENABLE);                           // 使能BANKx
}

/*********************************************************************************************
* 名称：TFTLCD_Init
* 功能: Lcd初始化函数
* 参数: 填充的颜色和位置	
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void TFTLCD_Init(void)
{	
  LCD_GPIO_Config();
  LCD_FSMC_Config(FSMC_Bank1_NORSRAM1);
  LCD_delay(20);
  
  LCD_WR_REG(0XD3); 
  lcd_dev.id = LCD_RD_DATA();                                   //dummy read
  lcd_dev.id = LCD_RD_DATA();                                   //读到0x00
  lcd_dev.id = LCD_RD_DATA();                                   //读到0x98
  
  lcd_dev.id <<=8;
  lcd_dev.id  |= LCD_RD_DATA();	                                //读到0x06
  
  LCD_WR_REG(0xFF);                                             // EXTC Command Set enable register
  LCD_WR_DATA(0xFF);
  LCD_WR_DATA(0x98);
  LCD_WR_DATA(0x06);
  
  LCD_WR_REG(0xBA);                                             // SPI Interface Setting
  LCD_WR_DATA(0x60);
  
  LCD_WR_REG(0xBC);                                             // GIP 1
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x10);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x11);
  LCD_WR_DATA(0x32);
  LCD_WR_DATA(0x10);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x50);
  LCD_WR_DATA(0x52);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x40);
  
  LCD_WR_REG(0xBD);                                             // GIP 2
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x23);
  LCD_WR_DATA(0x45);
  LCD_WR_DATA(0x67);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x23);
  LCD_WR_DATA(0x45);
  LCD_WR_DATA(0x67);
  
  LCD_WR_REG(0xBE);                                             // GIP 3
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x21);
  LCD_WR_DATA(0xAB);
  LCD_WR_DATA(0x60);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x22);
  
  LCD_WR_REG(0xC7);                                             // VCOM Control
  LCD_WR_DATA(0x30);
  
  LCD_WR_REG(0xED);                                             // EN_volt_reg
  LCD_WR_DATA(0x7F);
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x00);
  
  LCD_WR_REG(0xC0);                                             // Power Control 1
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x0C);                                            //0A  VGH VGL
  
  LCD_WR_REG(0xFD);                                             //External Power Selection Set
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x00);
  
  LCD_WR_REG(0xFC);                                             // LVGL
  LCD_WR_DATA(0x08);
  
  LCD_WR_REG(0xDF);                                             // Engineering Setting
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x20);
  
  LCD_WR_REG(0xF3);                                             // DVDD Voltage Setting
  LCD_WR_DATA(0x74);
  
  LCD_WR_REG(0xB4);                                             // Display Inversion Control
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  
  LCD_WR_REG(0xB5);                                             //Blanking Porch Control
  LCD_WR_DATA(0x08);
  LCD_WR_DATA(0x15);
  
  LCD_WR_REG(0xF7);                                             // 480x854
  LCD_WR_DATA(0x81);
  
  LCD_WR_REG(0xB1);                                             // Frame Rate Control
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x13);
  LCD_WR_DATA(0x13);
  
  LCD_WR_REG(0xF2);                                             //Panel Timing Control
  LCD_WR_DATA(0x80);
  LCD_WR_DATA(0x04);
  LCD_WR_DATA(0x40);
  LCD_WR_DATA(0x28);
  
  LCD_WR_REG(0xC1);                                             //Power Control 2
  LCD_WR_DATA(0x17);
  LCD_WR_DATA(0x71);                                            //VGMP
  LCD_WR_DATA(0x71);                                            //VGMN
  
  LCD_WR_REG(0xE0);                                             //P_Gamma
  LCD_WR_DATA(0x00);                                            //P1
  LCD_WR_DATA(0x13);                                            //P2
  LCD_WR_DATA(0x1A);                                            //P3
  LCD_WR_DATA(0x0C);                                            //P4
  LCD_WR_DATA(0x0E);                                            //P5
  LCD_WR_DATA(0x0B);                                            //P6
  LCD_WR_DATA(0x07);                                            //P7
  LCD_WR_DATA(0x05);                                            //P8
  LCD_WR_DATA(0x05);                                            //P9
  LCD_WR_DATA(0x0A);                                            //P10
  LCD_WR_DATA(0x0F);                                            //P11
  LCD_WR_DATA(0x0F);                                            //P12
  LCD_WR_DATA(0x0E);                                            //P13
  LCD_WR_DATA(0x1C);                                            //P14
  LCD_WR_DATA(0x16);                                            //P15
  LCD_WR_DATA(0x00);                                            //P16
  
  LCD_WR_REG(0xE1);                                             //N_Gamma
  LCD_WR_DATA(0x00);                                            //P1
  LCD_WR_DATA(0x13);                                            //P2
  LCD_WR_DATA(0x1A);                                            //P3
  LCD_WR_DATA(0x0C);                                            //P4
  LCD_WR_DATA(0x0E);                                            //P5
  LCD_WR_DATA(0x0B);                                            //P6
  LCD_WR_DATA(0x07);                                            //P7
  LCD_WR_DATA(0x05);                                            //P8
  LCD_WR_DATA(0x05);                                            //P9
  LCD_WR_DATA(0x0A);                                            //P10
  LCD_WR_DATA(0x0F);                                            //P11
  LCD_WR_DATA(0x0F);                                            //P12
  LCD_WR_DATA(0x0E);                                            //P13
  LCD_WR_DATA(0x1C);                                            //P14
  LCD_WR_DATA(0x16);                                            //P15
  LCD_WR_DATA(0x00);                                            //P16
  
  LCD_WR_REG(0x3A);
  LCD_WR_DATA(0x55);                                            //55-16BIT,66-18BIT,77-24BIT
  
  LCD_WR_REG(0x36);                                             //Memory Access Control
  LCD_WR_DATA(0x00);                                            //01-180

  LCD_WR_REG(0x2A);                                             //480
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0xdf);
  
  LCD_WR_REG(0x2B);                                             //854
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0x55);
  
  LCD_WR_REG(0x11);
  LCD_delay(120);
  LCD_WR_REG(0x29);
  LCD_delay(25);
  LCD_WR_REG(0x2C);
  
  LCD_delay(10);
  Lcd_Light_ON;                                                 //开启背光
  LCD_Display_Dir(1);                                           //0竖屏  1横屏
  LCD_Clear(BLACK);
}  

/*********************************************************************************************
* 名称：LCD_Fill
* 功能: 清屏函数
* 参数: 填充的颜色和位置
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_Clear(u16 color)
{
  u16 x=0,y=0;
  u32 index=0;
  u32 totalpoint = lcd_dev.width;
  totalpoint *= lcd_dev.height;

  LCD_WR_REG(0X2A);                                             /* column address control set */
  LCD_WR_DATA(x>>8);                                            /* 先高8位，然后低8位 */
  LCD_WR_DATA(x&0xff);                                          /* 设置起始点和结束点*/
  LCD_WR_DATA((lcd_dev.width) >>8);
  LCD_WR_DATA((lcd_dev.width) &0xff);

  LCD_WR_REG(0X2B);                                             /* page address control set */
  LCD_WR_DATA(y>>8);
  LCD_WR_DATA(y&0xff);
  LCD_WR_DATA((lcd_dev.height) >>8);
  LCD_WR_DATA((lcd_dev.height) &0xff);

  LCD_WR_REG(0x2C);                                             //写入GRAM

  for( index=0;index < totalpoint ; index++ )
  {
    LCD_WR_DATA( color );
  }
}

/*********************************************************************************************
* 名称：LCD_Fill
* 功能: 填充函数
* 参数: 填充的颜色和位置	
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_Fill(u16 x, u16 y, u16 width, u16 height, u16 color)
{
  u32 i = 0;

  LCD_WR_REG(0X2A);
  LCD_WR_DATA( x >> 8 );                                        /* 先高8位，然后低8位 */
  LCD_WR_DATA( x & 0xff );                                      /* column start   */ 
  LCD_WR_DATA( (x+width-1) >> 8 );                              /* column end   */
  LCD_WR_DATA( (x+width-1) & 0xff );

  LCD_WR_REG(0X2B);
  LCD_WR_DATA( y >> 8 );                                        /* page start   */
  LCD_WR_DATA( y & 0xff );
  LCD_WR_DATA( (y+height-1) >> 8);                              /* page end     */
  LCD_WR_DATA( (y+height-1) & 0xff);

  LCD_WR_REG(0x2c);                                             /* memory write */

  for( i=0; i < width*height; i++ )
  {
    LCD_WR_DATA( color );
  }
}

/*********************************************************************************************
* 名称：void LCD_Fill_s(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
* 功能：在指定区域内填充指定颜色
* 参数：sx,sy:填充数据起始坐标
        ex,ey:填充数据结束坐标
        color:要填充的颜色
* 返回：无
* 修改：
* 注释：区域大小:(ex-sx+1)*(ey-sy+1)
*********************************************************************************************/
void LCD_Fill_s(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
  u16 i, j;
  u16 xlen = 0;

  xlen = ex - sx + 1;

  for (i = sy; i <= ey; i++){
    LCD_SetCursor(sx, i);                                       //设置光标位置
    LCD_WriteRAM_Prepare();                                     //开始写入GRAM
    for (j = 0; j < xlen; j++){
      LCD_WR_DATA( color );
    }
  }
}

/*********************************************************************************************
* 名称：LCD_DrawLine
* 功能: 画水平线
* 参数: x1,y1  线起点位置、x2 y2  线终点位置
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
  u16 t; 
  int xerr=0,yerr=0,delta_x,delta_y,distance; 
  int incx,incy,uRow,uCol; 
  delta_x=x2-x1;                                                //计算坐标增量 
  delta_y=y2-y1; 
  uRow=x1; 
  uCol=y1; 
  if(delta_x>0)incx=1;                                          //设置单步方向 
  else if(delta_x==0)incx=0;                                    //垂直线 
  else {incx=-1;delta_x=-delta_x;} 
  if(delta_y>0)incy=1; 
  else if(delta_y==0)incy=0;                                    //水平线 
  else{incy=-1;delta_y=-delta_y;} 
  if( delta_x>delta_y)distance=delta_x;                         //选取基本增量坐标轴 
  else distance=delta_y; 
  for(t=0;t<=distance+1;t++ )                                   //画线输出 
  {  
    LCD_DrawPoint(uRow, uCol);                                  //画点 
    xerr+=delta_x ; 
    yerr+=delta_y ; 
    if(xerr>distance) 
    { 
      xerr-=distance; 
      uRow+=incx; 
    } 
    if(yerr>distance) 
    { 
      yerr-=distance; 
      uCol+=incy; 
    } 
  }  
}  

/*********************************************************************************************
* 名称：LCD_DrawRectangle
* 功能: 画矩形
* 参数: 矩形对角坐标
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

/*********************************************************************************************
* 名称：LCD_Draw_Circle
* 功能: 画圆
* 参数: 原点坐标和半径
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCD_Draw_Circle(u16 x,u16 y,u8 r)
{
  int a,b;
  int di;
  a=0;b=r;
  di=3-(r<<1);                                                  //判断下个点位置的标志
  while(a<=b)
  {
    LCD_DrawPoint(x+a,y-b);                                     //5
    LCD_DrawPoint(x+b,y-a);                                     //0
    LCD_DrawPoint(x+b,y+a);                                     //4
    LCD_DrawPoint(x+a,y+b);                                     //6 
    LCD_DrawPoint(x-a,y+b);                                     //1
    LCD_DrawPoint(x-b,y+a);             
    LCD_DrawPoint(x-a,y-b);                                     //2
    LCD_DrawPoint(x-b,y-a);                                     //7
    a++;
    /*使用Bresenham算法画圆 */
    if(di<0)di +=4*a+6;	  
    else
    {
      di+=10+4*(a-b);   
      b--;
    }
  }
} 

/*********************************************************************************************
* 名称：LCD_ShowChar
* 功能: 显示一个字符
* 参数: 位置、字符、大小
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{
  u8 temp,t1,t;
  u16 y0=y;
  u8 csize=(size/8+((size%8)?1:0))*(size/2);                    //得到字体一个字符对应点阵集所占的字节数	
  num=num-' ';                                                  //得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
  for(t=0;t<csize;t++)
  {
    if(size==12)temp=tft_asc2_1206[num][t];                     //调用1206字体
    else if(size==16)temp=tft_asc2_1608[num][t];                //调用1608字体
    else if(size==24)temp=tft_asc2_2412[num][t];                //调用2412字体
    else if(size==40)temp=tft_asc2_4020[num][t];                //调用4020字体
    else return;                                                //没有的字库
    for(t1=0;t1<8;t1++)
    {
      if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
      else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
      temp<<=1;
      y++;
      if(y>=lcd_dev.height)return;                              //超区域了
      if((y-y0)==size)
      {
        y=y0;
        x++;
        if(x>=lcd_dev.width)return;                             //超区域了
        break;
      }
    }
  }
}

/*********************************************************************************************
* 名称：LCDDrawChar
* 功能：显示一个字符 ，适配 应用层     
* 参数：x，y：显示坐标，num：字符，color：字符颜色，bc：背景色
* 返回：无
* 修改：
* 注释：绘制格式：阴码+逐列式+顺向+C51格式
**********************************************************************************************/
void LCDDrawChar(int x, int y, char num, u8 size,int color, int bc)
{
  u8 temp,t1,t;
  u16 y0=y;
  u8 csize=(size/8+((size%8)?1:0))*(size/2);                    //得到字体一个字符对应点阵集所占的字节数	
  num=num-' ';
  /* num:行  t:列  具体字节 */
  for(t=0;t<csize;t++)
  {   
    /* 取具体的字节 */
    if(size==12)temp=tft_asc2_1206[num][t];                     //调用1206字体
    else if(size==16)temp=tft_asc2_1608[num][t];                //调用1608字体
    else if(size==24)temp=tft_asc2_2412[num][t];                //调用2412字体
    else if(size==32)temp=tft_asc2_3216[num][t];                //调用3216字体
    else if(size==40)temp=tft_asc2_4020[num][t];                //调用4020字体
    else {
      printf("LCDDrawChar No font library\r\n");
      return;                                                   //没有的字库
    }
    /* 绘制字体 */
    for(t1=0;t1<8;t1++)
    {
      if(temp&0x80)LCD_Fast_DrawPoint(x,y,color);
      else if(bc>0)LCD_Fast_DrawPoint(x,y,bc);
      temp<<=1;
      y++;
      if(y>=lcd_dev.height)return;                              //超区域了
      /* 切换到下一列 */
      if((y-y0)==size)
      {
        y=y0;
        x++;
        if(x>=lcd_dev.width)return;                             //超区域了
        break;
      }
    }
  }
}

/*********************************************************************************************
* 名称：LCD_Pow
* 功能: 幂函数
* 参数: 
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
u32 LCD_Pow(u8 m,u8 n)
{
  u32 result=1;
  while(n--)result*=m;
  return result;
}

/*********************************************************************************************
* 名称：LCD_ShowNum
* 功能: 显示数字
* 参数: 位置、数值、长度、大小
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)
{
  u8 t,temp;
  u8 enshow=0;
  for(t=0;t<len;t++)
  {
    temp=(num/LCD_Pow(10,len-t-1))%10;
    if(enshow==0&&t<(len-1))
    {
      if(temp==0)
      {
        LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
        continue;
      }else enshow=1; 
    }
    LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
  }
} 

/*********************************************************************************************
* 名称：LCD_ShowString
* 功能: 显示字符串
* 参数: 位置、数值、颜色、大小
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{
  u8 x0=x;
  width+=x;
  height+=y;
  while((*p<='~')&&(*p>=' '))                                   // 判断是不是非法字符!
  {
    if(x>=width){x=x0;y+=size;}                                 // 换行
    if(y>=height)break;                                         // 退出
    LCD_ShowChar(x,y,*p,size,0);
    x+=size/2;
    p++;
  }  
}

/*********************************************************************************************
* 名称：LCD_ShowString_Mid
* 功能：lcd在指定宽度居中显示字符
* 参数：x,y:起始坐标值,str 字符，len绘制宽度
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_ShowString_Mid(u16 x,u16 y,u16 len,u8 size,char *str)
{
  u16 usesize;

  /* 获取多少个字符 */
  char *p=str;
  u8 str_num=0;
  while(*p!=0){ 
    if(*p<0x80){
      p++;
      str_num++;
    }
    /* 中文 */
    else return;
  }
  usesize = size/2*str_num ;                                    // 当前显示文本占用像素

  if(usesize>len){
    printf("LCD_ShowString_Mid parameter error!\r\n");
    return;
  }
  usesize=(len-usesize)/2;                                      // 起点偏移量
  LCD_ShowString(usesize+x,y,len,size,size,(u8*)str);
}


/*********************************************************************************************
* 名称：void LCD_FillColor(short x1,short y1,short x2,short y2,uint16_t color)
* 功能：在LCD指定区域填充同类颜色
* 参数：x1,y1:填充数据起始坐标
        x2,y2:填充数据结束坐标
        color:填充数据
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCD_FillColor(short x1,short y1,short x2,short y2,uint16_t color)
{
  uint16_t x, y;
  LCD_Set_Window(x1,y1,x2-x1,y2-y1);
  LCD_WriteRAM_Prepare();
  for (y=y1; y<=y2; y++){
    for (x=x1; x<=x2; x++){
      LCD_WR_DATA(color);
    }
  }
}

/*********************************************************************************************
* 名称：LCD_FillData
* 功能：在LCD指定区域填充缓存颜色数据
* 参数：x1:填充数据的起始坐标x
        y1:填充数据的起始坐标y
        width:填充数据宽度
        height:填充数据高度
        dat:填充数据指针，指向一个数据缓存区域，数据长度和width、height相关
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCD_FillData(short x1,short y1,short width,short height,unsigned short* dat)
{
  u16 i, j;

#if 0
  /* 不改变填充窗口方式 */
  dat = dat+2;                                                  // 在填充图片数据时，前面两位数据存放图像高度和宽度数据
  for (i = 0; i < height; i++){
    LCD_SetCursor(x1, y1+i);                                    // 设置光标位置
    LCD_WriteRAM_Prepare();                                     // 开始写入GRAM
    for (j = 0; j < width; j++){
      LCD_WR_DATA(*dat++);
    }
  }
#else
  /* 改变填充窗口方式，速度相对较快，
  * 但是部分函数需要注意还原LCD填充窗口大小 
  */
  LCD_Set_Window(x1,y1,width,height);
  LCD_WriteRAM_Prepare();
  for (i=0; i < height; i++){                                   // 行
    for (j=0; j < width; j++){                                  // 列
      LCD_WR_DATA(dat[j+i*width+2]);
    }
  }
  //LCD_Set_Window(0,0,LCD_WIDTH,LCD_HEIGHT);
#endif
}

/*********************************************************************************************
* 名称：LCD_FillData_ex
* 功能：在LCD指定区域填充缓存颜色数据
* 参数：x1:填充数据的起始坐标x
        y1:填充数据的起始坐标y
        width:填充数据宽度
        height:填充数据高度
        dat:填充数据指针，指向一个数据缓存区域，数据长度和width、height相关
        multiple:放大倍数
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void LCD_FillData_ex(short x1,short y1,short width,short height,unsigned short* dat,unsigned char multiple)
{
  /* 改变填充窗口方式，速度相对较快，
  * 但是部分函数需要注意还原LCD填充窗口大小 
  */
  if(!multiple){
    LOG_E("multiple parameter error!");
    return;
  }
  
  LCD_Set_Window(x1,y1,width*multiple,height*multiple);
  LCD_WriteRAM_Prepare();
  for (u16 i=0; i < height; i++){                               // 行
    for(u8 m=0; m < multiple; m++){                             // 在这行中，每一行填充几次，由放大倍数决定
      for (u16 j=0; j < width; j++){                            // 列
        for(u8 n=0; n < multiple; n++){                         // 在填充一行中的一个点时，每个点填充几次，由放大倍数决定
          LCD_WR_DATA(dat[j+i*width+2]);
        }
      }
    }
  }
  //LCD_Set_Window(0,0,LCD_WIDTH,LCD_HEIGHT);
}

/*********************************************************************************************
* 名称:LCD_BLCtrl
* 功能:控制LCD背光灯的亮灭
* 参数:cmd控制参数
* 返回:
* 修改:
* 注释:
*********************************************************************************************/
void LCD_BLCtrl(unsigned char cmd)
{
  if (cmd)
  {
    Lcd_Light_ON;
  }
  else
  {
    Lcd_Light_OFF;
  }
}
