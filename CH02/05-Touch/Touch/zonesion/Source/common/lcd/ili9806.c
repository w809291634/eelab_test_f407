#include "stm32f4xx.h"
#include "text.h"
#include "font.h"
#include "stdio.h"

/* 5����Ļ�ֱ��� 854 * 480  */

//LCD�Ļ�����ɫ�ͱ���ɫ	   
u16 POINT_COLOR=0x0000;	//������ɫ
u16 BACK_COLOR=0xFFFF;  //����ɫ 

_lcd_dev lcd_dev;

void LCD_delay(u32 nCount)
{
	u32 i;
	for(i=0;i<72000;i++)
		for(; nCount != 0; nCount--);
}

/*
****************************************************************
*����: д�Ĵ�������
*����: regval  �Ĵ�����ֵ
****************************************************************
*/
void LCD_WR_REG(u16 regval)
{   
	*(__IO u16 *) (Bank1_LCD_C) = regval;		 
}

/*
****************************************************************
*����: д����
*����: data  д���ֵ
****************************************************************
*/
void LCD_WR_DATA(u16 data)
{
	*(__IO u16 *) (Bank1_LCD_D) = data;	
}

/*
****************************************************************
*����: ������
*����ֵ: ������ֵ
****************************************************************
*/
u16 LCD_RD_DATA(void)
{
	u16 data;
	data =*(__IO u16 *)Bank1_LCD_D;
	return data;
}

/*
****************************************************************
*����: д�Ĵ���
*����: �Ĵ�����ַ������
****************************************************************
*/
void LCD_WriteReg(u16 regval, u16 data)
{
	LCD_WR_REG(regval);
	LCD_WR_DATA(data);
}

/*
****************************************************************
*����: ����LCD�ı�������
*����: ����ȼ�,0~100.Խ��Խ��.
****************************************************************
*/
void LCD_SetBackLight(u32 pwm)
{	
	TIM_SetCompare1(TIM1, pwm);
}

/*
****************************************************************
*����: �趨���λ��
*����: ��������ֵ	
****************************************************************
*/
void LCD_SetCursor(u16 x, u16 y)	
{
	LCD_WR_REG(0X2A);		/* column address control set */
	LCD_WR_DATA(x>>8);	/* �ȸ�8λ��Ȼ���8λ */
	LCD_WR_DATA(x&0xff);	/* ������ʼ��ͽ�����*/
  
	LCD_WR_REG(0X2B);		/* page address control set */
	LCD_WR_DATA(y>>8);
	LCD_WR_DATA(y&0xff);		
}

/*
****************************************************************
*����: ��ȡĳ�������ɫ
*����: ��������ֵ
****************************************************************
*/
u16 LCD_ReadPoint(u16 x, u16 y)
{
	u16 r=0,g=0,b=0,color =0;
	if(x>=lcd_dev.width||y>=lcd_dev.height)
    return 0;	//�����˷�Χ,ֱ�ӷ���		   
	LCD_SetCursor(x,y);	
	LCD_WR_REG(0x2E);		//���Ͷ�GRAMָ��
	r=LCD_RD_DATA();		//dummy Read	   
	LCD_delay(2);	
	r=LCD_RD_DATA();		//ʵ��������ɫ
	LCD_delay(2);	  
	b=LCD_RD_DATA(); 
	g=r&0XFF;		
	g<<=8;
	color = (((r>>11)<<11)|((g>>10)<<5)|(b>>11));
	return color;
}

/*
****************************************************************
*����: ����
*����: ��������ֵ
****************************************************************
*/
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);		//���ù��λ�� 
	LCD_WR_REG(0x2C);		//��ʼд��GRAM
	LCD_WR_DATA(POINT_COLOR);     
}

/*
****************************************************************
*����: ���ٻ���
*����: ��������ֵ
****************************************************************
*/

void LCD_Fast_DrawPoint(u16 x, u16 y, u16 color)
{
	LCD_WR_REG(0X2A);		/* column address control set */
	LCD_WR_DATA(x>>8);	/* �ȸ�8λ��Ȼ���8λ */
	LCD_WR_DATA(x&0xff);	/* ������ʼ��ͽ�����*/
  
	LCD_WR_REG(0X2B);		/* page address control set */
	LCD_WR_DATA(y>>8);
	LCD_WR_DATA(y&0xff);	
  
	LCD_WR_REG(0x2C);		//��ʼд��GRAM
	LCD_WR_DATA(color); 
}

