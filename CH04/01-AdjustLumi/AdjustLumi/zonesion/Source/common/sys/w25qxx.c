/*********************************************************************************************
* �ļ�: w25qxx.c
* ���ߣ�zonesion 2016.12.22
* ˵����
* ���ܣ�W25Q64��������
* �޸ģ�
*********************************************************************************************/

/*********************************************************************************************
* ͷ�ļ�
*********************************************************************************************/
#include "w25qxx.h" 
#include "spi.h"
#include "delay.h"	   
#include "extend.h"
#include "font_config.h"

/*********************************************************************************************
* �궨��
*********************************************************************************************/
#define W25QXX_SPI_CS_RCC     RCC_AHB1Periph_GPIOA
#define W25QXX_SPI_CS_GPIO    GPIOA
#define W25QXX_SPI_CS_PIN     GPIO_Pin_15

u16 W25QXX_TYPE=W25Q64;	                                        //Ĭ����W25Q64
u32 W25QXX_SIZE;

/*4KbytesΪһ��Sector
  16������Ϊ1��Block
  W25Q64����Ϊ8M�ֽ�,����128��Block,2048��Sector */

/*********************************************************************************************
* ����: W25QXX_Init
* ���ܣ���ʼ��SPI FLASH��IO��
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void W25QXX_Init(void)
{ 
  GPIO_InitTypeDef  GPIO_InitStructure={0};

  RCC_AHB1PeriphClockCmd(W25QXX_SPI_CS_RCC, ENABLE);            //ʹ��GPIOAʱ��

  /* GPIOA15 */
  GPIO_InitStructure.GPIO_Pin = W25QXX_SPI_CS_PIN;   
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //����
  GPIO_Init(W25QXX_SPI_CS_GPIO, &GPIO_InitStructure);        

  W25QXX_CS=1;                                                  //SPI FLASH��ѡ��
  SPI1_Init();                                                  //��ʼ��SPI
  //SPI1_SetSpeed(SPI_BaudRatePrescaler_4);                     //����Ϊ21Mʱ��
  W25QXX_TYPE=W25QXX_ReadID();	                                //��ȡFLASH ID.
//  printf("W25QXX_TYPE:0x%04X\r\n",W25QXX_TYPE);
  if(W25QXX_TYPE==0xEF16){
    W25QXX_SIZE = 8*1024*1024;
  }
  else if(W25QXX_TYPE==0xEF17){
    W25QXX_SIZE = 16*1024*1024;
  }
}  

/*********************************************************************************************
* ����: W25QXX_ReadSR
* ���ܣ���ȡW25QXX��״̬�Ĵ���
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�BIT7  6   5   4   3   2   1   0
        SPR   RV  TB BP2 BP1 BP0 WEL BUSY
        SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��;
        TB,BP2,BP1,BP0:FLASH����д��������;
        WEL:дʹ������
        Ĭ��:0x00
*********************************************************************************************/
u8 W25QXX_ReadSR(void)   
{  
  u8 byte=0;   
  W25QXX_CS=0;                                                  //ʹ������   
  SPI1_ReadWriteByte(W25X_ReadStatusReg);                       //���Ͷ�ȡ״̬�Ĵ�������    
  byte=SPI1_ReadWriteByte(0Xff);                                //��ȡһ���ֽ�  
  W25QXX_CS=1;                                                  //ȡ��Ƭѡ     
  return byte;   
} 

/*********************************************************************************************
* ����: W25QXX_Write_SR
* ���ܣ�дW25QXX״̬�Ĵ���
* ������
* ���أ�
* �޸ģ�
* ע�ͣ�ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
*********************************************************************************************/
void W25QXX_Write_SR(u8 sr)   
{   
  W25QXX_CS=0;                                                  //ʹ������   
  SPI1_ReadWriteByte(W25X_WriteStatusReg);                      //����дȡ״̬�Ĵ�������    
  SPI1_ReadWriteByte(sr);                                       //д��һ���ֽ�  
  W25QXX_CS=1;                                                  //ȡ��Ƭѡ     	      
}   

/*********************************************************************************************
* ����: W25QXX_Write_Enable
* ���ܣ�W25QXXдʹ��	
* ������
* ���أ�
* �޸ģ�
* ע�ͣ���WEL��λ 
*********************************************************************************************/
void W25QXX_Write_Enable(void)   
{
  W25QXX_CS=0;                                                  //ʹ������   
  SPI1_ReadWriteByte(W25X_WriteEnable);                         //����дʹ��  
  W25QXX_CS=1;                                                  //ȡ��Ƭѡ     	      
} 

