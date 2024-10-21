#include "../include/led_control.h"
#include "../include/gpio_config.h"
#include "../include/system_types.h"

extern volatile SystemStatus g_system_status;
extern volatile uint32_t fade_counter;
#define FADE_STEPS 255

/**
* @brief Sets RGB LED color using PWM channels
* 
* @param color RGB color structure containing intensity values for each channel
* 
* @details Updates PWM duty cycle for:
*          - Red channel   (0-255)
*          - Green channel (0-255)
*          - Blue channel  (0-255)
*/
void LED_SetColor(RGB_Color color) {
    PWM_Set_Duty(RED_LED_PIN, color.red);
    PWM_Set_Duty(GREEN_LED_PIN, color.green);
    PWM_Set_Duty(BLUE_LED_PIN, color.blue);
}

/**
* @brief Turns off RGB LED
* 
* @details Sets all RGB channels to 0 and updates system LED state
*/
void LED_Off(void) {
    RGB_Color color = {0, 0, 0};
    LED_SetColor(color);
    g_system_status.led_state = LED_OFF;
}

/**
* @brief Sets LED to static red color
* 
* @details Activates only red channel at full intensity and updates system state
*/
void LED_StaticRed(void) {
    RGB_Color color = {255, 0, 0};
    LED_SetColor(color);
    g_system_status.led_state = LED_RED_STATIC;
}

/**
* @brief Updates LED color for fading effect
* 
* @details Implements RGB color fading sequence:
*          - Red -> Green transition
*          - Green -> Blue transition
*          - Blue -> Red transition
* 
* @note Uses global fade_counter to track position in fade sequence
* @note FADE_STEPS defines the resolution of color transitions (255 steps)
*/
void LED_UpdateFading(void) {
    
    /*Position determines which transition phase we're in:
     *   0 to FADE_STEPS-1:         Red->Green transition
     *   FADE_STEPS to 2*FADE_STEPS-1:   Green->Blue transition
     *   2*FADE_STEPS to 3*FADE_STEPS-1: Blue->Red transition
     * Using modulo ensures continuous cycling through all phases.
     */
    uint32_t position = fade_counter % (3 * FADE_STEPS);
    RGB_Color color = {0, 0, 0};
    
    if (position < FADE_STEPS) {
        color.red = FADE_STEPS - position;
        color.green = position;
    }
    else if (position < 2 * FADE_STEPS) {
        position -= FADE_STEPS;
        color.green = FADE_STEPS - position;
        color.blue = position;
    }
    else {
        position -= 2 * FADE_STEPS;
        color.blue = FADE_STEPS - position;
        color.red = position;
    }
    
    LED_SetColor(color);
    fade_counter++;
}

/**
* @brief Sets LED to specified operational state
* 
* @param new_state Desired LED state (OFF, RED_STATIC, or RGB_FADING)
* 
* @details Handles state transitions:
*          - OFF: Turns off all channels
*          - RED_STATIC: Sets to solid red
*          - RGB_FADING: Initiates color fading sequence
*/
void LED_SetState(LED_State new_state) {
    switch (new_state) {
        case LED_OFF:
            LED_Off();
            break;
        case LED_RED_STATIC:
            LED_StaticRed();
            break;
        case LED_RGB_FADING:
            // Reset fading position
            fade_counter = 0;
            g_system_status.led_state = LED_RGB_FADING;
            break;
    }
}