/*
****************************************************************
*����: ����LCDɨ�跽��
*����: 
****************************************************************
*/
void LCD_Scan_Dir(u8 dir)
{
	u16 regval=0;
	u16 dirreg=0;
	u16 temp; 
	if(lcd_dev.dir == 1)//������ʾ
	{			   
		switch(dir)//����ת��
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
  case L2R_U2D://������,���ϵ���
    regval|=(0<<7)|(0<<6)|(0<<5); 
    break;
  case L2R_D2U://������,���µ���
    regval|=(1<<7)|(0<<6)|(0<<5); 
    break;
  case R2L_U2D://���ҵ���,���ϵ���
    regval|=(0<<7)|(1<<6)|(0<<5); 
    break;
  case R2L_D2U://���ҵ���,���µ���
    regval|=(1<<7)|(1<<6)|(0<<5); 
    break;	 
  case U2D_L2R://���ϵ���,������
    regval|=(0<<7)|(0<<6)|(1<<5); 
    break;
  case U2D_R2L://���ϵ���,���ҵ���
    regval|=(0<<7)|(1<<6)|(1<<5); 
    break;
  case D2U_L2R://���µ���,������
    regval|=(1<<7)|(0<<6)|(1<<5); 
    break;
  case D2U_R2L://���µ���,���ҵ���
    regval|=(1<<7)|(1<<6)|(1<<5); 
    break;	
	}
	dirreg=0X36;
	LCD_WriteReg(dirreg,regval);
	if((regval&0X20)||lcd_dev.dir==1)
	{
		if(lcd_dev.width<lcd_dev.height)//����X,Y
		{
			temp=lcd_dev.width;
			lcd_dev.width=lcd_dev.height;
			lcd_dev.height=temp;
 		}
	}
	else  
	{
		if(lcd_dev.width>lcd_dev.height)//����X,Y
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

/*
****************************************************************
*����: ����LCD��ʾ����
*����: 0  ����     1����
****************************************************************
*/
void LCD_Display_Dir(u8 dir)
{
	if(dir == 0 )	//����
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
	LCD_Scan_Dir(L2R_U2D);	//Ĭ��ɨ�跽��
}

/*
****************************************************************
*����: LCD��������
****************************************************************
*/
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
  
  /* LCD BL ���⿪�ؿ�������*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOF, &GPIO_InitStructure);
  
  /* FSMC����LCD��16�������� FSMC-D0~D15: PD 0 1 8 9 10 14 15,  PE 7 8 9 10 11 12 13 14 15, */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure); 
  
  /* ����LCD�Ŀ����� */
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
  
  /* �򿪱��� */
  GPIO_SetBits(GPIOF, GPIO_Pin_5);          //  BL = 1
}

/*
****************************************************************
*����: FSMCģʽ����
****************************************************************
*/
static void LCD_FSMC_Config(uint32_t FSMC_Bank)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
  FSMC_NORSRAMTimingInitTypeDef  writeTiming;

  readWriteTiming.FSMC_AddressSetupTime = 0x01;   //��ַ����ʱ�䣨ADDSET��Ϊ16��HCLK 1/168M=6ns*1
  readWriteTiming.FSMC_AddressHoldTime = 0x00;    //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
  readWriteTiming.FSMC_DataSetupTime = 0x01;      //���ݱ���ʱ��Ϊ1��HCLK��GD32F4��������1
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;  //ģʽA 

  writeTiming.FSMC_AddressSetupTime =0x01;        //��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK
  writeTiming.FSMC_AddressHoldTime = 0x00;        //��ַ����ʱ��
  writeTiming.FSMC_DataSetupTime = 0x01;          //���ݱ���ʱ��Ϊ1��HCLK
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;      //ģʽA 

  FSMC_NORSRAMStructInit(&FSMC_NORSRAMInitStructure);
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank;                            // ��������ʹ��NEx
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;// ���������ݵ�ַ
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;            // FSMC_MemoryType_SRAM;  //SRAM   
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;  //�洢�����ݿ��Ϊ16bit   
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable; //��ʹ��ͻ��
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;          //��ʹ��ͻ��
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;              //��ʹ��ͻ��
  
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;  // �����첽Э��ʱ������ NWAIT �ź�
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;//NWAIT �ź��ڵȴ�����֮ǰ��һ������������Ч
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable; //�洢��дʹ��
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;     //��дʹ�ò�ͬ��ʱ��
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;    //��дʱ��
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;            //дʱ��,ʹ����չʱ��Ч

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);         //��ʼ��FSMC����

  FSMC_NORSRAMCmd(FSMC_Bank, ENABLE);                   // ʹ��BANKx
}

