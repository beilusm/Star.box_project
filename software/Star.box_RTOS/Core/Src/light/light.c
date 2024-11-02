#include "light/light.h"

void Light_WriteByte(uint8_t Data)
{
    if(HAL_SPI_Transmit(&hspi1,&Data,1,MAX_TIMEOUT)!=HAL_OK)
    {
        HAL_GPIO_WritePin(led_GPIO_Port,led_Pin,GPIO_PIN_SET);
    }
}

void Light_WriteBytes(uint8_t* pBuffer,uint16_t byteCount)
{
    HAL_SPI_Transmit(&hspi1,pBuffer,byteCount,MAX_TIMEOUT);
}

void LightsendLEDGraphics(uint8_t (*leddata)[8])
{
    // 每个 MAX7219 都有8个寄存器需要写入数据
    for (int i = 0; i < 8; i++) 
    {
        uint8_t data[5] = {leddata[4][i], leddata[3][i], leddata[1][i], leddata[2][i], leddata[0][i]};
        lightSendToMax7219(i + 1, data, 5);  // MAX7219的地址从0x01到0x08
    }
}

void lightSendToMax7219(uint8_t address, uint8_t *data, uint8_t num_devices) 
{
    // 开始SPI通信
    __Select_light();

    for (int device = 0; device < num_devices; device++) 
    {
        HAL_SPI_Transmit_DMA(&hspi1, &address, 1);  // 发送地址
        HAL_SPI_Transmit_DMA(&hspi1, &data[device], 1);  // 发送数据
    }

    // 结束SPI通信
    __Deselect_light();
}

void Light_Tx(uint8_t reg,uint8_t val)
{
    uint8_t sendbuffer[2];
    sendbuffer[0]=reg;
    sendbuffer[1]=val;
    __Select_light();

    for(uint8_t i=1;i<=5;i++)
    HAL_SPI_Transmit(&hspi1,sendbuffer,2,MAX_TIMEOUT);

    __Deselect_light();
}



void Light_init()
{
    Light_Shutdown(0x01);//01 enable  00 disable
    Light_Test(0x00);
    Light_Decode(0x00);
    Light_Intensity(0x00);
    Light_ScanLimit(0x07);
}

void Light_Shutdown(uint8_t status)
{
    Light_Tx(MAX7219_REG_SHUTDOWN,status);
}

void Light_Test(uint8_t status)
{
    Light_Tx(MAX7219_REG_TEST,status);
}

void Light_Decode(uint8_t status)
{
    Light_Tx(MAX7219_REG_DECODE,status);
}

void Light_Intensity(uint8_t val)
{
    Light_Tx(MAX7219_REG_INTENSITY,val);
}

void Light_ScanLimit(uint8_t val)
{
    Light_Tx(MAX7219_REG_SCANLIMIT,val);
}
