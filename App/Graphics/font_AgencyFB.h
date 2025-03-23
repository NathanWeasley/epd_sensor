#pragma once

#include <stdint.h>

/**
 * Agency FB font characters
 *
 * 64px high, and variable width.
 */

#define AFB_HEIGHT    (64)
#define AFB_WIDTH_MAX (5)

#define AFB_CHAR_CNT  (12)

#define AFB_DOT_PTR   (10)
#define AFB_DASH_PTR  (11)

typedef struct
{
  uint8_t width;
  const uint8_t bitmap[AFB_HEIGHT*AFB_WIDTH_MAX];
} afb_char_t;

extern const afb_char_t afb_table[AFB_CHAR_CNT];
