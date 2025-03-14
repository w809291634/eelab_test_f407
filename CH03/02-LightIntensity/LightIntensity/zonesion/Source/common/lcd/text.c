#include "text.h"
#include "font_user.h"
#include "string.h"
#include "stdio.h"

int GB_40_SIZE = sizeof(GB_40) / sizeof(struct typFNT_GB40);
font_info_t font_info;

// LCD ʹ���Զ���� ����һ����ɫ��
// x,y ���  
// dot �����ַ�����ָ�� len �ֽ���
// color ǰ��ɫ  bc ����ɫ
static void LCD_Write_GRAM_BlitLine(u16 x, u16 y, u8* dot, u16 len, u16 color ,u16 bc)
{
	LCD_WR_REG(0X2A);		  /* column address control set */
	LCD_WR_DATA(x>>8);	  /* �ȸ�8λ��Ȼ���8λ */
	LCD_WR_DATA(x&0xff);	/* ���ú��� ��ʼ��ͽ�����*/
	LCD_WR_DATA((lcd_dev.width) >>8);
	LCD_WR_DATA((lcd_dev.width) &0xff);
  
	LCD_WR_REG(0X2B);		  /* page address control set */
	LCD_WR_DATA(y>>8);
	LCD_WR_DATA(y&0xff);	
	LCD_WR_DATA((lcd_dev.height) >>8);
	LCD_WR_DATA((lcd_dev.height) &0xff);
  
  LCD_WR_REG(0x2C);
  for(int i=0;i<len;i++){
    for(int t=0;t<8;t++){
      if (*dot & 1<<(7-t)) LCD_WR_DATA(color);
      else if (bc > 0) LCD_WR_DATA(bc);
    }
    dot ++;
  }
}

/********** ���� GB2312 �ֿ� *********/
// ��ʼ���ֿ��־
void font_lib_init(void)
{
  W25QXX_Init();
  READ_FON_INFO;
  if(font_info.font_flag!=FONT_OK){
    printf("Font library error\r\n");
    while(1);
  }
}

//code �ַ�ָ�뿪ʼ
//�� GB2312 �ֿ��в��ҳ���ģ
//code �ַ����Ŀ�ʼ��ַ,GBK��
//mat  ���ݴ�ŵ�ַ (size/8+((size%8)?1:0))*(size) bytes��С	
//size:�����С
static void Get_HzMat(u8 *code,u8 *mat,u8 size)
{		    
	unsigned char qh,ql;
	unsigned char i;					  
	unsigned long foffset; 
  if(size!=32){
    printf("Get_HzMat no size\r\n");
    return;
  }
  
	u8 csize=(size/8+((size%8)?1:0))*(size);//�õ�����һ���ַ���Ӧ������ռ���ֽ���	 
  qh=*code;
  ql=*(++code);
	if(qh<0xa1||ql<0xa1||ql==0xff||qh==0xff)//�� GB2312 ���ú���
	{   		    
	    for(i=0;i<csize;i++)*mat++=0x00;    //���հ�
	    return; //��������
	}          
  ql -= 0xa1;
  qh -= 0xa1;
  foffset = ((unsigned long)94*qh + ql)*csize;
	switch(size)
	{
		case 32:
      W25QXX_Read(mat,FONT_LIB_STORE_ADD+foffset,csize);
			break;
	}
}

// ����һ��GB2312���֣��ݽ�֧��32������
// �����ܹ����Ƶĺ��ֲ�ѯ GB2312 �ֿ��
// font ���������ָ�� size �ֺ�
static void LCDDrawGB2312(int x, int y, char* font, u8 size, int color, int bc)
{
  u8 dzk[128];
  
  if(size!=32){
    printf("LCDDrawGB2312_32 no size\r\n");
    return;
  }
  Get_HzMat((u8*)font,dzk,size);	//�õ���Ӧ��С�ĵ������� 
  
  /* ȡ���� */
	for(int t=0;t<size;t++)
	{   												   
    LCD_Write_GRAM_BlitLine(x,y+t,dzk+4*t,4,color,bc);
	}
}

