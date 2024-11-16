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


#include "app/snake.h"
#include "app/display.h"

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

#define MAX32BITS 0xffffffff

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

enum mpuStatus
{
  AxP,AxN,AyP,AyN,AzP,AzN,GyP,GyN
};

uint8_t fullled[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t shutdown[5][8]={0};

uint8_t Pback[8]={
  0b00000000,
  0b00011000,
  0b00111100,
  0b01111110,
  0b00011000,
  0b00011000,
  0b00000000,
  0b00000000

};
uint8_t Pfront[8] = {
  0b00000000,
  0b00000000,
  0b00011000,
  0b00011000,
  0b01111110,
  0b00111100,
  0b00011000,
  0b00000000
};
uint8_t Pleft[8] = {
  0b00000000,
  0b00010000,
  0b00110000,
  0b01111100,
  0b01111100,
  0b00110000,
  0b00010000,
  0b00000000
};
uint8_t Pright[8] = {
  0b00000000,
  0b00001000,
  0b00001100,
  0b00111110,
  0b00111110,
  0b00001100,
  0b00001000,
  0b00000000
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
  MenuItem *point;
  uint8_t status; //1左 2前 3右 4后
}pointer;



MenuItem *nowMenu;
pointer *Pointer;
MenuItem *root;
MenuItem *setting;
MenuItem *shutdownMode;

MenuItem *display;
MenuItem *display1;
MenuItem *display2;
MenuItem *display3;
MenuItem *display4;


MenuItem *game;
MenuItem *gameSnake;

MenuItem *nullMenu;

void (*runAction)(void);

uint32_t menuTimeout;

uint8_t ActionStatus=0;//0停止应用  1允许运行



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
void displayAction1();
void displayAction2();
void displayAction3();
void displayAction4();
void shutdownAction();


void gameSnakeAction();

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
    vTaskSuspend(appTaskHandle);
    ActionStatus=0;
    uint32_t mpuVal;
    uint8_t leddataTemp[5][8];
    memcpy(leddataTemp[0],Pointer->block, sizeof(Pointer->block));
    memcpy(leddataTemp[1],nowMenu->left->block,sizeof(nowMenu->left->block));
    memcpy(leddataTemp[2],nowMenu->front->block,sizeof(nowMenu->front->block));
    memcpy(leddataTemp[3],nowMenu->right->block,sizeof(nowMenu->right->block));
    memcpy(leddataTemp[4],nowMenu->back->block,sizeof(nowMenu->back->block));
    ledBlock=leddataTemp;
    if(xTaskNotifyWait(MAX32BITS,MAX32BITS,&mpuVal,pdMS_TO_TICKS(12000))==pdTRUE)
    {
      if(mpuVal==AzP)
      {
        if(!Pointer->point->action&&Pointer->point->front)
        {
          nowMenu=Pointer->point;
          switch (Pointer->status)
          {
          case 1:
            Pointer->point=Pointer->point->left;
            break;
          case 2:
            Pointer->point=Pointer->point->front;
            break;
          case 3:
            Pointer->point=Pointer->point->right;
            break;
          case 4:
            Pointer->point=Pointer->point->back;
            break;
          }
        }
        else
        {
          if(Pointer->point->action)runAction=Pointer->point->action;
          vTaskResume(appTaskHandle);
          vTaskSuspend(NULL);
        }
      }
      else if(mpuVal==GyN)
      {
        if(nowMenu->last)
        {
          nowMenu=nowMenu->last;
          runAction=shutdownAction;
        }
      }
      else if(mpuVal==AxP)
      {
        Pointer->point=nowMenu->front;
        Pointer->status=2;
        memcpy(Pointer->block,Pfront,sizeof(Pfront));
      }
      else if(mpuVal==AxN)
      {
        Pointer->point=nowMenu->back;
        Pointer->status=4;
        memcpy(Pointer->block,Pback,sizeof(Pfront));
      }
      else if(mpuVal==AyP)
      {
        Pointer->point=nowMenu->left;
        Pointer->status=1;
        memcpy(Pointer->block,Pleft,sizeof(Pfront));
      }
      else if(mpuVal==AyN)
      {
        Pointer->point=nowMenu->right;
        Pointer->status=3;
        memcpy(Pointer->block,Pright,sizeof(Pfront));
      }
    }
    else 
    {
      runAction=shutdownAction;
      vTaskResume(appTaskHandle);
      vTaskSuspend(NULL);
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

    if(datampu.Az>1.2&&eTaskGetState(mainMenuTaskHandle)==eSuspended)  //如果超过加速度阈值就唤醒菜单
    {  
      LRA_Start();                      //马达起振
      vTaskDelay(pdMS_TO_TICKS(3));
      LRA_Stop();  
      vTaskResume(mainMenuTaskHandle);
      vTaskDelay(1000);
      MPU6050_StartDMArx();
      vTaskDelay(pdMS_TO_TICKS(5));
    }
    if(eTaskGetState(mainMenuTaskHandle)==eBlocked)
    {
      if(datampu.Az>0.3)   
      {
        LRA_Start();                      //马达起振
        vTaskDelay(pdMS_TO_TICKS(3));
        LRA_Stop();  
        xTaskNotify(mainMenuTaskHandle,AzP,eSetValueWithOverwrite);
        vTaskDelay(1000);
      }
      else if(datampu.Gy<-150)xTaskNotify(mainMenuTaskHandle,GyN,eSetValueWithOverwrite);
      else if(datampu.Ax>0.7) xTaskNotify(mainMenuTaskHandle,AxP,eSetValueWithOverwrite);
      else if(datampu.Ax<-0.7)xTaskNotify(mainMenuTaskHandle,AxN,eSetValueWithOverwrite);
      else if(datampu.Ay>0.7) xTaskNotify(mainMenuTaskHandle,AyP,eSetValueWithOverwrite);
      else if(datampu.Ay<-0.7)xTaskNotify(mainMenuTaskHandle,AyN,eSetValueWithOverwrite);
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
    if(runAction)runAction();
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
  shutdownMode=malloc(sizeof(MenuItem));

  display=malloc(sizeof(MenuItem));
  display1=malloc(sizeof(MenuItem));
  display2=malloc(sizeof(MenuItem));
  display3=malloc(sizeof(MenuItem));
  display4=malloc(sizeof(MenuItem));


  game=malloc(sizeof(MenuItem));
  gameSnake=malloc(sizeof(MenuItem));

  nullMenu=malloc(sizeof(MenuItem));
  



  //nullMenu
  memcpy(nullMenu->block,fullled,sizeof(fullled));
  nullMenu->action=NULL;
  nullMenu->back=NULL;
  nullMenu->front=NULL;
  nullMenu->left=NULL;
  nullMenu->right=NULL;
  nullMenu->last=NULL;
  
  //root
  root->front=display;
  root->left=setting;
  root->right=game;
  root->back=shutdownMode;
  root->action=NULL;
  root->last=root;

  
  //pointer
  Pointer->status=2;
  Pointer->point=display;
  memcpy(Pointer->block,Pfront,sizeof(Pfront));

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
  setting->action=NULL;
  setting->last=root;

  //display
  uint8_t displayicon[8]={
  0b00000000,
  0b00000000,
  0b00011000,
  0b00100100,
  0b01000010,
  0b01111110,
  0b00000000,
  0b00000000
  };
  memcpy(display->block,displayicon,sizeof(displayicon));
  display->action=NULL;
  display->last=root;
  display->front=display1;
  display->left=display2;
  display->right=display3;
  display->back=display4;
  //1
  uint8_t display1icon[8]={
  0b10000001,
  0b01111110,
  0b01011010,
  0b01111110,
  0b01111110,
  0b01011010,
  0b01111110,
  0b10000001
  };
  memcpy(display1->block,display1icon,sizeof(display1icon));
  display1->action=displayAction1;
  display1->back=NULL;
  display1->front=NULL;
  display1->left=NULL;
  display1->right=NULL;
  display1->last=display;
  //2
  uint8_t display2icon[8]={
  0b01111110,
  0b10100101,
  0b11011011,
  0b01100110,
  0b01100110,
  0b11011011,
  0b10100101,
  0b01111110
  };
  memcpy(display2->block,display2icon,sizeof(display2icon));
  display2->action=displayAction2;
  display2->back=NULL;
  display2->front=NULL;
  display2->left=NULL;
  display2->right=NULL;
  display2->last=display;
  //3
  uint8_t display3icon[8]={
  0b11000011,
  0b10011001,
  0b00111100,
  0b01111110,
  0b01111110,
  0b00111100,
  0b10011001,
  0b11000011
  };
  memcpy(display3->block,display3icon,sizeof(display3icon));
  display3->action=displayAction3;
  display3->back=NULL;
  display3->front=NULL;
  display3->left=NULL;
  display3->right=NULL;
  display3->last=display;
  //4
  uint8_t display4icon[8]={
  0b11100111,
  0b10100101,
  0b11000011,
  0b00011000,
  0b00011000,
  0b11000011,
  0b10100101,
  0b11100111
  };
  memcpy(display4->block,display4icon,sizeof(display4icon));
  display4->action=displayAction4;
  display4->back=NULL;
  display4->front=NULL;
  display4->left=NULL;
  display4->right=NULL;
  display4->last=display;


  //game
  uint8_t gameicon[8]={
  0b00000000,
  0b00000110,
  0b01110100,
  0b01010100,
  0b01010100,
  0b01011100,
  0b01000000,
  0b00000000
  };
  memcpy(game->block,gameicon,sizeof(gameicon));
  game->action=gameSnakeAction;
  game->last=root;
  game->back=NULL;
  game->front=NULL;
  game->left=NULL;
  game->right=NULL;

  //shutdownMode
  uint8_t shutdownModeicon[8]={
  0b00000000,
  0b00111100,
  0b00000010,
  0b01111010,
  0b01111010,
  0b00000010,
  0b00111100,
  0b00000000
  };
  memcpy(shutdownMode->block,shutdownModeicon,sizeof(shutdownModeicon));
  shutdownMode->action=shutdownAction;
  shutdownMode->back=NULL;
  shutdownMode->front=NULL;
  shutdownMode->left=NULL;
  shutdownMode->right=NULL;
  shutdownMode->last=root;

  
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


void displayAction1()
{
  ledBlock=beginLed;
  vTaskDelay(100);
}
void displayAction2()
{
  ActionStatus=1;
  display1Main(&ledBlock,&ActionStatus);
}
void displayAction3()
{
  ActionStatus=1;
  display2Main(&ledBlock,&ActionStatus);
}
void displayAction4()
{
  vTaskDelay(100);
}


void shutdownAction()
{
  ledBlock=shutdown;
  vTaskDelay(1000);
}

void gameSnakeAction()
{
  ActionStatus=1;
  snakeGame(&ledBlock,&ActionStatus);
}

/* USER CODE END Application */

