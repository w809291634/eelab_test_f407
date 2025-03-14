/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "ov2640.h"
#include "dcmi.h"
#include "gui.h"
#include "timer.h"

/*********************************************************************************************
* ���ƣ�hardware_init()
* ���ܣ�Ӳ����ʼ��
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
static void hardware_init(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //����ϵͳ�ж����ȼ�����2
  usart_init(115200);                                           //���ڳ�ʼ��
  delay_init(168);                                              //��ʱ��ʼ��
  led_init();                                                   //LED��ʼ��
  key_init();                                                   //������ʼ��

  TIM3_Int_Init(10000-1,8400-1);                                //10Khz����,1�����ж�һ��,���ڼ���֡��
  DCMI_init();                                                  //DCMI����
  DCMI_DMA_Init((u32)Bank1_LCD_D,                               //DCMI DMA Ŀ���ַ����
                1,                                              //DMA Ŀ����16λ
                DMA_MemoryDataSize_HalfWord,                    //DMA Ŀ����16λ
                DMA_MemoryInc_Disable);                         //DMA Ŀ���ַ������

  lcd_init(CAMERA);                                             //LCD��ʼ��
}

const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	  //7����Ч 
u8 contrast=2,saturation=2,effect=0,resolution=0;
u8 contrast_buf[30];
u8 saturation_buf[30];
u8 effect_buf[30];
u8 resolution_buf[30];
volatile u8 key_active;

/*********************************************************************************************
* ���ƣ�DCMI_isr_app()
* ���ܣ�DCMI֡�ж���Ϊ����ʾLCD����������
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
void DCMI_isr_app(void)
{
  char buf[20];
  GUI_RESET_WIN;
  // ��ʾ֡��
  POINT_COLOR=BLACK;
  snprintf(buf,20,"%2d",ov_frame);
  LCD_ShowString(GUI_STAUS_BAR_X+GUI_STAUS_BAR_FONT/2*11,
                 GUI_STAUS_BAR_Y,GUI_STAUS_BAR_FONT*3,24,24,(u8*)buf);
  
  // ��ʾ��ť��ǰģʽ
  if(key_active){
    key_active=0;
    POINT_COLOR=RED;
    LCD_ShowString_Mid(GUI_STAUS_BAR_X+4,GUI_BTU_Y2(1)-16*2,GUI_BTU_W,16,(char*)contrast_buf);
    LCD_ShowString_Mid(GUI_STAUS_BAR_X+4,GUI_BTU_Y2(2)-16*2,GUI_BTU_W,16,(char*)saturation_buf);
    LCD_ShowString_Mid(GUI_STAUS_BAR_X+4,GUI_BTU_Y2(3)-16*2,GUI_BTU_W,16,(char*)effect_buf);
    LCD_ShowString_Mid(GUI_STAUS_BAR_X+4,GUI_BTU_Y2(4)-16*2,GUI_BTU_W,16,(char*)resolution_buf);
  }
}

/*********************************************************************************************
* ���ƣ�rgb565_app()
* ���ܣ�RGB565����Ӧ�ó���,RGB����ֱ����ʾ��LCD����
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
static void rgb565_app(void)
{ 
  u8 key;
  u8 msgbuf[30];
  /* ����ͷ�����ʼ�� */
  LCD_Clear(WHITE);
  GUI_RESET_WIN;
  //��ʾ֡��
  LCD_ShowString(GUI_STAUS_BAR_X,
                 GUI_STAUS_BAR_Y,200,
                 GUI_STAUS_BAR_FONT,GUI_STAUS_BAR_FONT,
                 "Frame rate:");                                
  // ���ư�ť
  gui_draw_btu(GUI_STAUS_BAR_X,GUI_BTU_Y1(1),
               lcd_dev.width-10,GUI_BTU_Y2(1),2,BLACK);
  gui_draw_btu(GUI_STAUS_BAR_X,GUI_BTU_Y1(2),
               lcd_dev.width-10,GUI_BTU_Y2(2),2,BLACK);
  gui_draw_btu(GUI_STAUS_BAR_X,GUI_BTU_Y1(3),
               lcd_dev.width-10,GUI_BTU_Y2(3),2,BLACK);
  gui_draw_btu(GUI_STAUS_BAR_X,GUI_BTU_Y1(4),
               lcd_dev.width-10,GUI_BTU_Y2(4),2,BLACK);
  // ��ʾ��ť����
  LCDDrawFont32_Mid(GUI_STAUS_BAR_X,GUI_BTU_Y1(1)+SPACING,"���öԱȶ�",GUI_BTU_W,BLACK,WHITE); 
  LCDDrawFont32_Mid(GUI_STAUS_BAR_X,GUI_BTU_Y1(2)+SPACING,"���ñ��Ͷ�",GUI_BTU_W,BLACK,WHITE); 
  LCDDrawFont32_Mid(GUI_STAUS_BAR_X,GUI_BTU_Y1(3)+SPACING,"������Ч",GUI_BTU_W,BLACK,WHITE); 
  LCDDrawFont32_Mid(GUI_STAUS_BAR_X,GUI_BTU_Y1(4)+SPACING,"���÷ֱ���",GUI_BTU_W,BLACK,WHITE); 

  /* ����ͷ������ʼ�� */
  OV2640_RGB565_Mode();
  OV2640_Resolution_800_600();
  OV2640_OutSize_Set(IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT); 
  /* ����ͷ���ݲ��� */
  DCMI_Start();
  /* �״�����ˢ�� */
  sprintf((char*)contrast_buf,"   Contrast:%d   ",(signed char)contrast-2);
  sprintf((char*)saturation_buf,"   Saturation:%d   ",(signed char)saturation-2);
  sprintf((char*)effect_buf,"   %s   ",EFFECTS_TBL[effect]);
  strcpy((char*)resolution_buf,"   800x600 30fps   ");
  key_active=1;
  while(1)
  { 
    key=KEY_Scan(0); 
    if(key)
    { 
      DCMI_Stop(0);                                             //ֹͣ��ʾ
      POINT_COLOR=RED;	                                        //������ɫ
      key_active=1;
      switch(key)
      {
        case K1_PRES:                                           //�Աȶ�����
          contrast++;
          if(contrast>4)contrast=0;
          OV2640_Contrast(contrast);
          sprintf((char*)msgbuf,"   Contrast:%d   ",(signed char)contrast-2);
          strncpy((char*)contrast_buf,(char*)msgbuf,30);
          break;
        case K2_PRES:                                           //���Ͷ�Saturation
          saturation++;
          if(saturation>4)saturation=0;
          OV2640_Color_Saturation(saturation);
          sprintf((char*)msgbuf,"   Saturation:%d   ",(signed char)saturation-2);
          strncpy((char*)saturation_buf,(char*)msgbuf,30);
          break;
        case K3_PRES:                                           //��Ч����
          effect++;
          if(effect>6)effect=0;
          OV2640_Special_Effects(effect);                       //������Ч
          sprintf((char*)msgbuf,"   %s   ",EFFECTS_TBL[effect]);
          strncpy((char*)effect_buf,(char*)msgbuf,30);
          break;
        case K4_PRES:                                           //�ֱ�������
          resolution=!resolution;
          if(resolution){
            OV2640_Resolution_1600_1200();
            OV2640_OutSize_Set(IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT);
            strcpy((char*)msgbuf,"  1600x1200 15fps  ");
          }else{
            OV2640_Resolution_800_600();
            OV2640_OutSize_Set(IMAGE_WIN_WIDTH,IMAGE_WIN_HEIGHT); 
            strcpy((char*)msgbuf,"   800x600 30fps   ");
          }
          strncpy((char*)resolution_buf,(char*)msgbuf,30);
          break;
      }
      GUI_RESET_WIN;
      LCD_ShowString(30+SPACING,50,210,16,16,msgbuf);           //��ʾ��ʾ����
      delay_ms(100);                                            //��ʾ�ȴ�
      DCMI_Start();                                             //���¿�ʼ����
    } 
    led_app(10);                                                //LEDָʾӦ��
    delay_ms(10);                                               //��ʱ
  }
}

