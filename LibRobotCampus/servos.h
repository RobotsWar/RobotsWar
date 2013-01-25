#ifndef _SERVOS_H
#define _SERVOS_H

#include <wirish/wirish.h>

/**
 * Max length of servos string label
 */
#define SERVOS_ID_LENGTH 10

/**
 * Max number of servos
 */
#define SERVOS_MAX_NB 12

/**
 * Timers parameters for output
 * PWM generation
 */
#define SERVOS_TIMERS_PRESCALE 24
#define SERVOS_TIMERS_OVERFLOW 60000

/**
 * Initializes timers for servos PWM
 * This function have to be called before
 * enabling any servos
 */
void servos_init();

/**
 * Create a new servo
 * @param pin : output PWM pin number
 * @param label : a non-NULL name for this servo
 * @return the servo index or -1 if error
 */
uint8_t servos_register(uint8_t pin, char* label);

/**
 * Retrieve the index
 * @param label : string label
 * @return the index of the first servo
 * with given label or -1 if not found
 */
uint8_t servos_index(char* label);

/**
 * Servo count
 * @return the number of registered servo
 */
uint8_t servos_count();

/**
 * Servo information
 * pin, min, zero, max, pos, reversed, enabled
 * @param index : servo index
 */
uint8_t servos_get_pin(uint8_t index);
uint16_t servos_get_min(uint8_t index);
uint16_t servos_get_zero(uint8_t index);
uint16_t servos_get_max(uint8_t index);
uint16_t servos_get_pos(uint8_t index);
bool servos_is_reversed(uint8_t index);
bool servos_is_enabled(uint8_t index);
char* servos_get_label(uint8_t index);

/**
 * Calibrate the servo
 * @param index
 * @param min : minimum position in timer value
 * @param zero : default zero position in timer value
 * @param max : maximum position in timer value
 * @param reversed : reverse direction if true
 */
void servos_calibrate(uint8_t index, 
    uint16_t min, uint16_t zero, uint16_t max, bool reversed = false);

/**
 * Set servo position
 * @param index
 * @param pos : timer value
 */
void servos_set_pos(uint8_t index, uint16_t pos);

/**
 * Set servo at position zero
 * @param index
 */
void servos_reset(uint8_t index);

/**
 * Enable servo
 * @param index
 * @param enabled : true for enabing servo
 */
void servos_enable(uint8_t index, bool enabled = true);

/**
 * Stop all servos
 */
void servos_emergency();

#endif