// lcd��ʾ32x32���֣��ļ��ֿ�棩
// x1,y1:��ʼ����ֵ
// p �ַ�  width���ƿ��
// color��������ɫ��Bcolor��������ɫ
void LCDShowFont32(u16 x,u16 y,char* str,u16 width,u16 color,u16 Bcolor)
{
  u8 size=32;         // �����ַ�դ���С
  u16 x0=x;
  
  while(*str!=0)    // ����δ����
  { 
    /* �ַ� ���ռ��һ��size */
    if(*str<0x80)
    {
      // ��ǰ�ַ�Ϊ�����ַ�
      if(*str=='\r'){
        str++; 
        continue;
      }
      if(*str=='\n'){   //���з���
        x=x0;
        y=y+size+10;    // ������10���м��
        //y+=size;
        str++; 
        continue;
      }  
      // ��ǰ�ַ�����,������ʾ
      if(x>(x0+width-size/2)){
        y=y+size+10;    // ������10���м��
        //y+=size;
        x=x0;	   
      }							    
      // if(y>(y0+height-size))break;   //Խ�緵��    
      LCDDrawChar(x, y, *str,size, color, Bcolor);//��Ч����д�� 
      str++; 
      x+=size/2;      //�ַ�,Ϊȫ�ֵ�һ�� 
    }
    /* ���� */
    else
    {     
      if(x>(x0+width-size))//����
      {	    
        y=y+size+10;
        //y+=size;
        x=x0;		  
      }
      // if(y>(y0+height-size))break;//Խ�緵��  						     
      LCDDrawGB2312(x,y,str,size,color,Bcolor);
      str+=2; 
      x+=size;//��һ������ƫ��	    
    }						 
  }
}

//��ָ����ȵ��м���ʾ�ı�
//len:ָ��Ҫ��ʾ�Ŀ��
void LCDDrawFont32_Mid(u16 x,u16 y,char *str,u16 len,int color, int Bcolor)
{
  u8 str_size=16;         // �����ַ���դ���С
  u8 hz_size=32;          // ���庺��դ���С
  u16 size;
  
  // ��ȡ���ٸ����ֺ��ַ�
  char *p=str;
  u8 str_num=0;
  u8 hz_num=0;
  while(*p!=0){ 
    if(*p<0x80){
      p++;
      str_num++;
    }
    /* ���� */
    else{
      p+=2; 
      hz_num++;
    }						 
  }
  size = str_size*str_num + hz_size*hz_num;     // ��ǰ��ʾ�ı�ռ������
 
  if(size>len){
    printf("LCDDrawFont32_Mid parameter error!\r\n");
    return;
  }
  size=(len-size)/2;                            // ���ƫ����
  LCDShowFont32(size+x,y,str,len,color,Bcolor);
} 

/********** ���� �Զ����ֿ� *********/
// ����һ���Զ����40������
void LCDDraw_User40(u16 x, u16 y, char *gb2, u16 color, u16 Bcolor)
{
  u8 size=40;         // �����ַ�դ���С
  char ok=0;
  
  for (int i=0; i<GB_40_SIZE; i++){
    /* ȡ���� */
    if (gb2[0] == GB_40[i].Index[0] && gb2[1] == GB_40[i].Index[1]){
      /* ȡ���� */
      char* dot = GB_40[i].Msk;
      ok=1;
      for(int t=0;t<size;t++)// ������
      {   												   
        LCD_Write_GRAM_BlitLine(x,y+t,(u8*)dot+size/8*t,size/8,color,Bcolor);
      }
    }
  }
  // û�����Զ����ֿ����ҵ�����䱳��
  if(ok==0){
    LCD_Fill(x,y,size,size,Bcolor);
  }
}

// lcd��ʾ40x40���֣��Զ����ֿ⣩  �ַ�ʹ���ַ���
// x1,y1:��ʼ����ֵ
// gb2 �ַ�
// color��������ɫ��Bcolor��������ɫ
// �������Զ����ֿ�����
// ���߱����й���
void LCDDrawFont40(u16 x, u16 y, char *str, u16 color, u16 Bcolor)
{
  u8 size=40;         // �����ַ�դ���С
  
  while(*str!=0)    // ����δ����
  { 
    /* �ַ� ���ռ��һ��size */
    if(*str<0x80)
    {
      LCDDrawChar(x, y, *str,size, color, Bcolor);//��Ч����д�� 
      str++; 
      x+=size/2;      //�ַ�,Ϊȫ�ֵ�һ�� 
    }
    /* ���� */
    else
    {
      LCDDraw_User40(x,y,str,color,Bcolor);
      str+=2; 
      x+=size;//��һ������ƫ��	    
    }						 
  }
}

//��ָ����ȵ��м���ʾ�ı�
//len:ָ��Ҫ��ʾ�Ŀ��
void LCDDrawFont40_Mid(u16 x,u16 y,char *str,u16 len,int color, int Bcolor)
{
  u8 str_size=20;         // �����ַ���դ���С
  u8 hz_size=40;          // ���庺��դ���С
  u16 size;
  
  // ��ȡ���ٸ����ֺ��ַ�
  char *p=str;
  u8 str_num=0;
  u8 hz_num=0;
  while(*p!=0){ 
    if(*p<0x80){
      p++;
      str_num++;
    }
    /* ���� */
    else{
      p+=2; 
      hz_num++;
    }						 
  }
  size = str_size*str_num + hz_size*hz_num;     // ��ǰ��ʾ�ı�ռ������
 
  if(size>len){
    printf("LCDDrawFont40_Mid parameter error!\r\n");
    return;
  }
  size=(len-size)/2;                            // ���ƫ����
  LCDDrawFont40(size+x,y,str,color,Bcolor);
}   
