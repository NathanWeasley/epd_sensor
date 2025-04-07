#include "EPD/epd_api.h"

#define IMG_SIZE (((EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT)

static uint8_t ImageBuf[IMG_SIZE];

void EPD_Refresh(void)
{
    EPD_SendCommand(0x22);
    EPD_SendData(0xF7);
    
    EPD_SendCommand(0x20); // Activate Display Update Sequence

    while (GPIOB->)
}

uint8_t EPD_GetSwitch()
{
    uint32_t idr = GPIOB->IDR;
    return (idr & LL_GPIO_PIN_14) > 0;
}

void EPD_SetWindow(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
    EPD_SendCommand(0x44); // SET_RAM_X_ADDRESS_START_END_POSITION
    EPD_SendData((Xstart>>3) & 0xFF);
    EPD_SendData((Xend>>3) & 0xFF);
    
    EPD_SendCommand(0x45); // SET_RAM_Y_ADDRESS_START_END_POSITION
    EPD_SendData(Ystart & 0xFF);
    EPD_SendData((Ystart >> 8) & 0xFF);
    EPD_SendData(Yend & 0xFF);
    EPD_SendData((Yend >> 8) & 0xFF);
}

void EPD_SetCursor(uint16_t Xstart, uint16_t Ystart)
{
    EPD_SendCommand(0x4E); // SET_RAM_X_ADDRESS_COUNTER
    EPD_SendData(Xstart & 0xFF);

    EPD_SendCommand(0x4F); // SET_RAM_Y_ADDRESS_COUNTER
    EPD_SendData(Ystart & 0xFF);
    EPD_SendData((Ystart >> 8) & 0xFF);
}

void EPD_Init(void)
{
    /** Enable GPIO */
    EPD_GPIO_Init();

    /** Enable power delivered to EPD */
    EPD_SetPower(1);
    EPD_Delay_ms(100);

    /** Hardware reset */
    EPD_Reset();
    EPD_WaitForBusy();

    /** Enable SPI */
    EPD_SPI_Init();

    /** Software reset */
    EPD_SendCommand(0x12);
    EPD_WaitForBusy();

    /** Drier output control */
    EPD_SendCommand(0x01);
    EPD_SendData(0xF9);
    EPD_SendData(0x00);
    EPD_SendData(0x00);

    /** Data entry mode */
    EPD_SendCommand(0x11);
    EPD_SendData(0x03);

    EPD_SetWindow(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
    EPD_SetCursor(0, 0);

    /** Border waveform */
    EPD_SendCommand(0x3C);
    EPD_SendData(0x05);
    
    /** Temperature sensor */
    EPD_SendCommand(0x1A);
    EPD_SendData(0x0F);
    EPD_SendData(0x00);

    /** Display update control */
    EPD_SendCommand(0x21);
    EPD_SendData(0x80);
    EPD_SendData(0x80);

    EPD_WaitForBusy();
}

void EPD_DeInit(void)
{
    LL_SPI_DeInit(SPI1);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SPI1);
}

void EPD_Clear(void)
{
    uint16_t Width, Height;
    Width = EPD_WIDTH_BYTE;
    Height = EPD_HEIGHT;
    
    EPD_SendCommand(0x24);
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            EPD_SendData(0XFF);
        }
    }
    EPD_SendCommand(0x26);
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            EPD_SendData(0XFF);
        }
    }

    EPD_Refresh();
}

void EPD_UpdateBlack(const uint8_t *blackImage)
{
    uint16_t Width, Height;
    Width = EPD_WIDTH_BYTE;
    Height = EPD_HEIGHT;
    
    EPD_SendCommand(0x24);
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            EPD_SendData(blackImage[i + j * Width]);
        }
    }
}

void EPD_UpdateRed(const uint8_t *redImage)
{
    uint16_t Width, Height;
    Width = EPD_WIDTH_BYTE;
    Height = EPD_HEIGHT;
    
    EPD_SendCommand(0x26);
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            EPD_SendData(redImage[i + j * Width]);
        }
    }
}

void EPD_UpdateAll(const uint8_t *blackImage, const uint8_t *redImage)
{
    uint16_t Width, Height;
    Width = EPD_WIDTH_BYTE;
    Height = EPD_HEIGHT;
    
    EPD_SendCommand(0x24);
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            EPD_SendData(blackImage[i + j * Width]);
        }
    }
    EPD_SendCommand(0x26);
    for (uint16_t j = 0; j < Height; j++)
    {
        for (uint16_t i = 0; i < Width; i++)
        {
            EPD_SendData(redImage[i + j * Width]);
        }
    }

    EPD_Refresh();
}

void EPD_Sleep(void)
{
    /** Enter deep sleep */
    EPD_SendCommand(0x10);
    EPD_SendData(0x01);
    EPD_Delay_ms(100);
}

uint8_t * EPD_GetVRAM()
{
    return ImageBuf;
}
