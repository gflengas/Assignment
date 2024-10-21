#include <Arduino.h>

// Pin definitions - Update these according to your ESP32 wiring
const int BUTTON_PIN = 35;     // GPIO4
const int RED_LED_PIN = 14;   // GPIO16
const int GREEN_LED_PIN = 12; // GPIO17
const int BLUE_LED_PIN = 13;  // GPIO18
const int VIBRATOR_PIN = 32;  // GPIO19

// PWM configuration
const int PWM_FREQUENCY = 5000;    // 5 kHz
const int PWM_RESOLUTION = 8;      // 8-bit resolution
const int PWM_RED_CHANNEL = 0;     // PWM channel for red LED
const int PWM_GREEN_CHANNEL = 1;   // PWM channel for green LED
const int PWM_BLUE_CHANNEL = 2;    // PWM channel for blue LED

// LED states
enum LED_State {
    LED_OFF,
    LED_RED_STATIC,
    LED_RGB_FADING
};

// Status structure
struct SystemStatus {
    bool button_pressed;
    LED_State led_state;
    bool vibrator_active;
    unsigned long button_press_time;
    bool long_press_handled;
};

// RGB Color structure
struct RGB_Color {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

// Global variables
volatile SystemStatus g_system_status = {false, LED_OFF, false, 0, false};
volatile uint32_t fade_counter = 0;
const int FADE_STEPS = 255;

// Function declarations
void LED_SetColor(RGB_Color color);
void LED_Off();
void LED_StaticRed();
void LED_UpdateFading();
void LED_SetState(LED_State new_state);

void IRAM_ATTR buttonISR() {
    g_system_status.button_pressed = !digitalRead(BUTTON_PIN);
    if (g_system_status.button_pressed) {
        g_system_status.button_press_time = millis();
    }
}

void setup() {
    Serial.begin(115200);
    
    // Initialize button with internal pullup
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, CHANGE);
    
    // Initialize vibrator pin
    pinMode(VIBRATOR_PIN, OUTPUT);
    
    // Configure LED PWM
    ledcSetup(PWM_RED_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_GREEN_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcSetup(PWM_BLUE_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
    
    // Attach LED pins to PWM channels
    ledcAttachPin(RED_LED_PIN, PWM_RED_CHANNEL);
    ledcAttachPin(GREEN_LED_PIN, PWM_GREEN_CHANNEL);
    ledcAttachPin(BLUE_LED_PIN, PWM_BLUE_CHANNEL);
    
    // Start with LEDs off
    LED_SetState(LED_OFF);
}

void loop() {
    static unsigned long last_fade_update = 0;
    static unsigned long last_status_print = 0;
    static unsigned long vibrator_start_time = 0;
    const unsigned long FADE_UPDATE_INTERVAL = 10;    // 10ms between fade updates
    const unsigned long STATUS_PRINT_INTERVAL = 2000; // 2s between status prints
    
    unsigned long current_time = millis();
    
    // Handle button press/release
    if (g_system_status.button_pressed && !g_system_status.long_press_handled) {
        if ((current_time - g_system_status.button_press_time) >= 3000) {
            // Long press detected - activate vibrator
            digitalWrite(VIBRATOR_PIN, HIGH);
            g_system_status.vibrator_active = true;
            vibrator_start_time = current_time;
            g_system_status.long_press_handled = true;
        }
    }
    else if (!g_system_status.button_pressed && g_system_status.button_press_time > 0) {
        // Button released
        unsigned long press_duration = current_time - g_system_status.button_press_time;
        
        if (!g_system_status.long_press_handled && press_duration < 3000) {
            // Short press - cycle LED state
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
        g_system_status.button_press_time = 0;
        g_system_status.long_press_handled = false;
    }
    
    // Update LED fading if active
    if (g_system_status.led_state == LED_RGB_FADING &&
        (current_time - last_fade_update) >= FADE_UPDATE_INTERVAL) {
        LED_UpdateFading();
        last_fade_update = current_time;
    }
    
    // Handle vibrator timeout
    if (g_system_status.vibrator_active &&
        (current_time - vibrator_start_time) >= 2000) {
        digitalWrite(VIBRATOR_PIN, LOW);
        g_system_status.vibrator_active = false;
    }
    
    // Print status every 2 seconds
    if (current_time - last_status_print >= STATUS_PRINT_INTERVAL) {
        Serial.println("=== System Status ===");
        Serial.print("Button: ");
        Serial.println(g_system_status.button_pressed ? "Pressed" : "Released");
        Serial.print("LED State: ");
        switch(g_system_status.led_state) {
            case LED_OFF:
                Serial.println("Off");
                break;
            case LED_RED_STATIC:
                Serial.println("Static Red");
                break;
            case LED_RGB_FADING:
                Serial.println("Fading RGB");
                break;
        }
        Serial.print("Vibrator: ");
        Serial.println(g_system_status.vibrator_active ? "On" : "Off");
        Serial.println("==================");
        
        last_status_print = current_time;
    }
}

void LED_SetColor(RGB_Color color) {
    ledcWrite(PWM_RED_CHANNEL, color.red);
    ledcWrite(PWM_GREEN_CHANNEL, color.green);
    ledcWrite(PWM_BLUE_CHANNEL, color.blue);
}

void LED_Off() {
    RGB_Color color = {0, 0, 0};
    LED_SetColor(color);
    g_system_status.led_state = LED_OFF;
}

void LED_StaticRed() {
    RGB_Color color = {255, 0, 0};
    LED_SetColor(color);
    g_system_status.led_state = LED_RED_STATIC;
}

void LED_UpdateFading() {
    uint32_t position = fade_counter % (3 * FADE_STEPS);
    RGB_Color color = {0, 0, 0};
    
    if (position < FADE_STEPS) {
        // Red to Green
        color.red = FADE_STEPS - position;
        color.green = position;
    }
    else if (position < 2 * FADE_STEPS) {
        // Green to Blue
        position -= FADE_STEPS;
        color.green = FADE_STEPS - position;
        color.blue = position;
    }
    else {
        // Blue to Red
        position -= 2 * FADE_STEPS;
        color.blue = FADE_STEPS - position;
        color.red = position;
    }
    
    LED_SetColor(color);
    fade_counter++;
}

void LED_SetState(LED_State new_state) {
    switch (new_state) {
        case LED_OFF:
            LED_Off();
            break;
        case LED_RED_STATIC:
            LED_StaticRed();
            break;
        case LED_RGB_FADING:
            fade_counter = 0;  // Reset fading position
            g_system_status.led_state = LED_RGB_FADING;
            break;
    }
}