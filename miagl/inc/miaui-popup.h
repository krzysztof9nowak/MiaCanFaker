#ifndef _MIAUI_POPUP_H_
#define _MIAUI_POPUP_H_

#include <miaui.h>

#include <stdint.h>

#define MUI_POPUP_TIME_INFINITE ((uint32_t)0x7FFFFFFF)
#define MUI_POPUP_TIME_DEFAULT 5000

typedef enum {
    MUI_POPUP_RELEASE_PBRAKE = 1,
} mui_popup_type_t;

void mui_PopupUpdate(mui_state_ptr instance, uint32_t delta_ms);
void mui_PopupDraw(mui_state_ptr instance, miagl_ptr gl);
void mui_ShowPopup(mui_state_ptr instance, mui_popup_type_t type, int32_t duration_ms);
void mui_HidePopup(mui_state_ptr instance);

#endif
