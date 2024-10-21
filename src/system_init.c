#include "../include/system_init.h"
#include "../include/gpio_config.h"
#include "../include/led_control.h"

/**
* @brief Initializes all system components and peripherals
* 
* @details Performs initialization sequence for:
*          - GPIO configuration for button and vibrator
*          - Button interrupt on falling edge
*          - PWM channels for RGB LED control
*          - Real-Time Clock (RTC)
*          - General purpose timer
*          - Watchdog timer (WDT)
*/
void System_Init(void) {
    GPIO_Init(BUTTON_PIN, GPIO_MODE_INPUT);
    GPIO_Enable_Interrupt(BUTTON_PIN, FALLING_EDGE);
    GPIO_Init(VIBRATOR_PIN, GPIO_MODE_OUTPUT);
    PWM_Init();
    RTC_Init();
    Timer_Init();
    WDT_Init();
}

/**
* @brief Initializes PWM channels for RGB LED control
* 
* @details Sets up PWM channels for:
*          - Red LED pin
*          - Green LED pin  
*          - Blue LED pin
* @note Sets initial LED state to OFF
*/
void PWM_Init(void) {
    PWM_Channel_Init(RED_LED_PIN);
    PWM_Channel_Init(GREEN_LED_PIN);
    PWM_Channel_Init(BLUE_LED_PIN);
    LED_SetState(LED_OFF);
}

/**
* @brief Initializes Real-Time Clock
* 
* @details Configures RTC with:
*          - 2000ms interval period
*          - Interrupt enabled
*/
void RTC_Init(void) {
    RTC_Set_Interval(2000);
    RTC_Enable_Interrupt();
}

/**
* @brief Initializes general purpose timer
* 
* @details Configures timer with:
*          - 2000ms period
*          - Interrupt enabled
*/
void Timer_Init(void) {
    Timer_Set_Period(2000);
    Timer_Enable_Interrupt();
}

/**
* @brief Initializes Watchdog Timer
* 
* @details Configures WDT with:
*          - 15000ms timeout period
*          - WDT enabled
*/
void WDT_Init(void) {
    WDT_Set_Timeout(15000);
    WDT_Enable();
}

/**
* @brief Enters low power mode
* 
* @details Performs power-down sequence:
*          - Configures MCU for deep sleep
*          - Executes Wait For Interrupt (WFI) instruction
* 
* @note System will wake on enabled interrupts
*/
void Enter_Low_Power_Mode(void) {
    set_mcu_deep_sleep();
    __WFI();
}