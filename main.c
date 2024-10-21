#include <stdio.h>
#include "../include/system_types.h"
#include "../include/gpio_config.h"
#include "../include/system_init.h"
#include "../include/led_control.h"
#include "../include/interrupts.h"

/** @brief Global system status structure */
volatile SystemStatus g_system_status = {0};

/** @brief Counter for LED fading effect progression */
volatile uint32_t fade_counter = 0;

/**
* @brief Main program entry point
* 
* @return int Returns 0 on normal execution (never reached in embedded loop)
* 
* @details Main control loop performs:
*          - System initialization
*          - LED fading updates every 10ms when in RGB_FADING state
*          - Watchdog kicks every 10 seconds
*          - Low power mode entry when system is idle
* 
* @note System is considered idle when:
*       - LED is OFF
*       - Vibrator is inactive
* 
* @note Timing constants:
*       - FADE_UPDATE_INTERVAL: 10ms between LED updates
*       - WDT_REFRESH_INTERVAL: 10000ms between watchdog kicks
* 
* @warning Main loop includes a 1ms delay for system timing stability
*/
int main(void) {
    System_Init();
    
    uint32_t last_wdt_kick = 0;
    uint32_t last_fade_update = 0;
    const uint32_t FADE_UPDATE_INTERVAL = 10;
    const uint32_t WDT_REFRESH_INTERVAL = 10000;
    
    while (1) {
        uint32_t current_time = Get_Timestamp();
        
        // Update LED fading if active
        if (g_system_status.led_state == LED_RGB_FADING &&
            (current_time - last_fade_update) >= FADE_UPDATE_INTERVAL) {
            LED_UpdateFading();
            last_fade_update = current_time;
        }
        
        // Kick watchdog every 10 seconds
        if ((current_time - last_wdt_kick) >= WDT_REFRESH_INTERVAL) {
            WDT_Kick();
            last_wdt_kick = current_time;
        }
        
        // Enter low-power mode if system is idl
        if (g_system_status.led_state == LED_OFF && !g_system_status.vibrator_active) {
            Enter_Low_Power_Mode();
        }

        Delay_ms(1);
    }
    
    return 0;
}