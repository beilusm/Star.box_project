#include "MPU6050/MPU6050.h"


MPU6050_t datampu;

HAL_StatusTypeDef MPU6050_Writebyte(uint8_t reg,uint8_t val)
{
    return HAL_I2C_Mem_Write(&hi2c1,MPU6050_I2C_ADDR,reg,I2C_MEMADD_SIZE_8BIT,&val,1,1000);
}
HAL_StatusTypeDef MPU6050_Readbyte(uint8_t reg,uint8_t* val)
{
    return HAL_I2C_Mem_Read(&hi2c1,MPU6050_I2C_ADDR,reg,I2C_MEMADD_SIZE_8BIT,val,1,1000);
}

HAL_StatusTypeDef MPU6050_Readbytes(uint8_t reg_addr, uint8_t length,uint8_t *data)
{
    return HAL_I2C_Mem_Read(&hi2c1,MPU6050_I2C_ADDR, reg_addr,I2C_MEMADD_SIZE_8BIT, data, length, 1000);
}

HAL_StatusTypeDef MPU6050_Writebytes(uint8_t reg_addr,uint8_t length, uint8_t *data)
{
    return HAL_I2C_Mem_Write(&hi2c1,MPU6050_I2C_ADDR, reg_addr,I2C_MEMADD_SIZE_8BIT, data, length, 1000);
}

HAL_StatusTypeDef MPU6050_writeBits(uint8_t reg_addr,uint8_t bitStart, uint8_t length, uint8_t data) 
{
    uint8_t tmp, dataShift;
    HAL_StatusTypeDef status = MPU6050_Readbytes(reg_addr, 1, &tmp);
    if (status == HAL_OK) 
    {
        uint8_t mask = (((1 << length) - 1) << (bitStart - length + 1));
        dataShift = data << (bitStart - length + 1);
        tmp &= mask;
        tmp |= dataShift;
        return MPU6050_Writebytes(reg_addr, 1, &tmp);
    } 
    else 
    {
        return status;
    }
}

HAL_StatusTypeDef MPU6050_writeBit(uint8_t reg_addr,uint8_t bitNum, uint8_t data)
{
    uint8_t tmp;
    MPU6050_Readbytes(reg_addr, 1, &tmp);
    tmp = (data != 0) ? (tmp | (1 << bitNum)) : (tmp & ~(1 << bitNum));
    return MPU6050_Writebytes(reg_addr, 1, &tmp);
}


//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
void MPU6050_setRate(uint16_t rate)
{
    uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
    MPU6050_Writebyte(MPU6050_RA_SMPLRT_DIV,data);

}

void MPU6050_setLPF(uint8_t lpf)
{
    MPU6050_writeBits(MPU6050_RA_CONFIG,MPU6050_CFG_DLPF_CFG_BIT,MPU6050_CFG_DLPF_CFG_LENGTH,lpf);
}

void MPU6050_initialize(void) {
    MPU6050_setClockSource(MPU6050_CLOCK_PLL_XGYRO); //设置时钟
    MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_250); //陀螺仪最大量程 +-250度每秒
    MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_2);	//加速度度最大量程 +-2G
    MPU6050_setRate(500);						//设置采样率500Hz
    MPU6050_setLPF(1);                          //设置低通滤波
    HAL_Delay(50);
    MPU6050_setSleepEnabled(0); //进入工作状态
    HAL_Delay(50);
    MPU6050_setI2CMasterModeEnabled(0);	 //不让MPU6050 控制AUXI2C
    MPU6050_setI2CBypassEnabled(0);	//主控制器的I2C与	MPU6050的AUXI2C	直通。控制器可以直接访问HMC5883L
}

void MPU6050_setClockSource(uint8_t source) 
{
    MPU6050_writeBits(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT,MPU6050_PWR1_CLKSEL_LENGTH, source);
}

