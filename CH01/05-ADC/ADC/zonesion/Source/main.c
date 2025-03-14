/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include "stm32f4xx.h"
#include "delay.h"
#include "ADC.h"
/*********************************************************************************************
* 全局变量
*********************************************************************************************/
uint32_t ADCvalue = 0;                                          //ADC真实值
float value=0;                                                  //转换后的值

/*********************************************************************************************
* 名称：hardware_init()
* 功能：硬件初始化
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
static void hardware_init(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //设置系统中断优先级分组2
  AdcInit();                                                    //ADC配置初始化
}

/*********************************************************************************************
* 名称：main()
* 功能：主函数
* 参数：无
* 返回：无
* 修改：
*********************************************************************************************/
int main(void)
{
  hardware_init();
  while(1){
    ADCvalue=AdcGet(ADC_Channel_13);                            //adc值获取
    value =(ADCvalue*(12.0f/4095));                             //标准电压转化
    delay_count(100);                                           //延时
  }
}  
