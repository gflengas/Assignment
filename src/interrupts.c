#include <stdio.h>
#include "../include/interrupts.h"
#include "../include/system_types.h"
#include "../include/gpio_config.h"
#include "../include/led_control.h"

extern volatile SystemStatus g_system_status;

/**
 * @brief Real-Time Clock Interrupt Service Routine
 * 
 * @details Prints the current system status to console, including:
 *          - Button state (Pressed/Released)
 *          - LED state (Off/Static Red/Fading RGB)
 *          - Vibrator state (On/Off)
 */
void RTC_ISR(void) {
    printf("=== System Status ===\n");
    printf("Button: %s\n", g_system_status.button_pressed ? "Pressed" : "Released");
    printf("LED State: ");
    switch(g_system_status.led_state) {
        case LED_OFF:
            printf("Off\n");
            break;
        case LED_RED_STATIC:
            printf("Static Red\n");
            break;
        case LED_RGB_FADING:
            printf("Fading RGB\n");
            break;
    }
    printf("Vibrator: %s\n", g_system_status.vibrator_active ? "On" : "Off");
    printf("==================\n");
}

/**
 * @brief Timer Interrupt Service Routine
 * 
 * @details Handles vibrator deactivation:
 *          - Disables vibrator output
 *          - Updates system status
 *          - Stops and clears the timer
 * 
 * @note Only executes when vibrator is active
 */
void TIMER_ISR(void) {
    if (g_system_status.vibrator_active) {
        GPIO_Set(VIBRATOR_PIN, 0);
        g_system_status.vibrator_active = false;
        Timer_Stop();
        Timer_Clear_Interrupt();
    }
}

/**
 * @brief Button Interrupt Service Routine
 * 
 * @details Handles button press/release events with debouncing:
 *          - Implements 50ms debounce protection
 *          - On press: Records press time and updates status
 *          - On release: 
 *            * Long press (>=3s): Activates vibrator
 *            * Short press: Cycles LED states (Off->Red->RGB->Off)
 */
void BUTTON_ISR(void) {
    static uint32_t last_interrupt_time = 0;
    uint32_t current_time = Get_Timestamp();
    
    // Debouncing
    if ((current_time - last_interrupt_time) < 50) {
        return;
    }
    last_interrupt_time = current_time;

    // Button Press
    if (GPIO_Read(BUTTON_PIN) == 0) {  
        g_system_status.button_pressed = true;
        g_system_status.button_press_time = current_time;
    }
    // Button Release 
    else {  
        uint32_t press_duration = current_time - g_system_status.button_press_time;
        // Long press (3 seconds) - Activate vibrator
        if (press_duration >= 3000) {
            GPIO_Set(VIBRATOR_PIN, 1);
            g_system_status.vibrator_active = true;
            // Start 2-second timer for vibrator
            Timer_Start();
        }
        // Short press - Change LED state
        else {
            switch (g_system_status.led_state) {
                case LED_OFF:
                    LED_SetState(LED_RED_STATIC);
                    break;
                case LED_RED_STATIC:
                    LED_SetState(LED_RGB_FADING);
                    break;
                case LED_RGB_FADING:
                    LED_SetState(LED_OFF);
                    break;
            }
        }
        g_system_status.button_pressed = false;
    }

    GPIO_Clear_Interrupt(BUTTON_PIN);
}