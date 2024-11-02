#ifndef __LIGHT_H
#define __LIGHT_H

#include "main.h"
#include "spi.h"
#include <string.h>

#define CS_PORT  nss_GPIO_Port
#define CS_PIN   nss_Pin
#define MAX_TIMEOUT 1000

#define __Select_light() HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_RESET)//CS=0
#define __Deselect_light() HAL_GPIO_WritePin(CS_PORT,CS_PIN,GPIO_PIN_SET)//CS=1

#define MAX7219_REG_NOOP		0x00
#define MAX7219_REG_D0			0x01
#define MAX7219_REG_D1			0x02
#define MAX7219_REG_D2			0x03
#define MAX7219_REG_D3			0x04
#define MAX7219_REG_D4			0x05
#define MAX7219_REG_D5			0x06
#define MAX7219_REG_D6			0x07
#define MAX7219_REG_D7			0x08
#define MAX7219_REG_DECODE		0x09
#define MAX7219_REG_INTENSITY	0x0A
#define MAX7219_REG_SCANLIMIT	0x0B
#define MAX7219_REG_SHUTDOWN	0x0C
#define MAX7219_REG_TEST		0x0F



void Light_WriteByte(uint8_t Data);
void Light_WriteBytes(uint8_t* pButter,uint16_t byteCount);
void Light_init();
void Light_Shutdown(uint8_t status);
void Light_Test(uint8_t status);
void Light_Decode(uint8_t status);
void Light_Intensity(uint8_t val);
void Light_ScanLimit(uint8_t val);

void LightsendLEDGraphics(uint8_t (*leddata)[8]);
void lightSendToMax7219(uint8_t address, uint8_t *data, uint8_t num_devices);

void Light_Tx(uint8_t reg,uint8_t val);



#endif