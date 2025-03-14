#include "pcf8563.h"
#include "iic4.h"
#include <stdio.h>

/***********************************************************
* ����: PCF8563_write_byte()
* ����: ��8563ʱ��оƬд��һ���ֽ�
* ����: ��
* ����: ��
* �޸�:
* ע��: addrΪд�ļĴ�����ַ;dat�Ĵ���Ҫд���ֵ
***********************************************************/
void PCF8563_write_byte(unsigned char addr,unsigned char dat)
{
  I2C4_Start();                                                 //����iic
  I2C4_WriteByte(0xA2);                                          //ѡ��PCF8563�����������ñ�д���׼��(PCF8563��д��ַΪ0xA2)
  I2C4_WaitAck();           
  I2C4_WriteByte(addr);                                          //д��Ҫд�ļĴ�����ַ
  I2C4_WaitAck();           
  I2C4_WriteByte(dat);                                           //д��Ҫд������
  I2C4_WaitAck();           
  I2C4_Stop();                                                  //ֹͣiic
}

/***********************************************************
* ����: PCF8563_read_byte()
* ����: ��PCF8563��ȡһ���ֽ�
* ����: ��
* ����: ����������
* �޸�:
* ע��: addrΪҪ���ĵ�ַ���������ض���������
***********************************************************/
unsigned char PCF8563_read_byte(unsigned char addr)
{
  unsigned char dat;
  I2C4_Start();                                               //����iic
  I2C4_WriteByte(0xA2);                                        //ѡ��PCF8563�����������ñ�д���׼��(PCF8563��д��ַΪ0xA2)
  I2C4_WaitAck();           
  I2C4_WriteByte(addr);                                        //д��Ҫ���ĵ�ַ
  I2C4_WaitAck();           
  I2C4_Start();                                               //���¿�ʼ��ͨѶ
  I2C4_WriteByte(0xA3);                                        //PCF8563�Ķ���ַΪ0xA3,׼����ʼ��ȡ
  I2C4_WaitAck();
  dat=I2C4_ReadByte(0);                                     //����������ֵ��������
  I2C4_NAck();                                               //��Ϊֻ��һ���ֽڣ����Բ��÷���Ӧ���ź�
  I2C4_Stop();                                                //ֹͣiic
  return(dat);
}

/***********************************************************
* ����: PCF8563_init()
* ����: ��ʼ��ʱ��оƬ
* ����: ��
* ����: ��
* �޸�:
* ע��: 
***********************************************************/
void PCF8563_init(void)
{
  I2C4_IO_Init();
  PCF8563_write_byte(0x00,0x00);                                 //0x00,0x01Ϊ����/״̬�Ĵ�����ַ
  PCF8563_write_byte(0x01,0x12);                                 //����,�����ж���Ч
  PCF8563_write_byte(0x09,0x80);                                 //�رշ��ӱ���//  
  PCF8563_write_byte(0x0A,0x80);                                 //�ر�Сʱ����
  PCF8563_write_byte(0x0B,0x80);                                 //�ر��ձ���
  PCF8563_write_byte(0x0C,0x80);                                 //�ر����ڱ���
  PCF8563_write_byte(0x0D,0x00);                                 //�ر�CLKOUT���
  PCF8563_write_byte(0x0E,0x03);                                 //�رն�ʱ��
}

/*********************************************************************************************
* ����: PCF8563_Read_NByte()
* ����: ��PCF8563��ȡN���ֽ�
* ����: 
* ����: 
* �޸�:
* ע��: 
*********************************************************************************************/
unsigned char * PCF8563_Read_NByte(unsigned char addr ,unsigned char len,unsigned char *buf)
{
  unsigned char i;
  I2C4_Start();                
  I2C4_WriteByte(0xA2);       
  if (I2C4_WaitAck())
    return NULL;
  I2C4_WriteByte(addr);        
  if ( I2C4_WaitAck())
    return NULL;
  I2C4_Start();                
  I2C4_WriteByte(0xA3); 
  if (I2C4_WaitAck())
    return NULL;
  for(i=0;i<len-1;i++)
    buf[i]=I2C4_ReadByte(1);
  buf[len-1]=I2C4_ReadByte(0);        
  I2C4_Stop();              
  return buf;
}