/**********************************************
Lcd��ʼ������
***********************************************/
void TFTLCD_Init(void)
{	
  LCD_GPIO_Config();
  LCD_FSMC_Config(FSMC_Bank1_NORSRAM1);
  LCD_delay(20);
  
  LCD_WR_REG(0XD3); 
  lcd_dev.id = LCD_RD_DATA();		//dummy read
  lcd_dev.id = LCD_RD_DATA();		//����0x00
  lcd_dev.id = LCD_RD_DATA();		//����0x98
  
  lcd_dev.id <<=8;
  lcd_dev.id  |= LCD_RD_DATA();	//����0x06
  
  //************* Start Initial Sequence 
  LCD_WR_REG(0xFF); // EXTC Command Set enable register
  LCD_WR_DATA(0xFF);
  LCD_WR_DATA(0x98);
  LCD_WR_DATA(0x06);
  
  LCD_WR_REG(0xBA); // SPI Interface Setting
  LCD_WR_DATA(0x60);
  
  LCD_WR_REG(0xBC); // GIP 1
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
  
  LCD_WR_REG(0xBD); // GIP 2
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x23);
  LCD_WR_DATA(0x45);
  LCD_WR_DATA(0x67);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0x23);
  LCD_WR_DATA(0x45);
  LCD_WR_DATA(0x67);
  
  LCD_WR_REG(0xBE); // GIP 3
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x21);
  LCD_WR_DATA(0xAB);
  LCD_WR_DATA(0x60);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x22);
  LCD_WR_DATA(0x22);
  
  LCD_WR_REG(0xC7); // VCOM Control
  LCD_WR_DATA(0x30);
  
  LCD_WR_REG(0xED); // EN_volt_reg
  LCD_WR_DATA(0x7F);
  LCD_WR_DATA(0x0F);
  LCD_WR_DATA(0x00);
  
  LCD_WR_REG(0xC0); // Power Control 1
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0x0B);
  LCD_WR_DATA(0x0C);//0A  VGH VGL
  
  LCD_WR_REG(0xFD);//External Power Selection Set
  LCD_WR_DATA(0x0A);
  LCD_WR_DATA(0x00);
  
  LCD_WR_REG(0xFC); // LVGL
  LCD_WR_DATA(0x08);
  
  LCD_WR_REG(0xDF); // Engineering Setting
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x20);
  
  LCD_WR_REG(0xF3); // DVDD Voltage Setting
  LCD_WR_DATA(0x74);
  
  LCD_WR_REG(0xB4); // Display Inversion Control
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  
  LCD_WR_REG(0xB5); //Blanking Porch Control
  LCD_WR_DATA(0x08);
  LCD_WR_DATA(0x15);
  
  LCD_WR_REG(0xF7); // 480x854
  LCD_WR_DATA(0x81);
  
  LCD_WR_REG(0xB1); // Frame Rate Control
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x13);
  LCD_WR_DATA(0x13);
  
  LCD_WR_REG(0xF2); //Panel Timing Control
  LCD_WR_DATA(0x80);
  LCD_WR_DATA(0x04);
  LCD_WR_DATA(0x40);
  LCD_WR_DATA(0x28);
  
  LCD_WR_REG(0xC1); //Power Control 2
  LCD_WR_DATA(0x17);
  LCD_WR_DATA(0x71);//VGMP
  LCD_WR_DATA(0x71); //VGMN
  
  LCD_WR_REG(0xE0);        //P_Gamma
  LCD_WR_DATA(0x00); //P1
  LCD_WR_DATA(0x13); //P2
  LCD_WR_DATA(0x1A); //P3
  LCD_WR_DATA(0x0C); //P4
  LCD_WR_DATA(0x0E); //P5
  LCD_WR_DATA(0x0B); //P6
  LCD_WR_DATA(0x07); //P7
  LCD_WR_DATA(0x05); //P8
  LCD_WR_DATA(0x05); //P9
  LCD_WR_DATA(0x0A); //P10
  LCD_WR_DATA(0x0F); //P11
  LCD_WR_DATA(0x0F); //P12
  LCD_WR_DATA(0x0E); //P13
  LCD_WR_DATA(0x1C); //P14
  LCD_WR_DATA(0x16); //P15
  LCD_WR_DATA(0x00); //P16
  
  LCD_WR_REG(0xE1);        //N_Gamma
  LCD_WR_DATA(0x00); //P1
  LCD_WR_DATA(0x13); //P2
  LCD_WR_DATA(0x1A); //P3
  LCD_WR_DATA(0x0C); //P4
  LCD_WR_DATA(0x0E); //P5
  LCD_WR_DATA(0x0B); //P6
  LCD_WR_DATA(0x07); //P7
  LCD_WR_DATA(0x05); //P8
  LCD_WR_DATA(0x05); //P9
  LCD_WR_DATA(0x0A); //P10
  LCD_WR_DATA(0x0F); //P11
  LCD_WR_DATA(0x0F); //P12
  LCD_WR_DATA(0x0E); //P13
  LCD_WR_DATA(0x1C); //P14
  LCD_WR_DATA(0x16); //P15
  LCD_WR_DATA(0x00); //P16
  
  LCD_WR_REG(0x3A);
  LCD_WR_DATA(0x55); //55-16BIT,66-18BIT,77-24BIT
  
  LCD_WR_REG(0x36); //Memory Access Control
  LCD_WR_DATA(0x00); //01-180
  //*****************
  LCD_WR_REG(0x2A); //480
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x01);
  LCD_WR_DATA(0xdf);
  
  LCD_WR_REG(0x2B); //854
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x00);
  LCD_WR_DATA(0x03);
  LCD_WR_DATA(0x55);
  //*****************************
  
  LCD_WR_REG(0x11);
  LCD_delay(120);
  LCD_WR_REG(0x29);
  LCD_delay(25);
  LCD_WR_REG(0x2C);
  
  LCD_delay(10);
  Lcd_Light_ON;	//��������
  LCD_Display_Dir(1);		//0����  1����
  LCD_Clear(BLACK);
}  