void MPU6050_setFullScaleGyroRange(uint8_t range) 
{
    MPU6050_writeBits(MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT,MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}

void MPU6050_setFullScaleAccelRange(uint8_t range) 
{
    MPU6050_writeBits(MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT,MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}

void MPU6050_setSleepEnabled(uint8_t enabled) 
{
    MPU6050_writeBit(MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}

void MPU6050_setI2CMasterModeEnabled(uint8_t enabled) 
{
    MPU6050_writeBit(MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT,enabled);
}

void MPU6050_setI2CBypassEnabled(uint8_t enabled) 
{
  MPU6050_writeBit(MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT,enabled);
}

void MPU6050_Read_Accel( MPU6050_t *DataStruct)
{
    uint8_t Rec_Data[6];

    // Read 6 BYTES of data starting from ACCEL_XOUT_H register

    MPU6050_Readbytes(MPU6050_RA_ACCEL_XOUT_H,6,Rec_Data);

    DataStruct->Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    DataStruct->Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    DataStruct->Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

    /*** convert the RAW values into acceleration in 'g'
         we have to divide according to the Full scale value set in FS_SEL
         I have configured FS_SEL = 0. So I am dividing by 16384.0
         for more details check ACCEL_CONFIG Register              ****/

    DataStruct->Ax = DataStruct->Accel_X_RAW / 16384.0;
    DataStruct->Ay = DataStruct->Accel_Y_RAW / 16384.0;
    DataStruct->Az = DataStruct->Accel_Z_RAW / 14418.0;
}

void MPU6050_Read_Gyro(MPU6050_t *DataStruct)
{
    uint8_t Rec_Data[6];
    MPU6050_Readbytes(MPU6050_RA_GYRO_XOUT_H,6,Rec_Data);
    DataStruct->Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    DataStruct->Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
    DataStruct->Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
    DataStruct->Gx = DataStruct->Gyro_X_RAW / 131.0;
    DataStruct->Gy = DataStruct->Gyro_Y_RAW / 131.0;
    DataStruct->Gz = DataStruct->Gyro_Z_RAW / 131.0;
}

void MPU6050_Read_G_A(MPU6050_t *DataStruct)
{
    MPU6050_Read_Accel(DataStruct);
    MPU6050_Read_Gyro(DataStruct);
}

void MPU6050_EnableMotionInterrupt()
{
    // 设置运动阈值，单位：LSB (1 LSB = 4 mg)
    MPU6050_setMOT_THR(1);// 设置阈值为 80 mg
    // 设置运动检测的持续时间
    MPU6050_setMOT_DUR(80);  // 设置为 40 ms
    // 配置 INT_ENABLE 寄存器启用运动中断
    MPU6050_setINT_ENABLE(1);  // 使能运动中断
}

void MPU6050_setMOT_THR(uint8_t val)
{
    MPU6050_Writebyte(MPU6050_RA_MOT_THR,val);
}

void MPU6050_setMOT_DUR(uint8_t val)
{
    MPU6050_Writebyte(MPU6050_RA_MOT_DUR,val);
}

void MPU6050_setINT_ENABLE(uint8_t val)
{
    MPU6050_writeBit(MPU6050_RA_INT_ENABLE,6,val);
}

void MPU6050_StartDMArx()
{
    HAL_I2C_Mem_Read_DMA(&hi2c1,MPU6050_I2C_ADDR,MPU6050_RA_ACCEL_XOUT_H,I2C_MEMADD_SIZE_8BIT,MPU6050_Buffer,14);
}

uint8_t MPU6050getStatus()
{
    if(datampu.Gy>200)
    {
      return MPUGyP;
    }
    else if(datampu.Gy<-200)
    {
      return MPUGyN;
    }
    else if(datampu.Gx>200)
    {
      return MPUGxP;
    }
    else if(datampu.Gx<-200)
    {
      return MPUGxN;
    }
    //各个方向的加速度
    else if(sqrt(datampu.Ax * datampu.Ax + datampu.Ay * datampu.Ay + datampu.Az * datampu.Az)>2)
    {
      return MPUAccel;
    }
    //静止
    //else if(datampu.Az<0)
    //{
    //  return MPUStay;
    //}
    return 0;//没有操作返回0
}