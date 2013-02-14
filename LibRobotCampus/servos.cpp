#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "servos.h"
#include "terminal.h"

/**
 * Servo structure
 * All position are stored as timer values
 */
struct servo_t
{
    uint8_t pin; //PWM output pin number
    uint16_t min; //Position min
    uint16_t max; //Positin max
    uint16_t init; //Position origin
    uint16_t pos; //Current position
    bool reversed; //True if direction are reversed (-1 gain)
    bool enabled; //True if servo are driven
    char label[SERVOS_ID_LENGTH+1]; //String shortcut
    double steps_per_degree;
};

/**
 * Global servos container
 */
static servo_t Servos[SERVOS_MAX_NB];
static uint8_t Servos_count = 0;

/**
 * Initialize timer
 * @param i : timer number
 */
void initTimer(uint8_t i)
{
    HardwareTimer timer(i);
    timer.pause();
    timer.setPrescaleFactor(SERVOS_TIMERS_PRESCALE);
    timer.setOverflow(SERVOS_TIMERS_OVERFLOW);
    timer.refresh();
    timer.resume();
}

void button_pressed()
{
    servos_disable_all();
}

void servos_init()
{
    /**
     * Set up hardware timer
     */
    initTimer(1);
    initTimer(2);
    initTimer(3);
    initTimer(4);

    /**
     * Set up board led and button
     */
    pinMode(BOARD_BUTTON_PIN, INPUT);
    pinMode(BOARD_LED_PIN, OUTPUT);
    attachInterrupt(BOARD_BUTTON_PIN, button_pressed, RISING);
}

uint8_t servos_register(uint8_t pin, char* label)
{
    if (Servos_count < SERVOS_MAX_NB) {
        for (uint8_t i=0;i<Servos_count;i++) {
            if (
                strncmp(Servos[i].label, label, SERVOS_ID_LENGTH) == 0 ||
                Servos[i].pin == pin
            ) {
                return -1;
            }
        }

        Servos[Servos_count].pin = pin;
        Servos[Servos_count].min = 0;
        Servos[Servos_count].max = SERVOS_TIMERS_OVERFLOW;
        Servos[Servos_count].init = SERVOS_TIMERS_OVERFLOW/20;
        Servos[Servos_count].pos = Servos[Servos_count].init;
        Servos[Servos_count].reversed = false;
        Servos[Servos_count].enabled = false;
        Servos[Servos_count].steps_per_degree = DEFAULT_STEPS_PER_DEGREE;
        if (label != NULL) {
            strncpy(Servos[Servos_count].label, label, SERVOS_ID_LENGTH+1);
        } else {
            Servos[Servos_count].label[0] = '\0';
        }
        Servos_count++;
    
        return Servos_count-1;
    } else {
        return -1;
    }
}

uint8_t servos_index(char* label)
{
    for (uint8_t i=0;i<Servos_count;i++) {
        if (strncmp(Servos[i].label, label, SERVOS_ID_LENGTH) == 0) {
            return i;
        }
    }

    return -1;
}

uint8_t servos_count()
{
    return Servos_count;
}

uint8_t servos_get_pin(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].pin;
    else return -1;
}
uint16_t servos_get_min(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].min;
    else return -1;
}
uint16_t servos_get_init(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].init;
    else return -1;
}
uint16_t servos_get_max(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].max;
    else return -1;
}
uint16_t servos_get_pos(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].pos;
    else return -1;
}
bool servos_is_reversed(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].reversed;
    else return false;
}
bool servos_is_enabled(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].enabled;
    else return false;
}
char* servos_get_label(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].label;
    else return NULL;
}
float servos_get_command(uint8_t index)
{
    if (index != -1 && index < Servos_count) {
        float pos = (Servos[index].pos-Servos[index].init)/Servos[index].steps_per_degree;

        return pos;
    } else {
        return 0.0;
    }
}

uint8_t servos_calibrate(uint8_t index,
    uint16_t min, uint16_t init, uint16_t max, bool reversed)
{
    if (
        index == -1 || index >= Servos_count || max <= min || 
        init < min || init > max
    ) {
        return 1;
    }

    Servos[index].min = min;
    Servos[index].max = max;
    Servos[index].init = init;
    Servos[index].pos = init;
    Servos[index].reversed = reversed;
    servos_set_pos(index, Servos[index].pos);

    return 0;
}

void servos_set_pos(uint8_t index, uint16_t pos)
{
    if (index == -1 || index >= Servos_count) {
        return;
    }
    if (pos < Servos[index].min) {
        pos = Servos[index].min;
    }
    if (pos > Servos[index].max) {
        pos = Servos[index].max;
    }
    
    Servos[index].pos = pos;
    pwmWrite(Servos[index].pin, Servos[index].pos);
}

void servos_command(uint8_t index, float pos)
{
    if (index == -1 || index >= Servos_count) {
        return;
    }

    if (Servos[index].reversed) {
        pos *= -1;
    }

    uint16_t p = (int)(Servos[index].init + pos*Servos[index].steps_per_degree);

    servos_set_pos(index, p);
}

void servos_reset(uint8_t index)
{
    if (index == -1 || index >= Servos_count) {
        return;
    }
    servos_set_pos(index, Servos[index].init);
}

void servos_enable(uint8_t index, bool enabled)
{
    if (index == -1 || index >= Servos_count) {
        return;
    }
    if (enabled == false && Servos[index].enabled == true) {
        pinMode(Servos[index].pin, INPUT_FLOATING);
        Servos[index].enabled = false;
    } else if (enabled == true && Servos[index].enabled == false) {
        servos_set_pos(index, Servos[index].pos);
        pinMode(Servos[index].pin, PWM);
        Servos[index].enabled = true;
    }
}

void servos_enable_all()
{
    for (int8_t i=0; i<Servos_count; i++) {
        servos_enable(i, true);
    }
    
    digitalWrite(BOARD_LED_PIN, HIGH);
}

void servos_disable_all()
{
    for (int8_t i=0; i<Servos_count; i++) {
        servos_enable(i, false);
    }

    digitalWrite(BOARD_LED_PIN, LOW);
}

void servos_flush()
{
    for (int8_t i=0; i<Servos_count; i++) {
        servos_enable(i, false);
    }
    Servos_count = 0;
}

void servos_attach_interrupt(voidFuncPtr func)
{
    HardwareTimer timer(4);
    timer.setChannelMode(4, TIMER_OUTPUT_COMPARE);
    timer.setCompare(4, 3*SERVOS_TIMERS_OVERFLOW/10);
    timer.attachInterrupt(4, func);
    timer.refresh();
    timer.resume();
}

