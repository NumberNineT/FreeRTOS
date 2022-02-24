#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
//#include "stm32f10x_exti.h"

#include "FreeRTOS.h"
#include "task.h"

 
//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;	
	NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��

	KEY_Init();//��ʼ��������Ӧioģʽ

    //GPIOC.5 �ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource5);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line5;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
 
		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x06;//��ռ���ȼ�6 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//�����ȼ�0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
}


//������
//ʹ�����ַ�ʽ�Ϳ��Բ���Ϊ��һ������������ͷ�ļ�������?
extern TaskHandle_t Task2Task_Handler;

void EXTI9_5_IRQHandler(void)
{	
	BaseType_t YieldRequired;	
	delay_xms(20);   //����			 
	if(KEY0 == 0)	
	{
		//YieldRequired = xTaskResumeFromISR(Task2Task_Handler);//�ָ�����2
		printf("�ָ�����2������!\r\n");
		if(YieldRequired == pdTRUE)
		{
			/*�������xTaskResumeFromISR()����ֵΪpdTRUE����ô˵��Ҫ�ָ������
			������������ȼ����ڻ��߸����������е�����(���жϴ�ϵ�����),������
			�˳��жϵ�ʱ��һ��Ҫ�����������л���*/
			portYIELD_FROM_ISR(YieldRequired);
		}
	}
 	EXTI_ClearITPendingBit(EXTI_Line5);    //���LINE5�ϵ��жϱ�־λ  
}