#include "EPD/epd_driver.h"

#define EPD_BUSY_PIN    LL_GPIO_PIN_0
#define EPD_RST_PIN     LL_GPIO_PIN_3
#define EPD_DC_PIN      LL_GPIO_PIN_6
#define EPD_CS_PIN      LL_GPIO_PIN_4
#define EPD_CLK_PIN     LL_GPIO_PIN_5
#define EPD_DIN_PIN     LL_GPIO_PIN_7

#define IMG_SIZE (((EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT)

static uint8_t ImageBuf[IMG_SIZE];

void SPI_Transmit(uint8_t byte)
{
    ;
}

void EPD_delayms(uint16_t ms)
{
    LL_mDelay(ms);
}

void EPD_Reset()
{
    GPIOA->BSRR = EPD_RST_PIN;
    EPD_delayms(20);
    GPIOA->BRR = EPD_RST_PIN;
    EPD_delayms(20);
    GPIOA->BSRR = EPD_RST_PIN;
    EPD_delayms(20);
}

void EPD_WaitForBusy()
{
	while (1)
	{	//=1 BUSY
		if (GPIOB->IDR & EPD_BUSY_PIN == 0) 
        {
			break;
        }
		EPD_delayms(20);
	}
	EPD_delayms(20);
}
