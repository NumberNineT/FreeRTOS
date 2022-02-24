#include "main.h"
#include "timer.h"
#include "my_timer.h"

#include "led0_thread.h"
#include "led1_thread.h"
#include "interrupt_thread.h"
#include "thread_ui.h"
#include "thread_algo.h"
#include "thread_key.h"
#include "thread_dual_comm.h"
#include "thread_monitor.h"
#include "thread_manager.h"
/**************************** global varible ******************************/
TaskHandle_t StartTask_Handler;     //������

/**************************** macro definition ******************************/

/**************************** macro definition ******************************/

/**
 * @brief hardware_init
 * 
 */
void hardware_init()
{
    //����ϵͳ�ж����ȼ�����4��	
	//0-15 �ܹ�16�����ȼ���û����ռ���ȼ�	 	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();	    				//��ʱ������ʼ��	  
	uart_init(115200);					//��ʼ������
	LED_Init();		  					//��ʼ��LED

    return;
}

/**
 * @brief software_init
 * 
 */
void software_init()
{
    // TIM3_Int_Init(10000-1,7200-1);		
	// TIM5_Int_Init(10000-1,7200-1);	

    return;
}

/**
 * @brief  thread manager thread entry function
 * 
 * @param pvParameters 
 */
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���,�ٽ�δ��뱣��

    //
    // Manager task
    //
    xTaskCreate((TaskFunction_t )thread_manager_task,     
                (const char*    )"manager_task",   
                (uint16_t       )MANAGER_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MANAGER_TASK_PRIO,
                (TaskHandle_t*  )&ManagerTask_Handler); 

    //
    //����LED0����
    //
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);  

    //
    //����LED1����
    //
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler); 

    //
    // key task
    //
    xTaskCreate((TaskFunction_t )key_task,     
                (const char*    )"key_task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KeyTask_Handler); 

    //
    // interrupt task
    //
    // xTaskCreate((TaskFunction_t )interrupt_task,     
    //             (const char*    )"interrupt_task",   
    //             (uint16_t       )INTERRUPT_STK_SIZE, 
    //             (void*          )NULL,
    //             (UBaseType_t    )INTERRUPT_TASK_PRIO,
    //             (TaskHandle_t*  )&INTERRUPTTask_Handler); 

    //
    // ui_task  
    //
    xTaskCreate((TaskFunction_t )thread_ui_entry,     
                (const char*    )"thread_ui",   
                (uint16_t       )THREAD_UI_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )THREAD_UI_PRIO,
                (TaskHandle_t*  )&Thread_UI_Handler); 

    //
    // dual_comm_task  
    //
    // xTaskCreate((TaskFunction_t )dual_comm_task,     
    //             (const char*    )"thread_dual_comm",   
    //             (uint16_t       )DUAL_COMM_STK_SIZE, 
    //             (void*          )NULL,
    //             (UBaseType_t    )DUAL_COMM_TASK_PRIO,
    //             (TaskHandle_t*  )&DualCommTask_Handler); 
    //
    // static create task
    // xTaskCreateStatic() �ķ���ֵ��������
    //
    // ThreadAlgo_Handler = xTaskCreateStatic(	(TaskFunction_t )thread_algo_enery,
    //                                         (const char*    )"thread_algo",
    //                                         (uint16_t       )THREAD_ALGO_STK_SIZE,
    //                                         ( void *        )NULL,
    //                                         (UBaseType_t    )THREAD_ALGO_PRIO,
    //                                         (StackType_t*   )ThreadAlgoStack,
    //                                         (StaticTask_t*  )&ThreadAlgoTCB );
    
    //
    // monitor thread
    //
    monitor_binary_handle = xTaskCreateStatic(	(TaskFunction_t )thread_monitor_enery,
                                                (const char*    )"thread_monitor",
                                                (uint16_t       )THREAD_MONITOR_STK_SIZE,
                                                ( void *        )NULL,
                                                (UBaseType_t    )THREAD_MOINITOR_PRIO,
                                                (StackType_t*   )ThreadMonitorStack,
                                                (StaticTask_t*  )&ThreadMonitorTCB );
        


    // �������˳� while(1) ʱ, һ��Ҫɾ������
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
	
	return;
}

