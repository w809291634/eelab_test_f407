#ifndef __GUI_H__
#define __GUI_H__
#include "extend.h"
#include "lcd.h"

#define IMAGE_WIN_WIDTH         600       //��������ͷ��ʾ���ڴ�С�����  �ο�ֵ��600 640  
#define IMAGE_WIN_HEIGHT        440       //��������ͷ��ʾ���ڴ�С���߶�  �ο�ֵ��440 480
#define GUI_RESET_WIN           LCD_Set_Window(0,0,lcd_dev.width,lcd_dev.height);

// �ײ�״̬��λ��
#define GUI_STAUS_BAR_FONT      24        //״̬�����ı��������С
#define GUI_STAUS_BAR_X         (lcd_dev.width-(lcd_dev.width-IMAGE_WIN_WIDTH-(lcd_dev.height-IMAGE_WIN_HEIGHT))+30)  //offset 30 ʹ��֡���ı��������½�
#define GUI_STAUS_BAR_Y         (lcd_dev.height-(GUI_STAUS_BAR_FONT+SPACING))

// ��ťλ��
#define GUI_BTU_SPACING         20        //��ť֮���������
#define GUI_BTU_H               ((GUI_STAUS_BAR_Y-GUI_BTU_SPACING*5)/4)
                                  //�����յ�           ���       ��ť�߿�
#define GUI_BTU_W               ((lcd_dev.width-10)-GUI_STAUS_BAR_X-6)    //offset 6

#define GUI_BTU_Y1(x)           (GUI_BTU_SPACING*x+GUI_BTU_H*(x-1)) 
#define GUI_BTU_Y2(x)           ((GUI_BTU_SPACING+GUI_BTU_H)*x)

void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color);
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color);
void gui_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color);
void gui_draw_btu(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color);

#endif
