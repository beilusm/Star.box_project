#ifndef __DISPLAY_H
#define __DISPLAY_H


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include<math.h>

#include<stdlib.h>
#include<time.h>


#include "light.h"
#include "MPU6050.h"
#include "DRV2605L.h"


void display1Main(uint8_t (**led)[8],uint8_t *status);
void display2Main(uint8_t (**led)[8],uint8_t *status);

#endif