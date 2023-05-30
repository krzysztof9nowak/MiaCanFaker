#ifndef _MIAUI_H_
#define _MIAUI_H_

#include <miagl.h>

#include <stdint.h>

typedef struct mui_state {

    const char* debug_text; //!< any debug text to draw, may be NULL
    uint16_t capacitor_voltage; //!< in 0.1v, e.g. 762 = 76.2 V
    uint8_t cell_count; //!< capacitor_voltage / cell_count = cell voltage
    int16_t motor_current; //!< motor current in Amps (signed)
    
    struct {
        uint32_t elapsed_time_ms;
    } __internal;

} mui_state_t, *mui_state_ptr;

void mui_InitInternalState(mui_state_ptr instance);
void mui_Update(mui_state_ptr instance, uint32_t delta_ms);
void mui_Draw(mui_state_ptr instance, miagl_ptr gl);

#endif
