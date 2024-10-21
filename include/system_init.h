#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include "../include/system_types.h"

void System_Init(void);
void PWM_Init(void);
void RTC_Init(void);
void Timer_Init(void);
void WDT_Init(void);
void Enter_Low_Power_Mode(void);

#endif