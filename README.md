##### STM32 ѧϰ�����Ĵ����汾

�ӼĴ����汾��ʼѧϰ������⺯���汾�Լ�HAL��汾�������ɣ�

��Ŀ�����Լ������Լ������������϶���Ҫֱ���� HAL ��ģ�����ѧϰ����Ҫ��ԭ��Ϊ����ѧ��͸����ͨ͸һ�㡣����~

��ʹ�� Keil ����汾 -> GCC ����汾

##### 1. CM3 ���жϹ���



```C
unsigned int a;	//unsigned int ���ͼӵ����ֵ INT_MAX ֮��, ���0���¿�ʼ, int ��û����������
uint8_t b;	// 0-255, int8_t -128 - 127
```



##### 2.��λ����������ʲôԭ��

```C
#include <stdint.h>

int a = 1;
uint8 _t x = 1;
int8_t y = 1;

x << 1;
y << 30
```



##### 3.����ת��

itoa������ԭ��Ϊchar* itoa(int value, char * string, int radix); int value ��ת����������char *string ת���󴢴���ַ�����int radix ת������������2,8,10,16 ���Ƶȣ�

```C
#include <stdlib.h>
void fun()
{
  	a = 12;
	char s[10];
  	itoa(a, s, 2);	//ת��Ϊ������
  	printf("%s\n", s);
  	itoa(a, s, 3);
  	printf("%s\n", s);	//������
}
```



##### 4. stm32f103RCT6 ������ӵ�WK_UP���������е����⣬һֱ��������



##### 5. FreeeRTOS����ϵͳ�и����߳�ͬ����ʹ�÷����Լ�ʹ�ó������ο���ǰ��Ŀ

д��STM32_FreeRTOS��֧