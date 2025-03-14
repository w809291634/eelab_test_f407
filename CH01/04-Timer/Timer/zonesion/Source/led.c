/*********************************************************************************************
* 文件: led.c
* 作者：liutong 2015.8.19
* 说明：
* 修改：
* 注释：
*********************************************************************************************/
#include "led.h"

/*********************************************************************************************
* 指定本文件的引脚配置参数
*********************************************************************************************/
#define LED1_RCC       RCC_AHB1Periph_GPIOG
#define LED2_RCC       RCC_AHB1Periph_GPIOF
#define LED3_RCC       RCC_AHB1Periph_GPIOF
#define LED4_RCC       RCC_AHB1Periph_GPIOF

#define LED1_GPIO      GPIOG
#define LED2_GPIO      GPIOF
#define LED3_GPIO      GPIOF
#define LED4_GPIO      GPIOF

#define LED1_PIN       GPIO_Pin_0
#define LED2_PIN       GPIO_Pin_15
#define LED3_PIN       GPIO_Pin_14
#define LED4_PIN       GPIO_Pin_13

/*********************************************************************************************
* 名称：rgb_init
* 功能：初始化RGB对应的GPIO
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void led_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};                    
  
  RCC_AHB1PeriphClockCmd(LED1_RCC | LED2_RCC |
                         LED3_RCC | LED3_RCC, ENABLE);          //使能GPIO时钟
  
  GPIO_InitStructure.GPIO_Pin = LED1_PIN;                       //选中引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //输出速度2MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //无上下拉
  GPIO_Init(LED1_GPIO, &GPIO_InitStructure);                    //根据上述参数配置
  
  GPIO_InitStructure.GPIO_Pin = LED2_PIN ;                      //选中引脚
  GPIO_Init(LED2_GPIO, &GPIO_InitStructure);                    //根据上述参数配置
  
  GPIO_InitStructure.GPIO_Pin = LED3_PIN ;                      //选中引脚
  GPIO_Init(LED3_GPIO, &GPIO_InitStructure);                    //根据上述参数配置

  GPIO_InitStructure.GPIO_Pin = LED4_PIN ;                      //选中引脚
  GPIO_Init(LED4_GPIO, &GPIO_InitStructure);                    //根据上述参数配置
  
  led_turn_off(LED1_NUM);
  led_turn_off(LED2_NUM);
  led_turn_off(LED3_NUM);
  led_turn_off(LED4_NUM);
}

/*********************************************************************************************
* 名称：led_turn_off
* 功能：置引脚高电平，关闭灯
* 参数：cmd
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void led_turn_off(unsigned char cmd){
  if(cmd & LED1_NUM)
    GPIO_ResetBits(LED1_GPIO, LED1_PIN);                        //置引脚低电平,关闭
  if(cmd & LED2_NUM)
    GPIO_ResetBits(LED2_GPIO, LED2_PIN);
  if(cmd & LED3_NUM)
    GPIO_ResetBits(LED3_GPIO, LED3_PIN);
  if(cmd & LED4_NUM)
    GPIO_ResetBits(LED4_GPIO, LED4_PIN);
}

/*********************************************************************************************
* 名称：led_turn_on
* 功能：置引脚低电平，打开灯
* 参数：cmd
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void led_turn_on(unsigned char cmd){
  if(cmd & LED1_NUM)
    GPIO_SetBits(LED1_GPIO, LED1_PIN);                          //置引脚高电平，打开灯
  if(cmd & LED2_NUM)
    GPIO_SetBits(LED2_GPIO, LED2_PIN);
  if(cmd & LED3_NUM)
    GPIO_SetBits(LED3_GPIO, LED3_PIN);
  if(cmd & LED4_NUM)
    GPIO_SetBits(LED4_GPIO, LED4_PIN);
}

/*********************************************************************************************
* 名称：get_led_status
* 功能：获取灯的状态
* 参数：
* 返回：status，按位分别表示灯的状态
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_led_status(void){
  unsigned char status = 0;
  if(GPIO_ReadOutputDataBit(LED1_GPIO, LED1_PIN))
    status |= LED1_NUM;
  else
    status &= ~LED1_NUM;
  
  if(GPIO_ReadOutputDataBit(LED2_GPIO, LED2_PIN))
    status |= LED2_NUM; 
  else
    status &= ~LED2_NUM;
  
  if(GPIO_ReadOutputDataBit(LED3_GPIO, LED3_PIN))
    status |= LED3_NUM;
  else
    status &= ~LED3_NUM;
  if(GPIO_ReadOutputDataBit(LED4_GPIO, LED4_PIN))
    status |= LED4_NUM;
  else
    status &= ~LED4_NUM;
  return status;                                                //返回status
}

// 设置 LED 的状态
// cmd 具体控制的LED 号
// state 指定为新的状态
void led_setState(uint16_t cmd, unsigned char state)
{
  if(state)
  {
    if(cmd & LED1_NUM)
      led_turn_on(LED1_NUM);
    if(cmd & LED2_NUM)         
      led_turn_on(LED2_NUM);
    if(cmd & LED3_NUM)         
      led_turn_on(LED3_NUM);
    if(cmd & LED4_NUM)         
      led_turn_on(LED4_NUM);
  }
  else
  {
    if(cmd & LED1_NUM)
      led_turn_off(LED1_NUM);
    if(cmd & LED2_NUM)         
      led_turn_off(LED2_NUM);
    if(cmd & LED3_NUM)         
      led_turn_off(LED3_NUM);
    if(cmd & LED4_NUM)         
      led_turn_off(LED4_NUM);
  }
}

// LED 控制
// 控制位为1表示点亮
void led_ctrl(uint16_t cmd)
{
  if(cmd & LED1_NUM)
    led_turn_on(LED1_NUM);
  else                         
    led_turn_off(LED1_NUM);
  if(cmd & LED2_NUM)         
    led_turn_on(LED2_NUM);
  else                         
    led_turn_off(LED2_NUM);
  if(cmd & LED3_NUM)         
    led_turn_on(LED3_NUM);
  else                         
    led_turn_off(LED3_NUM);
  if(cmd & LED4_NUM)         
    led_turn_on(LED4_NUM);
  else                         
    led_turn_off(LED4_NUM);
}

// cycle 外部循环周期
void led_app(u16 cycle)
{
  static u16 time;time++;
  static u8 ledon;
  if(cycle>1000)cycle=1000;
  if(time>=1000/cycle){
    time=0;
    ledon=!ledon;
    if(ledon) led_setState(LED4_NUM,1);
    else led_setState(LED4_NUM,0);
  }
}