/*
****************************************************************
*����: ��������
*����: ������ɫ��λ��	
****************************************************************
*/
void LCD_Clear(u16 color)
{
	u16 x=0,y=0;
	u32 index=0;
	u32 totalpoint = lcd_dev.width;
	totalpoint *= lcd_dev.height;
  
	LCD_WR_REG(0X2A);		/* column address control set */
	LCD_WR_DATA(x>>8);	/* �ȸ�8λ��Ȼ���8λ */
	LCD_WR_DATA(x&0xff);	/* ������ʼ��ͽ�����*/
	LCD_WR_DATA((lcd_dev.width) >>8);
	LCD_WR_DATA((lcd_dev.width) &0xff);
  
	LCD_WR_REG(0X2B);		/* page address control set */
	LCD_WR_DATA(y>>8);
	LCD_WR_DATA(y&0xff);	
	LCD_WR_DATA((lcd_dev.height) >>8);
	LCD_WR_DATA((lcd_dev.height) &0xff);
	
	LCD_WR_REG(0x2C);			//д��GRAM
	
	for( index=0;index < totalpoint ; index++ )
	{
		LCD_WR_DATA( color );
	}
}

/*
****************************************************************
*����: ��亯��
*����: ������ɫ��λ��	
****************************************************************
*/
void LCD_Fill(u16 x, u16 y, u16 width, u16 height, u16 color)
{
	u32 i = 0;
	
	LCD_WR_REG(0X2A);
	LCD_WR_DATA( x >> 8 );				/* �ȸ�8λ��Ȼ���8λ */
	LCD_WR_DATA( x & 0xff );			/* column start   */ 
	LCD_WR_DATA( (x+width-1) >> 8 );	/* column end   */
	LCD_WR_DATA( (x+width-1) & 0xff );
	
  LCD_WR_REG(0X2B); 			     
	LCD_WR_DATA( y >> 8 );				/* page start   */
	LCD_WR_DATA( y & 0xff );
	LCD_WR_DATA( (y+height-1) >> 8);	/* page end     */
	LCD_WR_DATA( (y+height-1) & 0xff);
	
	LCD_WR_REG(0x2c);					/* memory write */
  
	for( i=0; i < width*height; i++ )
	{
		LCD_WR_DATA( color );
	}
}