/*********************************************************************************************
* ���ƣ�main()
* ���ܣ�������
* ��������
* ���أ���
* �޸ģ�
*********************************************************************************************/
int main(void)
{
  u8 count=0,blink=1;
  u8 key;
  u16 MID=0xffff,PID=0xffff;
  char buf[100];
  hardware_init();                                              //Ӳ����ʼ��
  
  /* ��ʼ��OV2640 */
  while(OV2640_Init(&MID,&PID))                                 
  {
    snprintf(buf,100,"OV2640��ʼ��ʧ��,Product ID:0x%04x",PID);
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,                    //LCD��ʾ�ı�
                  buf,LCD_WIDTH,BLACK,WHITE);    
    delay_ms(500);
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,                    //LCD��ʾ�ı�
                  "����...                               ",LCD_WIDTH,BLACK,WHITE);   
    delay_ms(500);
    led_app(1000);
  }
  snprintf(buf,100,"OV2640��ʼ���ɹ�,Product ID:0x%04x",PID);
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,                      //LCD��ʾ�ı�
                buf,LCD_WIDTH,BLACK,WHITE);    
  LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,                  //LCD��ʾ�ı�
                "������ⰴ����������ͷ����...",LCD_WIDTH,BLACK,WHITE);   
  while(1){
    key=KEY_Scan(0); 
    /* ���ⰴ������APP */
    if(key){
      rgb565_app();
    }
    /* �����л���ʾ */
    if(count>100){
      count=0;
      blink=!blink;
      if(blink)
        LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,            //LCD��ʾ�ı�
                      "������ⰴ����������ͷ����...",LCD_WIDTH,BLACK,WHITE);    
      else
        LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,            //LCD��ʾ�ı�
                      "                              ",LCD_WIDTH,BLACK,WHITE);    
    }

    count++;
    led_app(10);                                                //LEDָʾӦ��
    delay_ms(10);                                               //��ʱ10����
  }
}  