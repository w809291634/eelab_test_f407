/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "led.h"
#include "key.h"

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
  
  lcd_init(SPI);                                                //LCD��ʼ��
}

/*********************************************************************************************
* ���ƣ�main()
* ���ܣ�������
* ��������
* ���أ���
* �޸ģ�
* ע�⣺SPI flash�д����LCD GB2312 �ֿ��������Ϣ���벻Ҫ�����޸� SPI flash �е�����
*       ������޸�λ�òο������ļ���
*********************************************************************************************/
const u8 TEXT_Buffer[]={"Hello IOT! This is flash test!"};
#define SIZE sizeof(TEXT_Buffer)
int main(void)
{
  u16 id = 0;
  u8 datatemp[SIZE];
  u32 start_addr=FLASH_EXP_ADD;                                 //ͳһflash�Ķ�дʾ����ַ
  //start_addr=FLASH_EXP_ADD+4096-10;                           //�����ڿ�������д
  //start_addr=FLASH_EXP_ADD+4096*4-10;                         //�����ַ
  char temp[100]={0};
  hardware_init();                                              //Ӳ����ʼ��
  /* ��ȡ�豸ID */
	while(1)
	{
    id = W25QXX_ReadID();
    if (id == W25Q64 || id == W25Q128) break;
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,                    //LCD��ʾ�ı�
                  "W25Q64���ʧ��",LCD_WIDTH,BLACK,WHITE);    
    delay_ms(500);
    LCDShowFont32(8+32*2,REF_POS+32+SPACING,                    //LCD��ʾ�ı�
                  "����        ",LCD_WIDTH,BLACK,WHITE);   
    delay_ms(500);
    led_app(1000);
	}
  snprintf(temp,100,"W25Q64׼��������ID:0x%04X",id);
  LCDShowFont32(8+32*2,REF_POS+32+SPACING,temp,LCD_WIDTH,BLACK,WHITE);   
  /* ����дflash�ķ�Χ */
    if(start_addr<FLASH_EXP_ADD ||                              //��ʼ��ַ����С�ڷ�����ʼ��ַ
        start_addr+SIZE > FLASH_EXP_ADD+FLASH_EXP_SIZE){        //������ַ���ܴ��ڷ���������ַ
      LCDShowFont32(8+32*2,REF_POS+32+SPACING,
           "��ַ�����������ַ��Χ       ",LCD_WIDTH,BLACK,WHITE);   
      while(1);
    }
  /* Ӧ�ô�ѭ�� */
  while(1){                                                     //Ӧ�ô�ѭ��
		u8 key=KEY_Scan(0);
    if(key==K1_PRES)                                            //KEY1����,д��W25Q64
    {
      LCD_Fill(8+32*2,REF_POS+32*2+SPACING*2,
               LCD_WIDTH,32*2+SPACING,WHITE);   
      LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,
                    "��ʼдW25Q64....",LCD_WIDTH,BLACK,WHITE);  
      W25QXX_Write((u8*)TEXT_Buffer,start_addr,SIZE);           //�Ӷ���ĵ�ַ�Ͽ�ʼд���ַ���
      printf("W25Q64 Write Finished!\r\n");
      LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,
                    "W25Q64д��ɹ�   ",LCD_WIDTH,BLACK,WHITE); 
		}
		if(key==K2_PRES)                                            //KEY2����,��ȡ�ַ�������ʾ
		{
      memset(datatemp,0,SIZE);                                  //���֮ǰ��ȡ������
      LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,
                    "��ʼ��W25Q64....",LCD_WIDTH,BLACK,WHITE); 

      W25QXX_Read(datatemp,start_addr,SIZE);					          //�Ӷ���ĵ�ַ�Ͽ�ʼ,����SIZE���ֽ�
      printf("W25Q64 Read Finished!data:%s\r\n",datatemp);
      snprintf(temp,100,"��ȡ��������:%s",datatemp);
      LCDShowFont32(8+32*2,REF_POS+32*2+SPACING*2,
                    temp,LCD_WIDTH,BLACK,WHITE); 
    } 

    led_app(10);                                                //LED״̬��ʾ
    delay_ms(10);                                               //��ʱ
  }
}  
