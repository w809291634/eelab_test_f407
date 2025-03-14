#include "key.h"
#include "delay.h"

// ��ʼ��
void key_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure ={0};                      //����һ��GPIO_InitTypeKef���͵Ľṹ��
  RCC_AHB1PeriphClockCmd(KEY_CLK, ENABLE);                      //����KEY��ص�GPIO����ʱ��
  
  GPIO_InitStructure.GPIO_Pin   = K1_PIN | K2_PIN | 
                                  K3_PIN | K4_PIN;              //ѡ��Ҫ���Ƶ�GPIO����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������ŵ��������Ϊ����
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;                 //��������ģʽΪ����ģʽ
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                 //��������Ϊ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //������������Ϊ2MHz
  
  GPIO_Init(KEY_PORT, &GPIO_InitStructure);                     //��ʼ��GPIO����
}

//����������
//���ذ���ֵ
//mode: 0,��֧��������; 1,֧��������;
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY1==0)return K1_PRES;
		else if(KEY2==0)return K2_PRES;
		else if(KEY3==0)return K3_PRES;
		else if(KEY4==0)return K4_PRES;
	}else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	    
 	return 0;// �ް�������
}
