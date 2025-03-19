#include "EPD/epd_driver.h"

#define EPD_PWR_PIN     LL_GPIO_PIN_10
#define EPD_BUSY_PIN    LL_GPIO_PIN_0       ///< PORTB
#define EPD_RST_PIN     LL_GPIO_PIN_3
#define EPD_DC_PIN      LL_GPIO_PIN_6
#define EPD_CS_PIN      LL_GPIO_PIN_4
#define EPD_CLK_PIN     LL_GPIO_PIN_5
#define EPD_DIN_PIN     LL_GPIO_PIN_7

void SPI_Transmit(uint8_t byte)
{
    while (!(SPI1->SR & SPI_SR_TXE));
    LL_SPI_TransmitData8(SPI1, byte);
    while (!(SPI1->SR & SPI_SR_TXE));
}

void EPD_Delay_ms(uint16_t ms)
{
    LL_mDelay(ms);
}

void EPD_SetPower(uint8_t onoff)
{
    if (onoff)
    {
        /** Active low for PMOS */
        GPIOA->BSRR = EPD_PWR_PIN;
    }
    else
    {
        GPIOA->BRR = EPD_PWR_PIN;
    }
}

void EPD_Reset()
{
    GPIOA->BSRR = EPD_RST_PIN;
    EPD_Delay_ms(20);
    GPIOA->BRR = EPD_RST_PIN;
    EPD_Delay_ms(20);
    GPIOA->BSRR = EPD_RST_PIN;
    EPD_Delay_ms(20);
}

void EPD_WaitForBusy()
{
	while (1)
	{	//=1 BUSY
		if ((GPIOB->IDR & EPD_BUSY_PIN) == 0) 
        {
			break;
        }
		EPD_Delay_ms(20);
	}
	EPD_Delay_ms(20);
}

void EPD_SendData(uint8_t data)
{
    GPIOA->BSRR = EPD_DC_PIN;
    GPIOA->BRR = EPD_CS_PIN;
    SPI_Transmit(data);
    GPIOA->BSRR = EPD_CS_PIN;
}

void EPD_SendCommand(uint8_t cmd)
{
    GPIOA->BRR = EPD_DC_PIN;
    GPIOA->BRR = EPD_CS_PIN;
    SPI_Transmit(cmd);
    GPIOA->BSRR = EPD_CS_PIN;
}
