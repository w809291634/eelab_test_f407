#ifndef __ILI9806_H
#define __ILI9806_H
#include "extend.h"

typedef struct
{
  u16   width;                                                  //LCD ���
  u16   height;                                                 //LCD �߶�
  u16   id;                                                     //LCD ID
  u8    dir;                                                    //���������������ƣ�0��������1��������
  u16   touch_x;                                                //�����������
  u16   touch_y;                                                //������������
}_lcd_dev; 

extern u16  POINT_COLOR;
extern u16  BACK_COLOR;
extern _lcd_dev lcd_dev;

/*********************************************************************************************
* �������
*********************************************************************************************/
#define Lcd_Light_ON   GPIO_SetBits(GPIOF, GPIO_Pin_5);         //  BL = 1
#define Lcd_Light_OFF  GPIO_ResetBits(GPIOF, GPIO_Pin_5);       //  BL = 0
#define Bank1_LCD_C    ((u32)0x60000000)                        //�����ַ
#define Bank1_LCD_D    ((u32)0x60080000)                        //���ݵ�ַ
#define LCD_WIDTH       lcd_dev.width
#define LCD_HEIGHT      lcd_dev.height

//ɨ�跽����
#define L2R_U2D  0                                              //������,���ϵ���
#define L2R_D2U  1                                              //������,���µ���
#define R2L_U2D  2                                              //���ҵ���,���ϵ���
#define R2L_D2U  3                                              //���ҵ���,���µ���

#define U2D_L2R  4                                              //���ϵ���,������
#define U2D_R2L  5                                              //���ϵ���,���ҵ���
#define D2U_L2R  6                                              //���µ���,������
#define D2U_R2L  7                                              //���µ���,���ҵ���

#define DFT_SCAN_DIR      L2R_U2D                               //Ĭ�ϵ�ɨ�跽��

//������ɫ
#define WHITE             0xFFFF
#define BLACK             0x0000
#define BLUE              0x001F
#define BRED              0XF81F
#define GRED              0XFFE0
#define GBLUE             0X07FF
#define RED               0xF800
#define MAGENTA           0xF81F
#define GREEN             0x07E0
#define CYAN              0x7FFF
#define YELLOW            0xFFE0
#define BROWN             0XBC40                                //��ɫ
#define BRRED             0XFC07                                //�غ�ɫ
#define GRAY              0X8430                                //��ɫ
//GUI��ɫ

#define DARKBLUE          0X01CF                                //����ɫ
#define LIGHTBLUE         0X7D7C                                //ǳ��ɫ  
#define GRAYBLUE          0X5458                                //����ɫ
//������ɫΪPANEL����ɫ 
 
#define LIGHTGREEN        0X841F                                //ǳ��ɫ
#define LIGHTGRAY         0XEF5B                                //ǳ��ɫ(PANNEL)
#define LGRAY             0XC618                                //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE         0XA651                                //ǳ����ɫ(�м����ɫ)
#define LBBLUE            0X2B12                                //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

void TFTLCD_Init(void);                                         //��ʼ��
u16 LCD_ReadPoint(u16 x,u16 y);

void LCD_DisplayOn(void);                                       //����ʾ
void LCD_DisplayOff(void);                                      //����ʾ
void LCD_Clear(u16 Color);                                      //����
void LCD_SetBackLight(u32 pwm);
void LCD_WriteRAM_Prepare(void);
void LCD_SetCursor(u16 Xpos, u16 Ypos);                         //���ù��
void LCD_Set_Window(u16 sx, u16 sy, u16 width, u16 height);
void LCD_DrawPoint(u16 x,u16 y);                                //����
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);                 //���ٻ���
u16  LCD_ReadPoint(u16 x,u16 y);                                //���� 
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);                       //��Բ
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);              //����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);         //������
void LCD_Fill(u16 x, u16 y, u16 width, u16 height, u16 color);  //��䵥ɫ
void LCD_Fill_s(u16 sx, u16 sy, u16 ex, u16 ey, u16 color);
void LCD_FillColor(short x1,short y1,short x2,short y2,uint16_t color);
void LCD_FillData(short x1,short y1,short width,short height,unsigned short* dat);
void LCD_FillData_ex(short x1,short y1,short width,short height,unsigned short* dat,unsigned char multiple);
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);    //���ָ����ɫ
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);          //��ʾһ���ַ�
void LCDDrawChar(int x, int y, char num, u8 size,int color, int bc);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);           //��ʾһ������
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);  //��ʾ ����
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);  //��ʾһ���ַ���,12/16����
void LCD_ShowString_Mid(u16 x,u16 y,u16 len,u8 size,char *str);

void LCD_WR_REG(vu16 regval);
void LCD_WR_DATA(vu16 data);
u16 LCD_RD_DATA(void);
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u16 LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
void LCD_Scan_Dir(u8 dir);                                      //������ɨ�跽��
void LCD_Display_Dir(u8 dir);                                   //������Ļ��ʾ����
void LCD_BLCtrl(unsigned char cmd);                             //����LCD����Ƶ�����

#endif
