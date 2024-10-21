/**
 * @file system_types.h
 * @brief System-wide type definitions and data structures
 * @details Contains all common types used throughout the embedded application for 
 *          LED control, button handling, and system status tracking
 */

#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief LED operation states
 * 
 * @details Defines possible states for the LED subsystem:
 *          - LED_OFF: All LEDs turned off
 *          - LED_RED_STATIC: Red LED on at full brightness
 *          - LED_RGB_FADING: Continuous RGB color fading animation
 */
typedef enum {
    LED_OFF,         
    LED_RED_STATIC, 
    LED_RGB_FADING   
} LED_State;

/**
 * @brief System status and state tracking
 * 
 * @details Maintains current state of system components including button state,
 *          LED mode, vibrator status, and timing information for debouncing and
 *          long press detection
 */
typedef struct {
    bool button_pressed;        
    LED_State led_state;        
    bool vibrator_active;
    // Additional fields for interrupt handling    
    uint32_t timestamp;         
    uint32_t button_press_time; 
    bool long_press_handled;    
} SystemStatus;

/**
 * @brief RGB color representation
 * 
 * @details Holds 8-bit values for each RGB color channel. Used for setting LED
 *          colors via PWM duty cycle control
 */
typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGB_Color;

#endif 