#include "Task/task.h"
#include "Graphics/GUI_Paint.h"
#include "stm32l0xx_ll_utils.h"

void task_sensor()
{
    EPD_Init();
    EPD_Clear();

    LL_mDelay(500);

    
}
