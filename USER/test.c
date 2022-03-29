#include "test.h"
 
int main(void)
{			
	uint8_t ix = 0;
	Stm32_Clock_Init(9); 	//系统时钟设置
	delay_init(72000000);	     //延时初始化 
	uart_init(72000000, 9600);	 //串口初始化为9600
	LED_Init();		  	 //初始化与LED连接的硬件接口
	KEY_Init();
	EXTI_Init();
	LCD_Init(); 		//感觉这个板子跑个LCD屏已经有点心有余而力不足了, 可以优化吗?
	usmart_dev.init(72); //初始化 USMART
	
	//IWDG_Init(4, 615); // watch dog intialization
	//WWDG_Init(0X7F,0X5F,3);	//计数器值为7f,窗口寄存器为5f,分频数为8	 
	TIM3_Int_Init(9999, 7199);	//10Khz的计数频率，计数到5000为500ms  
	printf("intialized.");
	while(1)
	{
		//open watch dog
		uart1_print_recv_msg(S_USART1);
		LCD_Test(&ix);
		// printf("%d\n", KEY_Scan(1));
		// if(cmd == "circle loop"){
		// 	while(1);	//dog will bark
		// }

		//feed watch dog
		
		//printf("thread running...\n");
		//delay_ms(1000);
	}

	
}
















