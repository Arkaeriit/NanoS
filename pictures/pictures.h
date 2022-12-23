#ifndef PICTURES_H
#define PICTURES_H

#include "stdio.h"
#include "stdint.h"

uint8_t* picture_get_bg(uint32_t width, uint32_t height);
unsigned int picture_get_fg(void);
FILE* picture_get_nano_theme(void);

#endif