int PCF8563_Write_NByte(unsigned char addr, unsigned char len, unsigned char *buf)
{
  unsigned char i = 0;
  I2C4_Start();                
  I2C4_WriteByte(0xA2); 
  if (I2C4_WaitAck())
    return -1;
  I2C4_WriteByte(addr);        
  if ( I2C4_WaitAck())
    return -1;
  for(i=0; i<len; i++)
  {
    I2C4_WriteByte(buf[i]);        
    if ( I2C4_WaitAck())
      return -1;
  }
  I2C4_Stop();
  return i;
}

// bcd��ת��Ϊ16������
unsigned char bcd_to_hex(unsigned char data)
{
    unsigned char temp;
 
    temp = ((data>>4)*10 + (data&0x0f));
    return temp;
}

// 16������ת��Ϊbcd��
unsigned char hex_to_bcd(unsigned char data)
{
    unsigned char temp;
 
    temp = (((data/10)<<4) + (data%10));
    return temp;
}

// PCF8563��ȡʱ��
int PCF8563_gettime(calendar_t *time)
{
  char timeBuf[7] = {0};
  if (PCF8563_Read_NByte(0x02, 7, (unsigned char *)timeBuf))
  {
    time->second = bcd_to_hex(timeBuf[0] & 0x7F);           //��7λ��Ч SECONDS (0 to 59)
    time->minute = bcd_to_hex(timeBuf[1] & 0x7F);           //��7λ��Ч MINUTES (0 to 59)
    time->hour = bcd_to_hex(timeBuf[2] & 0x3F);              //��6λ��Ч HOURS (0 to 23)
    time->day = bcd_to_hex(timeBuf[3] & 0x3F);               //��6λ��Ч DAYS (1 to 31)
    time->week = bcd_to_hex(timeBuf[4] & 0x07);              //��3λ��Ч WEEKDAYS (0 to 6)
    time->month = bcd_to_hex(timeBuf[5] & 0x1F);             //��5λ��Ч MONTHS (1 to 12)
    time->year = bcd_to_hex(timeBuf[6]);                     //8λ��Ч YEARS (0 to 99)
    return 1;
  }
  return 0;
}

// ����ʱ���������
char CountWeek(int iY, int iM, int iD) 
{
  int iWeekDay = -1; 
  if (1 == iM || 2 == iM) 
  {   
    iM += 12; 
    iY--;
  }   
  iWeekDay = (iD + 1 + 2 * iM + 3 * (iM + 1) / 5 + iY + iY / 4 - iY / 100 + iY / 400) % 7;
  return iWeekDay;
}

/*********************************************************************************************
* ����: PCF8563_settime()
* ����: ����ʱ��,�����Զ�����
* ����: 
* ����: 
* �޸�:
* ע��: 
*********************************************************************************************/
int PCF8563_settime(calendar_t *time)
{
  char timeBuf[7] = {0};
  if( time->year>99 ||
      time->month<1 || time->month>12 ||
      time->week>6 ||
      time->day<1 || time->day>31 ||
      time->hour>23 ||
      time->minute>59 ||
      time->second>59) return 0;

  time->week = CountWeek(time->year+2000, time->month, time->day);
  timeBuf[0] = hex_to_bcd(time->second);
  timeBuf[1] = hex_to_bcd(time->minute); 
  timeBuf[2] = hex_to_bcd(time->hour); 
  timeBuf[3] = hex_to_bcd(time->day);
  timeBuf[4] = hex_to_bcd(time->week); 
  timeBuf[5] = hex_to_bcd(time->month);
  timeBuf[6] = hex_to_bcd(time->year); 
  PCF8563_Write_NByte(0x02, 7, (unsigned char*)timeBuf);
  return 1;
}
