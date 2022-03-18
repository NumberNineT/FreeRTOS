#include "watch_dog_timer.h"
#include "board.h"
#include "led.h"

/**
 * @brief ��ʼ���������Ź�
 *        ��Ƶ����=4*2^prer.�����ֵֻ����256!ͨ����Ƶ���Ӽ���Ԥ��Ƶϵ��
 *        ʱ�����(���):Tout=((4*2^prer)*rlr)/40 (ms).
 * @param[in]  prer ��Ƶ��:0~7(ֻ�е�3λ��Ч!)
 * @param[out] rlr rlr:��װ�ؼĴ���ֵ:��11λ��Ч.
 */
void IWDG_Init(u8 prer, u16 rlr) 
{
	IWDG->KR = 0X5555;//ʹ�ܶ�IWDG->PR��IWDG->RLR��д		 										  
  	IWDG->PR = prer;  //���÷�Ƶ���� -> �������Ƶϵ��   
  	IWDG->RLR = rlr;  //�Ӽ��ؼĴ��� IWDG->RLR  
	IWDG->KR = 0XAAAA;//reload, ���³�ֵ
  	IWDG->KR = 0XCCCC;//ʹ�ܿ��Ź�	
}

/**
 * @brief ι�������Ź�
 * 
 */
void IWDG_Feed(void)
{
	IWDG->KR=0XAAAA;//reload											   
}

/*************** ���ڿ��Ź� *********************/
//����WWDG������������ֵ,Ĭ��Ϊ���. 
u8 WWDG_CNT=0x7f; 

/**
 * @brief ��ʼ�����ڿ��Ź� 	
 *        fprer:��Ƶϵ����WDGTB��,�����2λ��Ч 
 *        Fwwdg=PCLK1/(4096*2^fprer). 
 * @param[in]  tr T[6:0],������ֵ 
 * @param[out] wr 
 * @param fprer W[6:0],����ֵ 
 */
void WWDG_Init(u8 tr,u8 wr,u8 fprer) 
{    
	RCC->APB1ENR |= (1 << 11); 	//ʹ��wwdgʱ�� 
	WWDG_CNT = (tr & WWDG_CNT);   //��ʼ��WWDG_CNT.     
	WWDG->CFR |= fprer<<7;    //PCLK1/4096�ٳ�2^fprer 
	WWDG->CFR &= 0XFF80;      
	WWDG->CFR |= wr;     		//�趨����ֵ      
	WWDG->CR |= WWDG_CNT; 	//�趨������ֵ 
	WWDG->CR |= (1 << 7);  		//�������Ź�      
	MY_NVIC_Init(WWDT_PRIEMPTION_PRIORITY, WWDT_SUB_PRIORITY, WWDG_IRQn, WWDT_NVIC_GROUP);//��ռ2�������ȼ�3����2     
	WWDG->SR = 0X00; 			//�����ǰ�����жϱ�־λ 
	WWDG->CFR |= (1 << 9);        //ʹ����ǰ�����ж� 
} 

//������WWDG��������ֵ 
void WWDG_Set_Counter(u8 cnt) 
{ 
	WWDG->CR =(cnt & 0x7F);//������7λ������ 
} 

/**
 * @brief ���ڿ��Ź����жϷ����� -> ͨ�����ڿ��Ź�����ʵ�ֶ�����Ź���
 * 
 */
void WWDG_IRQHandler(void)
{
    //printf("%s\r\n", __func__); //���������ж���������ֿ��ܵ�����ʱ�ĺ���, ����ֻ�����ñ�־λ
	WWDG_Set_Counter(WWDG_CNT);//���贰�ڿ��Ź���ֵ!         
	WWDG->SR = 0X00;//�����ǰ�����жϱ�־λ 
	LED1 =! LED1; 
    return;
}