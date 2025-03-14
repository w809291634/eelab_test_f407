#ifndef __FONT_CONFIG_H__
#define __FONT_CONFIG_H__

typedef struct {
  int font_flag;            // �洢��־
}font_info_t;
extern font_info_t font_info;

#define FONT_OK                   0XA5B4C3D2
#define FONT_NOK                  0Xffffffff
/** ������Ϣ **/
#define FONT_INFO_ADD             0x1000              // �ֿ�洢��Ϣ����ʼ��ַ��ռ��4KB
#define FONT_LIB_STORE_ADD        0x2000              // GB2312_24.DZK �������ļ��Ĵ洢��ʼ��ַ��
#define FONT_LIB_PARTITION_SIZE   0x200000            // �ֿ�ռ��2MB�Ŀռ�
// SPI flash ��ʵ�����ÿռ�
#define FLASH_EXP_ADD             FONT_LIB_STORE_ADD+FONT_LIB_PARTITION_SIZE     // SPI flash ��д ʵ�� ����ʼ��ַ
#define FLASH_EXP_SIZE            0x4000              // SPI flash ��д ʵ�� 16kb��ռ�ÿռ�

/** ������Ϣ��д�ӿ� **/
#define READ_FON_INFO             {W25QXX_Read((u8*)&font_info,FONT_INFO_ADD,sizeof(font_info_t));}
#define WRITE_FON_INFO            {W25QXX_Erase_Sector(FONT_INFO_ADD/4096); \
                                  W25QXX_Write_NoCheck((u8*)&font_info,FONT_INFO_ADD,sizeof(font_info_t));}
#endif