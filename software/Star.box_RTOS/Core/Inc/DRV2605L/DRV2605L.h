#ifndef __DRV2605L_H
#define __DRV2605L_H

#include "i2c.h" 

#define DRV2605_I2C_ADDR          0x5A<<1

#define DRV2605_REG_STATUS        0x00
#define DRV2605_REG_MODE          0x01

#define DRV2605_MODE_INTTRIG      0x00
#define DRV2605_MODE_EXTTRIGEDGE  0x01
#define DRV2605_MODE_EXTTRIGLVL   0x02
#define DRV2605_MODE_PWMANALOG    0x03
#define DRV2605_MODE_AUDIOVIBE    0x04
#define DRV2605_MODE_REALTIME     0x05
#define DRV2605_MODE_DIAGNOS      0x06
#define DRV2605_MODE_AUTOCAL      0x07
#define DRV2605_MODE_STANDBY      0x40

#define DRV2605_REG_RTPIN         0x02
#define DRV2605_REG_LIBRARY       0x03
#define DRV2605_REG_WAVESEQ1      0x04
#define DRV2605_REG_WAVESEQ2      0x05
#define DRV2605_REG_WAVESEQ3      0x06
#define DRV2605_REG_WAVESEQ4      0x07
#define DRV2605_REG_WAVESEQ5      0x08
#define DRV2605_REG_WAVESEQ6      0x09
#define DRV2605_REG_WAVESEQ7      0x0A
#define DRV2605_REG_WAVESEQ8      0x0B

#define DRV2605_REG_GO            0x0C
#define DRV2605_REG_OVERDRIVE     0x0D
#define DRV2605_REG_SUSTAINPOS    0x0E
#define DRV2605_REG_SUSTAINNEG    0x0F
#define DRV2605_REG_BRAKE         0x10
#define DRV2605_REG_AUDIOVIBECTRL 0x11
#define DRV2605_REG_AUDIOMINLVL   0x12
#define DRV2605_REG_AUDIOMAXLVL   0x13
#define DRV2605_REG_AUDIOMINDRV   0x14
#define DRV2605_REG_AUDIOMAXDRV   0x15
#define DRV2605_REG_RATEDV        0x16
#define DRV2605_REG_CLAMPV        0x17
#define DRV2605_REG_AUTOCALCOMP   0x18
#define DRV2605_REG_AUTOCALEMP    0x19
#define DRV2605_REG_FEEDBACK      0x1A
#define DRV2605_REG_CONTROL1      0x1B
#define DRV2605_REG_CONTROL2      0x1C
#define DRV2605_REG_CONTROL3      0x1D
#define DRV2605_REG_CONTROL4      0x1E
#define DRV2605_REG_RFU1          0x1F
#define DRV2605_REG_RFU2          0x20
#define DRV2605_REG_VBAT          0x21
#define DRV2605_REG_LRARESON      0x22

#define DRV2605_FC_FB_BRAKE_FACTOR_BIT 6
#define DRV2605_FC_LOOP_GAIN_BIT 3
#define DRV2605_CTRL4_AUTO_CAL_TIME_BIT 5
#define DRV2605_CTRL1_DRIVE_TIME_BIT 4
#define DRV2605_LS_LIBRARY_SEL_BIT 2


HAL_StatusTypeDef DRV2605L_Writebyte(uint8_t reg,uint8_t val);
HAL_StatusTypeDef DRV2605L_Readbyte(uint8_t reg,uint8_t* val);
HAL_StatusTypeDef DRV2605L_Writebit(uint8_t reg,uint8_t bitNum, uint8_t data);
HAL_StatusTypeDef DRV2605L_writeBits(uint8_t reg_addr,uint8_t bitStart, uint8_t length, uint8_t data);
HAL_StatusTypeDef DRV2605L_Writebytes(uint8_t reg_addr,uint8_t length, uint8_t *data);
HAL_StatusTypeDef DRV2605L_Readbytes(uint8_t reg_addr, uint8_t length,uint8_t *data);

void DRV2605L_SetERMorLRA(uint8_t val);
void DRV2605L_SetFB_BRAKE_FACTOR(uint8_t val);
void DRV2605L_SetLOOP_GAIN(uint8_t val);
void DRV2605L_SetAUTO_CAL_TIME(uint8_t val);
void DRV2605L_SetDrive_TIME(uint8_t val);
void DRV2605L_SetLib(uint8_t val);


void DRV2605L_Init(void);
void DRV2605L_SetWaveformLibrary(uint8_t val);
HAL_StatusTypeDef LRA_Start(void);
HAL_StatusTypeDef LRA_Stop(void);

void LRA_LightClick(void);



#endif