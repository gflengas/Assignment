# RGB LED and Button Control System

This repository contains an embedded system project that controls an RGB LED, a button, and a vibrator motor. The system is designed to handle button presses, manage the RGB LED's state (off, static red, or fading RGB), and activate a vibration motor based on input from the user.

## Project Structure
```
├── main.c
├── test.ino
├── include/
│   ├── system_types.h
│   ├── gpio_config.h
│   ├── system_init.h
│   ├── led_control.h
│   └── interrupts.h
└── src/
    ├── system_init.c
    ├── led_control.c
    └── interrupts.c
```


### Features
- **Button Handling**: Detects short and long button presses, cycling through LED states or activating the vibrator motor.
- **LED Control**: Manages different LED states:
  - `LED_OFF`: Turns off the LEDs.
  - `LED_RED_STATIC`: Lights up the red LED.
  - `LED_RGB_FADING`: Smooth color transitions (RGB fading effect).
- **Vibrator Activation**: Triggered by a long button press and runs for 2 seconds.
- **Power Management**: Enters low-power mode when the system is idle (LED off and vibrator inactive).
- **Watchdog Timer**: Ensures the system stays responsive by refreshing the watchdog every 10 seconds.

### File Descriptions

- **main.c**: The main loop handling LED updates, watchdog kicking, and transitioning the system to low-power mode.
- **test.ino**: Ports the project to ESP32 using the Arduino IDE to validate the logic and functionality in a different environment.
- **include/system_types.h**: Defines structures and enumerations such as `LED_State` and `SystemStatus`.
- **include/gpio_config.h**: Defines GPIO pin assignments and configuration methods.
- **include/system_init.h**: Declares functions for initializing peripherals like GPIO, PWM, RTC, and timers.
- **include/led_control.h**: Declares functions for controlling the RGB LED and managing different states.
- **include/interrupts.h**: Declares the interrupt service routines for the button, timers, and RTC.
- **src/system_init.c**: Implements the system initialization functions.
- **src/led_control.c**: Implements LED control functions, including color transitions for RGB fading.
- **src/interrupts.c**: Implements the button, timer, and RTC interrupt handlers.

### How It Works
- A button press cycles through different LED states or activates the vibrator motor.
- Short press: Cycles between LED off, static red, and RGB fading.
- Long press: Activates the vibrator for 2 seconds.
- The system uses timers and interrupts to ensure efficient operation and enters low-power mode when idle.

### Testing and Validation
- The `test.ino` file serves as a testbed for porting the functionality of this project to the ESP32 platform, confirming that the logic applied in the desing works correctly with at least one platform.