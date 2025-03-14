#include "user_cmd.h"
#include "shell.h"
#include "usart1.h"
#include "w25qxx.h" 
#include "stdlib.h"
#include "font_config.h"

static void SoftReset(void* arg)
{ 
  (void)arg;
  __set_FAULTMASK(1); // �ر������ж�
  NVIC_SystemReset(); // ��λ
}

static void font_update(void * arg)
{
  usart1_mode=1;
}

// �������ݶ�ȡ����ȡһ����������ݣ�����flashд�����ݲ���
// 24�������һ������Ϊ32*32 һ������ռ��128�ֽ�
static void font_read(void * arg)
{
#define SIZE 128
  char * argv[2];
  char buf[SIZE];
  int argc =cmdline_strtok((char*)arg,argv,2);
  int addr=atoi(argv[1]);
  
  printf("\r\n");
  W25QXX_Read((u8*)buf,FONT_LIB_STORE_ADD+addr,SIZE);
  for(int m=0;m<SIZE/16;m++){   // ��
    for(int n=0;n<16;n++){      // ��
      if(n==0)printf("%08d ",addr+16*m);
      printf("%02X ",buf[16*m+n]);
      if(n==7)printf(" ");
      if(n==15)printf("\r\n");
    }
  }
  printf("\r\n");
}

// �û�����ע��
void register_user_cmd(void)
{
  shell_register_command("reboot",SoftReset);
  shell_register_command("font_update",font_update);
  shell_register_command("font_read",font_read);
}
