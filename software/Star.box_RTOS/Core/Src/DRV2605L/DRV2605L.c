#include "DRV2605L/DRV2605L.h"

HAL_StatusTypeDef DRV2605L_Writebyte(uint8_t reg,uint8_t val)
{
    return HAL_I2C_Mem_Write(&hi2c2,DRV2605_I2C_ADDR,reg,I2C_MEMADD_SIZE_8BIT,&val,1,1000);
}

HAL_StatusTypeDef DRV2605L_Readbyte(uint8_t reg,uint8_t* val)
{
    return HAL_I2C_Mem_Read(&hi2c2,DRV2605_I2C_ADDR,reg,I2C_MEMADD_SIZE_8BIT,val,1,1000);
}

HAL_StatusTypeDef DRV2605L_Writebit(uint8_t reg,uint8_t bitNum, uint8_t data)
{
    uint8_t tmp;
    DRV2605L_Readbyte(reg,&tmp);
    tmp = (data != 0) ? (tmp | (1 << bitNum)) : (tmp & ~(1 << bitNum));
    return DRV2605L_Writebyte(reg,tmp);
}

HAL_StatusTypeDef DRV2605L_Writebytes(uint8_t reg_addr,uint8_t length, uint8_t *data)
{
    return HAL_I2C_Mem_Write(&hi2c2,DRV2605_I2C_ADDR, reg_addr,I2C_MEMADD_SIZE_8BIT, data, length, 1000);
}
HAL_StatusTypeDef DRV2605L_Readbytes(uint8_t reg_addr, uint8_t length,uint8_t *data)
{
    return HAL_I2C_Mem_Read(&hi2c2,DRV2605_I2C_ADDR, reg_addr,I2C_MEMADD_SIZE_8BIT, data, length, 1000);
}

HAL_StatusTypeDef DRV2605L_writeBits(uint8_t reg_addr,uint8_t bitStart, uint8_t length, uint8_t data) 
{
    uint8_t tmp, dataShift;
    HAL_StatusTypeDef status = DRV2605L_Readbytes(reg_addr, 1, &tmp);
    if (status == HAL_OK) 
    {
        uint8_t mask = (((1 << length) - 1) << (bitStart - length + 1));
        dataShift = data << (bitStart - length + 1);
        tmp &= mask;
        tmp |= dataShift;
        return DRV2605L_Writebytes(reg_addr, 1, &tmp);
    } 
    else 
    {
        return status;
    }
}



void DRV2605L_SetERMorLRA(uint8_t val)
{
    DRV2605L_Writebit(DRV2605_REG_FEEDBACK,7,val);
}

void DRV2605L_SetFB_BRAKE_FACTOR(uint8_t val)
{
    DRV2605L_writeBits(DRV2605_REG_FEEDBACK,DRV2605_FC_FB_BRAKE_FACTOR_BIT,3,val);
}

void DRV2605L_SetLOOP_GAIN(uint8_t val)
{
    DRV2605L_writeBits(DRV2605_REG_FEEDBACK,DRV2605_FC_LOOP_GAIN_BIT,2,val);
}


void DRV2605L_SetAUTO_CAL_TIME(uint8_t val)
{
    DRV2605L_writeBits(DRV2605_REG_CONTROL4,DRV2605_CTRL4_AUTO_CAL_TIME_BIT,2,val);
}

void DRV2605L_SetDrive_TIME(uint8_t val)
{
    DRV2605L_writeBits(DRV2605_REG_CONTROL1,DRV2605_CTRL1_DRIVE_TIME_BIT,5,val);
}

void DRV2605L_SetLib(uint8_t val)
{
    DRV2605L_writeBits(DRV2605_REG_LIBRARY,DRV2605_LS_LIBRARY_SEL_BIT,3,val);
}

void DRV2605L_Init(void)
{
    DRV2605L_Writebyte(DRV2605_REG_MODE,0x07); //移除standby并进入自动校准模式
    /*写入自动校准参数*/

    DRV2605L_SetERMorLRA(1); //LRA
    DRV2605L_SetFB_BRAKE_FACTOR(0x02);
    DRV2605L_SetLOOP_GAIN(0x02);


    DRV2605L_Writebyte(DRV2605_REG_RATEDV,255);//设置额定电压 81
    DRV2605L_Writebyte(DRV2605_REG_CLAMPV,255);//设置峰值电压
    DRV2605L_SetAUTO_CAL_TIME(0x02);
    DRV2605L_SetDrive_TIME(10);  //31
    //设置GO位
    DRV2605L_Writebyte(DRV2605_REG_GO,0x01);//启动自动校准


    //设置库
    DRV2605L_SetLib(0x06); //LRA

}


void DRV2605L_SetWaveformLibrary(uint8_t val)
{
    DRV2605L_Writebyte(DRV2605_REG_WAVESEQ1,val);
}

HAL_StatusTypeDef LRA_Start(void)
{
    return DRV2605L_Writebyte(DRV2605_REG_GO,0x01);
}

HAL_StatusTypeDef LRA_Stop(void)
{
    return DRV2605L_Writebyte(DRV2605_REG_GO,0x00);
}

void LRA_LightClick(void)
{
    LRA_Start();
}
