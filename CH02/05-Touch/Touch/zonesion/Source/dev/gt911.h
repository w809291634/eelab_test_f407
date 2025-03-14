#ifndef __GT911_H_
#define __GT911_H_

// 引脚定义
#define GT911_RST_CLK         RCC_AHB1Periph_GPIOF
#define GT911_RST_GPIO        GPIOF
#define GT911_RST_PIN         GPIO_Pin_2

#define GT911_EXTI_CLK        RCC_AHB1Periph_GPIOG
#define GT911_EXTI_GPIO       GPIOG
#define GT911_EXTI_PIN        GPIO_Pin_2

#define GT911_RST_L           GT911_RST_GPIO->BSRRH = GT911_RST_PIN
#define GT911_RST_H           GT911_RST_GPIO->BSRRL = GT911_RST_PIN

#define GT911_EXTI_L          GT911_EXTI_GPIO->BSRRH = GT911_EXTI_PIN
#define GT911_EXTI_H          GT911_EXTI_GPIO->BSRRL = GT911_EXTI_PIN

// IIC 命令定义
#define GT911_CMD_WR          0x28
#define GT911_CMD_RD          0x29

#define GT911_CTRL_REG        0x8040
#define GT911_CFG_REG         0x8047
#define GT911_CHECK_REG       0X80FF
#define GT911_PID_REG         0x8140
#define GT911_GSTID_REG       0X814E

// 触摸定义
#define GT911_TOUCH_NUM       5

typedef struct{
  unsigned char id;
  unsigned short x;
  unsigned short y;
  unsigned short size;
}GT911_point_t;

// 外部引用
extern GT911_point_t GT911Point[GT911_TOUCH_NUM];
extern unsigned char GT911_pointNum;

int GT911_init(void);
void GT911_scan(void);

#endif // __GT911_H_
