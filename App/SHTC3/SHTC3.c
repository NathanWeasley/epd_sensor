#include "SHTC3.h"
#include "core_cm0plus.h"

#define SDA_H       (SHTC3_GPIO_Port->BSRR = SHTC3_SDA_Pin)
#define SDA_L       (SHTC3_GPIO_Port->BRR =  SHTC3_SDA_Pin)
#define SCL_H       (SHTC3_GPIO_Port->BSRR = SHTC3_SCL_Pin)
#define SCL_L       (SHTC3_GPIO_Port->BRR =  SHTC3_SCL_Pin)
#define SDA_READ    (SHTC3_GPIO_Port->IDR & SHTC3_SDA_Pin)
#define SCL_READ    (SHTC3_GPIO_Port->IDR & SHTC3_SCL_Pin)
#define IIC_ACK     0
#define IIC_NACK    1

inline void IIC_Delay(uint8_t us)
{
    __NOP(); __NOP(); __NOP(); __NOP();
}

void IIC_Init()
{
    /** GPIO Init */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    /** Configure GPIO */
    GPIO_InitStruct.Pin = SHTC3_SCL_Pin | SHTC3_SDA_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(SHTC3_GPIO_Port, &GPIO_InitStruct);

    /** IIC signal is idle-high */
    SDA_H;
    SCL_H;
}

inline void IIC_Start()
{
    SDA_H;
    IIC_Delay(1);
    SCL_H;
    IIC_Delay(1);

    /**
     * IIC start is defined by a
     * falling edge on SDA while SCL is high
     */
    SDA_L;
    IIC_Delay(10);
    SCL_L;
    IIC_Delay(10);
}

inline void IIC_Stop()
{
    SDA_L;
    IIC_Delay(1);
    SCL_L;
    IIC_Delay(1);

    /**
     * IIC stop is defined by a
     * rising edge on SDA while SCL is high
     */
    SCL_H;
    IIC_Delay(10);
    SDA_H;
    IIC_Delay(10);
}

uint8_t IIC_SendByte(uint8_t byte)
{
    uint8_t mask, ack;

    /** Assumint IIC start signal is sent */
    for (mask = 0x80; mask > 0; mask >>= 1)
    {
        if (mask & byte)
        {
            SDA_H;
        }
        else
        {
            SDA_L;
        }
        IIC_Delay(1);
        SCL_H;
        IIC_Delay(5);
        SCL_L;
        IIC_Delay(1);
    }

    /** Releasae SDA for ack reading */
    SDA_H;
    SCL_H;
    IIC_Delay(1);
    ack = SDA_READ;
    SCL_L;

    return ack;
}

uint8_t IIC_ReadByte(uint8_t ack)
{
    uint8_t mask, buf = 0;

    /** Release SDA */
    SDA_H;

    for (mask = 0x80; mask > 0; mask >>= 1)
    {
        SCL_H;
        IIC_Delay(1);

        /** Wait for clock stretching */
        while (SCL_READ == 0);
        IIC_Delay(3);

        if (SDA_READ)
        {
            buf |= mask;
        }

        SCL_L;
        IIC_Delay(1);
    }

    /** Send ack */
    if (ack == IIC_ACK)
    {
        SDA_L;
    }
    else
    {
        SDA_H;
    }
    IIC_Delay(1);

    SCL_H;
    IIC_Delay(5);
    SCL_L;

    SDA_H;

    return buf;
}
















void SHTC3_Init()
{
    IIC_Init();
}
