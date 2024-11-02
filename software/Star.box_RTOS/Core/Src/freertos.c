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

#define PointFront 0
#define PointBack 1
#define PointRight 2
#define PointLeft 3


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */


uint8_t fullled[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

uint8_t Pback[8]={
  0x00,
  0x00,
  0x18,
  0x3c,
  0x7e,
  0x7e,
  0x00,
  0x00
};
uint8_t Pfront[8] = {
  0x00,
  0x00,
  0x7e,
  0x7e,
  0x3c,
  0x18,
  0x00,
  0x00
};
uint8_t Pleft[8] = {
  0x10,
  0x30,
  0x70,
  0xf0,
  0xf0,
  0x70,
  0x30,
  0x10
};
uint8_t Pright[8] = {
  0x08,
  0x0c,
  0x0e,
  0x0f,
  0x0f,
  0x0e,
  0x0c,
  0x08
};



uint8_t beginLed[5][8];
uint8_t (*ledBlock)[8];

typedef struct MenuItem {
  uint8_t block[8];

  struct MenuItem *front;
  struct MenuItem *back;
  struct MenuItem *left;
  struct MenuItem *right;

  struct MenuItem *last;

  void (*action)(void);
}MenuItem;

typedef struct pointer
{
  uint8_t block[8];
  uint8_t point;
}pointer;



MenuItem *nowMenu;
pointer *Pointer;
MenuItem *root;
MenuItem *setting;
MenuItem *display;
MenuItem *game;
MenuItem *nullMenu;


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


void menuInit();

void rootAction();

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
  
  menuInit();

  vTaskSuspend(NULL);
  /* Infinite loop */
  for(;;)
  {
    if(HAL_GetTick()-menuTimeout>8000)
    {
      vTaskResume(appTaskHandle);
      vTaskSuspend(NULL);
    }
    else
    {
      vTaskSuspend(appTaskHandle);

      
      if(datampu.Ax>0.7)
      {
        Pointer->point=PointFront;
        memcpy(Pointer->block,Pfront,sizeof(Pfront));
      }
      else if(datampu.Ax<-0.7)
      {
        Pointer->point=PointBack;
        memcpy(Pointer->block,Pback,sizeof(Pfront));
      }
      else if(datampu.Ay>0.7)
      {
        Pointer->point=PointLeft;
        memcpy(Pointer->block,Pleft,sizeof(Pfront));
      }
      else if(datampu.Ay<-0.7)
      {
        Pointer->point=PointRight;
        memcpy(Pointer->block,Pright,sizeof(Pfront));
      }
      else if(datampu.Az>1.2)
      {
        if(Pointer->point==PointFront)
        {
          nowMenu=nowMenu->front;
        }
        else if(Pointer->point==PointBack)
        {
          nowMenu=nowMenu->back;
        }
        else if(Pointer->point==PointLeft)
        {
          nowMenu=nowMenu->left;
        }
        if(Pointer->point==PointRight)
        {
          nowMenu=nowMenu->right;
        }
      }

      uint8_t leddataTemp[5][8];
      memcpy(leddataTemp[0],Pointer->block, sizeof(Pointer->block));
      memcpy(leddataTemp[1],nowMenu->left->block,sizeof(nowMenu->left->block));
      memcpy(leddataTemp[2],nowMenu->back->block,sizeof(nowMenu->back->block));
      memcpy(leddataTemp[3],nowMenu->back->block,sizeof(nowMenu->back->block));
      memcpy(leddataTemp[4],nowMenu->back->block,sizeof(nowMenu->back->block));
      ledBlock=leddataTemp;

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
  

  for (int i = 0; i < 5; i++) 
  {
    memcpy(beginLed[i], fullled, sizeof(fullled));
  }
  ledBlock=beginLed;

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
    LightsendLEDGraphics(ledBlock);
    vTaskDelay(pdMS_TO_TICKS(100));
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

    if(datampu.Az>1)  //如果超过加速度阈值就唤醒菜单
    {  
      LRA_Start();                      //马达起振
      vTaskDelay(pdMS_TO_TICKS(3));
      LRA_Stop();  

      menuTimeout=HAL_GetTick();
      vTaskResume(mainMenuTaskHandle);
      vTaskDelay(1000);

    }

    else if(datampu.Ay>0.7||datampu.Ay<-0.7||datampu.Ax>0.7||datampu.Ax<-0.7)  //如果轴倾斜
    {  


      LRA_Start();                      //马达起振
      vTaskDelay(pdMS_TO_TICKS(3));
      LRA_Stop();  

      menuTimeout=HAL_GetTick();
      vTaskResume(mainMenuTaskHandle);
      vTaskDelay(1000);


      //menuTimeout=HAL_GetTick();
    }
    

    vTaskDelay(pdMS_TO_TICKS(10));  //采样率

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
    nowMenu->action();
    osDelay(10);
  }
  /* USER CODE END appRun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


void menuInit()
{

  
  //init menu val
  
  nowMenu=malloc(sizeof(MenuItem));
  Pointer=malloc(sizeof(pointer));
  root=malloc(sizeof(MenuItem));
  setting=malloc(sizeof(MenuItem));
  display=malloc(sizeof(MenuItem));
  game=malloc(sizeof(MenuItem));
  nullMenu=malloc(sizeof(MenuItem));
  

  //pointer
  Pointer->point=PointFront;
  memcpy(Pointer->block,Pfront,sizeof(Pfront));


  //nullMenu
  memcpy(nullMenu->block,fullled,sizeof(fullled));
  
  //root
  root->front=display;
  root->left=setting;
  root->right=game;
  root->back=nullMenu;
  root->action=rootAction;

  //setting
  uint8_t settingicon[8] = {
    0x00, 
    0x5a, 
    0x24, 
    0x5a, 
    0x5a, 
    0x24, 
    0x5a, 
    0x00
  };
  memcpy(setting->block,settingicon,sizeof(settingicon));


  
  nowMenu=root;
}


void rootAction()
{
  uint8_t IncreaseAndDecrease = 0; //0:+ 1:-
  uint8_t Intval=0;

  for(;;)
  {
    
    ledBlock=beginLed;
    if(IncreaseAndDecrease==0)
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
    }
    vTaskDelay(100);
  }
}


/* USER CODE END Application */

