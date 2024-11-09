#ifndef __SNAKE_H
#define __SNAKE_H

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include<stdlib.h>
#include<time.h>
#include "light.h"
#include "MPU6050.h"
#include "DRV2605L.h"


uint8_t snakeGame(uint8_t (**ledBlock)[8],uint8_t *status);


#endif