/*********************************************************************************************
* ����: W25QXX_Write_Disable
* ���ܣ�W25QXXд��ֹ	
* ������
* ���أ�
* �޸ģ�
* ע�ͣ���WEL���� 
*********************************************************************************************/
void W25QXX_Write_Disable(void)   
{  
  W25QXX_CS=0;                                                  //ʹ������   
  SPI1_ReadWriteByte(W25X_WriteDisable);                        //����д��ָֹ��    
  W25QXX_CS=1;                                                  //ȡ��Ƭѡ     	      
}

/*********************************************************************************************
* ����: W25QXX_ReadID
* ���ܣ���ȡоƬID
* ������
* ���أ�����ֵ����:				   
        0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
        0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
        0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
        0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
        0XEF17,��ʾоƬ�ͺ�ΪW25Q128 
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
u16 W25QXX_ReadID(void)
{
  u16 Temp = 0;
  W25QXX_CS=0;
  SPI1_ReadWriteByte(0x90);                                     //���Ͷ�ȡID����
  SPI1_ReadWriteByte(0x00);
  SPI1_ReadWriteByte(0x00);
  SPI1_ReadWriteByte(0x00);
  Temp|=SPI1_ReadWriteByte(0xFF)<<8;
  Temp|=SPI1_ReadWriteByte(0xFF);
  W25QXX_CS=1;
  return Temp;
}

/*********************************************************************************************
* ����: W25QXX_Read
* ���ܣ���ȡSPI FLASH  
* ������pBuffer:���ݴ洢��;
        ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit);
        NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
* ���أ���
* �޸ģ�
* ע�ͣ���ָ����ַ��ʼ��ȡָ�����ȵ�����
*********************************************************************************************/
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
  u16 i;
  W25QXX_CS=0;                                                  //ʹ������   
  SPI1_ReadWriteByte(W25X_ReadData);                            //���Ͷ�ȡ����   
  SPI1_ReadWriteByte((u8)((ReadAddr)>>16));                     //����24bit��ַ    
  SPI1_ReadWriteByte((u8)((ReadAddr)>>8));                      
  SPI1_ReadWriteByte((u8)ReadAddr);   
  for(i=0;i<NumByteToRead;i++)
  { 
    pBuffer[i]=SPI1_ReadWriteByte(0XFF);                        //ѭ������  
  }
  W25QXX_CS=1;
}  

/*********************************************************************************************
* ����: W25QXX_Write_Page
* ���ܣ���ָ����ַ��ʼд�����256�ֽڵ�����
* ������pBuffer:���ݴ洢��;
        WriteAddr:��ʼд��ĵ�ַ(24bit);		
        NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
* ���أ���
* �޸ģ�
* ע�ͣ�SPI��һҳ(0~65535)��д������256���ֽڵ�����
*********************************************************************************************/
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
  u16 i;  
  W25QXX_Write_Enable();                                        //SET WEL 
  W25QXX_CS=0;                                                  //ʹ������   
  SPI1_ReadWriteByte(W25X_PageProgram);                         //����дҳ����   
  SPI1_ReadWriteByte((u8)((WriteAddr)>>16));                    //����24bit��ַ    
  SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
  SPI1_ReadWriteByte((u8)WriteAddr);   
  for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);  //ѭ��д��  
  W25QXX_CS=1;                                                  //ȡ��Ƭѡ 
  W25QXX_Wait_Busy();                                           //�ȴ�д�����
} 

/*********************************************************************************************
* ����: W25QXX_Write_NoCheck
* ���ܣ��޼���дSPI FLASH 
* ������pBuffer:���ݴ洢��;
        WriteAddr:��ʼд��ĵ�ַ(24bit);	
        NumByteToWrite:Ҫд����ֽ���(���65535)  
* ���أ���
* �޸ģ�
* ע�ͣ������Զ���ҳ���� ;
        ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
        ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
*********************************************************************************************/
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{
  u16 pageremain;
  pageremain=256-WriteAddr%256;                                 //��ҳʣ����ֽ���
  if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;      //������256���ֽ�
  while(1)
  {
    W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
    if(NumByteToWrite==pageremain)break;                        //д�������
    else //NumByteToWrite>pageremain
    {
      pBuffer+=pageremain;
      WriteAddr+=pageremain;

      NumByteToWrite-=pageremain;                               //��ȥ�Ѿ�д���˵��ֽ���
      if(NumByteToWrite>256)pageremain=256;                     //һ�ο���д��256���ֽ�
      else pageremain=NumByteToWrite;                           //����256���ֽ���
    }
  };
} 

