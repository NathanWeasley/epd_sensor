#include "EPD/epd_driver.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_bus.h"

void EPD_GPIO_Init()
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_GPIO_SetOutputPin(EPD_RST_PORT, EPD_RST_PIN);
    LL_GPIO_SetOutputPin(EPD_CS_PORT, EPD_CS_PIN);
    LL_GPIO_SetOutputPin(EPD_DC_PORT, EPD_DC_PIN);
    LL_GPIO_SetOutputPin(EPD_PWR_PORT, EPD_PWR_PIN);

    GPIO_InitStruct.Pin = EPD_PWR_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(EPD_PWR_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = EPD_RST_PIN;
    LL_GPIO_Init(EPD_RST_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = EPD_CS_PIN;
    LL_GPIO_Init(EPD_CS_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = EPD_DC_PIN;
    LL_GPIO_Init(EPD_DC_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = EPD_BUSY_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(EPD_BUSY_PORT, &GPIO_InitStruct);
}

void EPD_SPI_Init()
{
    LL_SPI_InitTypeDef SPI_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    GPIO_InitStruct.Pin = EPD_CLK_PIN | EPD_DIN_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(EPD_SPI_PORT, &GPIO_InitStruct);

    LL_SPI_DeInit(SPI1);
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 7;
    LL_SPI_Init(SPI1, &SPI_InitStruct);
    LL_SPI_SetStandard(SPI1, LL_SPI_PROTOCOL_MOTOROLA);
    LL_SPI_Enable(SPI1);
}

void EPD_Transmit(uint8_t byte)
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
        EPD_PWR_PORT->BRR = EPD_PWR_PIN;
    }
    else
    {
        EPD_PWR_PORT->BSRR = EPD_PWR_PIN;
    }
}

void EPD_Reset()
{
    EPD_RST_PORT->BSRR = EPD_RST_PIN;
    EPD_Delay_ms(20);
    EPD_RST_PORT->BRR = EPD_RST_PIN;
    EPD_Delay_ms(20);
    EPD_RST_PORT->BSRR = EPD_RST_PIN;
    EPD_Delay_ms(20);
}

void EPD_WaitForBusy()
{
	while (1)
	{	//=1 BUSY
		if ((EPD_BUSY_PORT->IDR & EPD_BUSY_PIN) == 0) 
        {
			break;
        }
		EPD_Delay_ms(20);
	}
	EPD_Delay_ms(20);
}

void EPD_SendData(uint8_t data)
{
    EPD_DC_PORT->BSRR = EPD_DC_PIN;
    EPD_CS_PORT->BRR = EPD_CS_PIN;
    EPD_Transmit(data);
    EPD_CS_PORT->BSRR = EPD_CS_PIN;
}

void EPD_SendCommand(uint8_t cmd)
{
    EPD_DC_PORT->BRR = EPD_DC_PIN;
    EPD_CS_PORT->BRR = EPD_CS_PIN;
    EPD_Transmit(cmd);
    EPD_CS_PORT->BSRR = EPD_CS_PIN;
}
