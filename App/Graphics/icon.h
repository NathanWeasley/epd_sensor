#pragma once

#include <stdint.h>

#define ICON_MAX_HEIGHT     (64)
#define ICON_MAX_WIDTH      (6)

#define ICON_CNT            (5)
#define ICON_DEGC           (0)
#define ICON_PRH            (1)
#define ICON_NOBAT          (2)
#define ICON_CHRG           (3)

typedef struct
{
  uint8_t width;
  uint8_t height;
  const uint8_t bitmap[ICON_MAX_HEIGHT*ICON_MAX_WIDTH];
} icon_t;

extern const icon_t icon_table[ICON_CNT];
