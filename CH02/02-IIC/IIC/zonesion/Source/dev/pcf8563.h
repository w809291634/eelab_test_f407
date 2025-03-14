#ifndef __PCF8563_H_
#define __PCF8563_H_

typedef struct {
  unsigned char hour : 5;                                       // ʱ��0-23��
  unsigned char week : 3;                                       // ���ڣ�0-6��
  unsigned char minute;                                         // �֣�0-59��
  unsigned char second;                                         // �루0-59��
  unsigned char day;                                            // �գ�1-31��
  unsigned char month;                                          // �£�1-12��
  unsigned char year;                                           // �꣨0-99��
}calendar_t;

void PCF8563_init(void);
int PCF8563_gettime(calendar_t *time);
int PCF8563_settime(calendar_t *time);
char CountWeek(int iY, int iM, int iD) ;

#endif // __PCF8563_H_
