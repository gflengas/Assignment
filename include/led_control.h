#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include "../include/system_types.h"

void LED_SetColor(RGB_Color color);
void LED_Off(void);
void LED_StaticRed(void);
void LED_UpdateFading(void);
void LED_SetState(LED_State new_state);

#endif