/*
***********************************************************************
*����: ��ˮƽ��
*����: x1 y1  �����λ��
*		 x2 y2  ���յ�λ��
***********************************************************************
*/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //���õ������� 
	else if(delta_x==0)incx=0;//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//������� 
	{  
		LCD_DrawPoint(uRow, uCol);//���� 
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


/*
****************************************************************
*����: ������
*����: ���ζԽ�����
****************************************************************
*/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

/*
****************************************************************
*����: ��Բ
*����: ԭ������Ͱ뾶
****************************************************************
*/
void LCD_Draw_Circle(u16 x,u16 y,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_DrawPoint(x+a,y-b);             //5
 		LCD_DrawPoint(x+b,y-a);             //0           
		LCD_DrawPoint(x+b,y+a);             //4               
		LCD_DrawPoint(x+a,y+b);             //6 
		LCD_DrawPoint(x-a,y+b);             //1       
 		LCD_DrawPoint(x-b,y+a);             
		LCD_DrawPoint(x-a,y-b);             //2             
    LCD_DrawPoint(x-b,y-a);             //7     	         
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
} 

/*
****************************************************************
*����: ��ʾһ���ַ�
*����: λ�á��ַ�����С
****************************************************************
*/
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
	u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		  //�õ�����һ���ַ���Ӧ������ռ���ֽ���	
 	num=num-' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=tft_asc2_1206[num][t]; 	 	  //����1206����
		else if(size==16)temp=tft_asc2_1608[num][t];	//����1608����
		else if(size==24)temp=tft_asc2_2412[num][t];	//����2412����
		else return;								//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,POINT_COLOR);
			else if(mode==0)LCD_Fast_DrawPoint(x,y,BACK_COLOR);
			temp<<=1;
			y++;
			if(y>=lcd_dev.height)return;		//��������
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcd_dev.width)return;	//��������
				break;
			}
		}  	 
	}   	 	  
}

/*
****************************************************************
* ���ܣ���ʾһ���ַ� ������ Ӧ�ò�
*       ���Ƹ�ʽ������+����ʽ+˳��+C51��ʽ
* ������x��y����ʾ���꣬num���ַ���color���ַ���ɫ��bc������ɫ
* ���أ���
****************************************************************
*/
void LCDDrawChar(int x, int y, char num, u8 size,int color, int bc)
{  							  
	u8 temp,t1,t;
  u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		  //�õ�����һ���ַ���Ӧ������ռ���ֽ���	
 	num=num-' ';
  /* num:��  t:��  �����ֽ� */
	for(t=0;t<csize;t++)
	{   
    /* ȡ������ֽ� */
		if(size==12)temp=tft_asc2_1206[num][t]; 	 	  //����1206����
		else if(size==16)temp=tft_asc2_1608[num][t];	//����1608����
		else if(size==24)temp=tft_asc2_2412[num][t];	//����2412����
    else if(size==32)temp=tft_asc2_3216[num][t];	//����3216����
    else if(size==40)temp=tft_asc2_4020[num][t];	//����4020����
		else {
      printf("LCDDrawChar No font library\r\n");
      return;								//û�е��ֿ�
    }
    /* �������� */
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)LCD_Fast_DrawPoint(x,y,color);
			else if(bc>0)LCD_Fast_DrawPoint(x,y,bc);
			temp<<=1;
			y++;
			if(y>=lcd_dev.height)return;		//��������
      /* �л�����һ�� */
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcd_dev.width)return;	//��������
				break;
			}
		}    	 
	}   	 	  
}

/*
****************************************************************
*����: �ݺ���
*����: 
****************************************************************
*/
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

/*
****************************************************************
*����: ��ʾ����
*����: λ�á���ֵ�����ȡ���С
****************************************************************
*/
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

/*
****************************************************************
*����: ��ʾ�ַ���
*����: λ�á���ֵ����ɫ����С
****************************************************************
*/
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
  while((*p<='~')&&(*p>=' '))     // �ж��ǲ��ǷǷ��ַ�!
  {       
    if(x>=width){x=x0;y+=size;}   // ����
    if(y>=height)break;           // �˳�
    LCD_ShowChar(x,y,*p,size,0);
    x+=size/2;
    p++;
  }  
}
