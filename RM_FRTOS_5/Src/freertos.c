#include "main.h"

/* USER CODE BEGIN Includes */     
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/

osThreadId defaultTaskHandle;
/* USER CODE BEGIN Variables */

TaskHandle_t shoot_task_t;
TaskHandle_t can_msg_seng_task_t;

TaskHandle_t mode_sw_task_t;
TaskHandle_t info_get_task_t;

osTimerId chassis_timer_id;
osTimerId gimbal_timer_id;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/

/* USER CODE BEGIN FunctionPrototypes */
void StartDefaultTask(void const * argument);
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
 
/* USER CODE BEGIN Application */
/* init freertos*/
void MX_FREERTOS_Init(){

  taskENTER_CRITICAL();
  
		osTimerDef(chassisTimer, chassis_task);
		chassis_timer_id = osTimerCreate(osTimer(chassisTimer), osTimerPeriodic, NULL);
	
		osTimerDef(gimTimer, gimbal_task);
		gimbal_timer_id =  osTimerCreate(osTimer(gimTimer), osTimerPeriodic, NULL);
		
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  
		/* high priority task */	
		osThreadDef(shotTask, shoot_task,osPriorityAboveNormal,0,128);
		shoot_task_t = osThreadCreate(osThread(shotTask),NULL);
		
		osThreadDef(canTask, can_msg_send_task,osPriorityAboveNormal,0,128);
		can_msg_seng_task_t = osThreadCreate(osThread(canTask),NULL);
		
		/* low priority task */
		osThreadDef(modeTask, mode_swtich_task, osPriorityNormal, 0, 128);
		mode_sw_task_t = osThreadCreate(osThread(modeTask), NULL);
		
		osThreadDef(infoTask, info_get_task, osPriorityNormal, 0, 128);
    info_get_task_t = osThreadCreate(osThread(infoTask), NULL);
		
		/* unpack task */
	
	taskEXIT_CRITICAL();
/* USER CODE END RTOS_THREADS */

/* USER CODE BEGIN RTOS_QUEUES */
/* add queues, ... */
/* USER CODE END RTOS_QUEUES */
}   
/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  //MX_USB_DEVICE_Init();

  /* init code for FATFS */
  //MX_FATFS_Init();

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}
/* USER CODE END Application */



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
