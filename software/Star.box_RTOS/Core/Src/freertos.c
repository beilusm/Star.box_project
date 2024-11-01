/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "queue.h"
#include "semphr.h"

#include "light.h"
#include "MPU6050.h"
#include "DRV2605L.h"
#include "math.h"
#include <stdlib.h>
#include <stdbool.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint8_t led1[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t led2[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t led3[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t led4[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t led5[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};



uint32_t menuTimeout;





/* USER CODE END Variables */
/* Definitions for mainMenuTask */
osThreadId_t mainMenuTaskHandle;
const osThreadAttr_t mainMenuTask_attributes = {
  .name = "mainMenuTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for refreshLightTas */
osThreadId_t refreshLightTasHandle;
const osThreadAttr_t refreshLightTas_attributes = {
  .name = "refreshLightTas",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MPUTask */
osThreadId_t MPUTaskHandle;
const osThreadAttr_t MPUTask_attributes = {
  .name = "MPUTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for appTask */
osThreadId_t appTaskHandle;
const osThreadAttr_t appTask_attributes = {
  .name = "appTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */




/* USER CODE END FunctionPrototypes */

void mainMenu(void *argument);
void refreshLight(void *argument);
void MPUWatch(void *argument);
void appRun(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of mainMenuTask */
  mainMenuTaskHandle = osThreadNew(mainMenu, NULL, &mainMenuTask_attributes);

  /* creation of refreshLightTas */
  refreshLightTasHandle = osThreadNew(refreshLight, NULL, &refreshLightTas_attributes);

  /* creation of MPUTask */
  MPUTaskHandle = osThreadNew(MPUWatch, NULL, &MPUTask_attributes);

  /* creation of appTask */
  appTaskHandle = osThreadNew(appRun, NULL, &appTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_mainMenu */
/**
* @brief Function implementing the mainMenuTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_mainMenu */
void mainMenu(void *argument)
{
  /* USER CODE BEGIN mainMenu */
  
  vTaskSuspend(NULL);
  /* Infinite loop */
  for(;;)
  {
    if(HAL_GetTick()-menuTimeout>10000)
    {
      vTaskResume(appTaskHandle);
      vTaskSuspend(NULL);
    }
    else
    {
      vTaskSuspend(appTaskHandle);

      menuinit();


    }

    vTaskDelay(pdMS_TO_TICKS(100));  // 每100ms执行一次循环
    

    
  }
  /* USER CODE END mainMenu */
}

/* USER CODE BEGIN Header_refreshLight */
/**
* @brief Function implementing the refreshLightTas thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_refreshLight */
void refreshLight(void *argument)
{
  /* USER CODE BEGIN refreshLight */
  uint8_t IncreaseAndDecrease = 0; //0:+ 1:-
  uint8_t Intval=0;
  
  /* Infinite loop */
  for(;;)
  {
    /*if(IncreaseAndDecrease==0)
    {
      Light_Tx(MAX7219_REG_INTENSITY,Intval);
      Intval++;
    }
    if(IncreaseAndDecrease==1)
    {
      Light_Tx(MAX7219_REG_INTENSITY,Intval);
      Intval--;
    }
    if(Intval>0x0f) 
    {
      IncreaseAndDecrease=1;
      Intval=0x0f;
    }
    if(Intval<0x01) 
    {
      IncreaseAndDecrease=0;
      Intval=0x00;
    }*/
    LightsendLEDGraphics(led1,led2,led3,led4,led5);
    
    vTaskDelay(pdMS_TO_TICKS(50));
  }
  /* USER CODE END refreshLight */
}

/* USER CODE BEGIN Header_MPUWatch */
/**
* @brief Function implementing the MPUTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MPUWatch */
void MPUWatch(void *argument)
{
  /* USER CODE BEGIN MPUWatch */
  
  /* Infinite loop */
  for(;;)
  {
    
    MPU6050_StartDMArx();

    vTaskDelay(pdMS_TO_TICKS(5));

    if(datampu.Az>2)  //如果超过加速度阈值就唤醒菜单
    {  
      LRA_Start();                      //马达起振
      vTaskDelay(pdMS_TO_TICKS(4));
      LRA_Stop();  

      menuTimeout=HAL_GetTick();
      vTaskResume(mainMenuTaskHandle);


    }

    else if(datampu.Ay>0.7||datampu.Ay<-0.7||datampu.Ax>0.7||datampu.Ax<-0.7)  //如果轴倾斜
    {  
      menuTimeout=HAL_GetTick();
    }
    

    vTaskDelay(pdMS_TO_TICKS(100));  //采样率

  }
  /* USER CODE END MPUWatch */
}

/* USER CODE BEGIN Header_appRun */
/**
* @brief Function implementing the appTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_appRun */
void appRun(void *argument)
{
  /* USER CODE BEGIN appRun */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END appRun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */




/* USER CODE END Application */

