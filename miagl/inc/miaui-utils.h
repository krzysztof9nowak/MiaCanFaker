#ifndef _MIAUI_UTILS_H_
#define _MIAUI_UTILS_H_

#include <stdint.h>

void mui_UFixedPointToStr(char* out, uint32_t value, uint8_t dec_places);
void mui_FixedPointToStr(char* out, int32_t value, uint8_t dec_places);

#endif