/*********************************************************************************************
* ����: W25QXX_Write
* ���ܣ�дSPI FLASH  
* ������pBuffer:���ݴ洢��;
        WriteAddr:��ʼд��ĵ�ַ(24bit);		
        NumByteToWrite:Ҫд����ֽ���(���65535)  
* ���أ���
* �޸ģ�
* ע�ͣ���ָ����ַ��ʼд��ָ�����ȵ�����;
        �ú�������������!
*********************************************************************************************/
u8 W25QXX_BUFFER[4096];		 
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
  u32 secpos;
  u16 secoff;
  u16 secremain;
  u16 i;    
  u8 * W25QXX_BUF;
  W25QXX_BUF=W25QXX_BUFFER;
  secpos=WriteAddr/4096;                                        //������ַ  
  secoff=WriteAddr%4096;                                        //�������ڵ�ƫ��
  secremain=4096-secoff;                                        //����ʣ��ռ��С   
  //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);         //������
  if(NumByteToWrite<=secremain)secremain=NumByteToWrite;        //������4096���ֽ�
  while(1) 
  {	
    W25QXX_Read(W25QXX_BUF,secpos*4096,4096);                   //������������������
    for(i=0;i<secremain;i++)                                    //У������
    {
      if(W25QXX_BUF[secoff+i]!=0XFF)break;                      //��Ҫ����
    }
    if(i<secremain)                                             //��Ҫ����
    {
      W25QXX_Erase_Sector(secpos);                              //�����������
      for(i=0;i<secremain;i++)	                                //����
      {
        W25QXX_BUF[i+secoff]=pBuffer[i];	  
      }
      W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);        //д����������  

    }else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);    //д�Ѿ������˵�,ֱ��д������ʣ������.
    if(NumByteToWrite==secremain)break;                         //д�������
    else                                                        //д��δ����
    {
      secpos++;                                                 //������ַ��1
      secoff=0;                                                 //ƫ��λ��Ϊ0

      pBuffer+=secremain;                                       //ָ��ƫ��
      WriteAddr+=secremain;                                     //д��ַƫ��
      NumByteToWrite-=secremain;                                //�ֽ����ݼ�
      if(NumByteToWrite>4096)secremain=4096;                    //��һ����������д����
      else secremain=NumByteToWrite;                            //��һ����������д����
    }
  };
}

/*********************************************************************************************
* ����: W25QXX_Erase_Chip
* ���ܣ���������оƬ		  
* ������
* ���أ���
* �޸ģ�
* ע�ͣ��ȴ�ʱ�䳬��...
*********************************************************************************************/
void W25QXX_Erase_Chip(void)   
{                                   
  W25QXX_Write_Enable();                                        //SET WEL 
  W25QXX_Wait_Busy();   
  W25QXX_CS=0;                                                  //ʹ������   
  SPI1_ReadWriteByte(W25X_ChipErase);                           //����Ƭ��������  
  W25QXX_CS=1;                                                  //ȡ��Ƭѡ
  W25QXX_Wait_Busy();                                           //�ȴ�оƬ��������
}   

/*********************************************************************************************
* ���ƣ�W25QXX_Erase_Sector
* ���ܣ�����һ������
* ������Dst_Addr:������ַ ����ʵ����������
* ���أ���
* �޸ģ�
* ע�ͣ�����һ��ɽ��������ʱ��:150ms
*********************************************************************************************/
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{
  /*����falsh�������,������ */    
//  printf("fe:%x\r\n",Dst_Addr);	  
  Dst_Addr*=4096;
  W25QXX_Write_Enable();                                        //SET WEL
  W25QXX_Wait_Busy();   
  W25QXX_CS=0;                                                  //ʹ������
  SPI1_ReadWriteByte(W25X_SectorErase);                         //������������ָ�� 
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));                     //����24bit��ַ
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
  SPI1_ReadWriteByte((u8)Dst_Addr);  
  W25QXX_CS=1;                                                  //ȡ��Ƭѡ
  W25QXX_Wait_Busy();                                           //�ȴ��������
}  
    /* �ȴ����� */
void W25QXX_Wait_Busy(void)   
{
  while((W25QXX_ReadSR()&0x01)==0x01);                          // �ȴ�BUSYλ���
}

/*********************************************************************************************
* ���ƣ�W25QXX_PowerDown
* ���ܣ��������ģʽ
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void W25QXX_PowerDown(void)   
{ 
  W25QXX_CS=0;                                                  //ʹ������
  SPI1_ReadWriteByte(W25X_PowerDown);                           //���͵�������
  W25QXX_CS=1;                                                  //ȡ��Ƭѡ
  delay_us(3);                                                  //�ȴ�TPD
}

/*********************************************************************************************
* ���ƣ�W25QXX_WAKEUP
* ���ܣ�����
* ������
* ���أ���
* �޸ģ�
* ע�ͣ�
*********************************************************************************************/
void W25QXX_WAKEUP(void)   
{  
  W25QXX_CS=0;                                                  //ʹ������   
  SPI1_ReadWriteByte(W25X_ReleasePowerDown);                    // send W25X_PowerDown command 0xAB    
  W25QXX_CS=1;                                                  //ȡ��Ƭѡ     	      
  delay_us(3);                                                  //�ȴ�TRES1
}   
