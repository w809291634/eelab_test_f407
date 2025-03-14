/*********************************************************************************************
* �ļ���vk16k33.h
* ���ߣ�zonesion
* ˵����vk16k33ͷ�ļ�
* �޸ģ�Chenkm 2017.01.10 �޸Ĵ����ʽ�����Ӵ���ע�ͺ��ļ�˵��
* ע�ͣ�
*********************************************************************************************/
#ifndef __VK16K33_H__
#define __VK16K33_H__
#include "stm32f4xx.h"

/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define VK16K33_ADDRESS 0x70

#define VK16K33_CMD_OSC_OFF 0x20
#define VK16K33_CMD_OSC_ON 0x21
#define VK16K33_CMD_DISPLAY_OFF 0x80
#define VK16K33_CMD_DISPLAY_ON 0x81
#define VK16K33_INT_LOW 0xA1
#define VK16K33_CMD_BRIGHT(x) (0xE0|(x&0x0f))

#define IICKEY_UP                   0x01		                    //��
#define IICKEY_LEFT                 0x02		                    //��
#define IICKEY_DOWN                 0x03		                    //��
#define IICKEY_RIGHT                0x04		                    //��
#define IICKEY_CENTER               0x05		                    //��

/*********************************************************************************************
* �ⲿԭ�ͺ���
*********************************************************************************************/
extern char onoff;
void vk16k33_init(void);
void segment_display(unsigned int num);
void display_off(void);
void display_on(void);
unsigned char vk16k33_get_keyval(void);
#endif //__ZLG7290_H__