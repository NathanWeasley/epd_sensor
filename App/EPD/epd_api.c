#include "EPD/epd_api.h"
#include "EPD/epd_driver.h"

#define IMG_SIZE (((EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT)

static uint8_t ImageBuf[IMG_SIZE];

void EPD_TurnOnDisplay(void)
{
    EPD_SendCommand(0x22);
    EPD_SendData(0xF7);
    
    EPD_SendCommand(0x20); // Activate Display Update Sequence
    
    EPD_WaitForBusy();
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
    /** Enable power delivered to EPD */
    EPD_SetPower(1);
    EPD_Delay_ms(100);

    /** Hardware reset */
    EPD_Reset();
    EPD_WaitForBusy();

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

//    EPD_SendCommand(0x44); //set Ram-X address start/end position
//    EPD_SendData(0x00);
//    EPD_SendData(0x0F);    //0x0F-->(15+1)*8=128

//    EPD_SendCommand(0x45); //set Ram-Y address start/end position
//    EPD_SendData(0x00);   //0xF9-->(249+1)=250
//    EPD_SendData(0x00);
//    EPD_SendData(0xF9);
//    EPD_SendData(0x00);

    EPD_SetWindow(0, 0, EPD_WIDTH-1, EPD_HEIGHT-1);
    EPD_SetCursor(0, 0);

    /** Border waveform */
    EPD_SendCommand(0x3C);
    EPD_SendData(0x05);

//    EPD_SendCommand(0x18); //Read built-in temperature sensor
//    EPD_SendData(0x80);
    
    /** Temperature sensor */
    EPD_SendCommand(0x1A);
    EPD_SendData(0x0F);
    EPD_SendData(0x00);

    /** Display update control */
    EPD_SendCommand(0x21);
    EPD_SendData(0x80);            ///< Formerly 0x80
    EPD_SendData(0x80);

//    EPD_SendCommand(0x4E);   // set RAM x address count to 0;
//    EPD_SendData(0x00);
//    EPD_SendCommand(0x4F);   // set RAM y address count to 0X199;    
//    EPD_SendData(0x00);
//    EPD_SendData(0x00);

    EPD_WaitForBusy();
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

    EPD_TurnOnDisplay();
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

    EPD_TurnOnDisplay();
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
