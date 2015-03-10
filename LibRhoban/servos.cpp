#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "servos.h"
#include "terminal.h"
#include "dxl.h"

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
static volatile servo_t Servos[SERVOS_MAX_NB];
static volatile uint8_t Servos_count = 0;
static volatile bool Servos_enable_smoothing = false;
static volatile double Servos_smooth = 0.0;
HardwareTimer Servos_timer(1);

#define SERVOS_CHANNELS (SERVOS_MAX_NB/SERVOS_PER_CHANNEL)
static volatile int Servos_current;
#define SERVO_FOR_CHANNEL(channel) (channel*SERVOS_PER_CHANNEL+Servos_current)

static void _servos_update(int channel)
{
    int index = SERVO_FOR_CHANNEL(channel);

    if (index < SERVOS_MAX_NB) {
        if (Servos[index].enabled) {
            digitalWrite(Servos[index].pin, LOW);
        }
    }
}

static void _servos_compare1_irq()
{
    _servos_update(0);
}
static void _servos_compare2_irq()
{
    _servos_update(1);
}
static void _servos_compare3_irq()
{
    _servos_update(2);
}
    
static void _servos_ovf()
{
    Servos_current++;
    if (Servos_current >= SERVOS_PER_CHANNEL) {
        Servos_current = 0;
    }

    for (unsigned int channel=0; channel<SERVOS_CHANNELS; channel++) {
        int index = SERVO_FOR_CHANNEL(channel);
        if (Servos[index].enabled) {
            digitalWrite(Servos[index].pin, HIGH);
            Servos_timer.setCompare(TIMER_CH1+channel, Servos[index].pos);
        } else {
            Servos_timer.setCompare(TIMER_CH1+channel, SERVOS_TIMERS_OVERFLOW/2);
        }
    }
}

/**
 * Initialize timer
 * @param i : timer number
 */
void initTimer(uint8_t i)
{
    Servos_timer.pause();
    Servos_timer.setPrescaleFactor(SERVOS_TIMERS_PRESCALE);
    Servos_timer.setOverflow(SERVOS_TIMERS_OVERFLOW);

    Servos_timer.setMode(TIMER_CH1, TIMER_OUTPUT_COMPARE);
    Servos_timer.setMode(TIMER_CH2, TIMER_OUTPUT_COMPARE);
    Servos_timer.setMode(TIMER_CH3, TIMER_OUTPUT_COMPARE);
    Servos_timer.setMode(TIMER_CH4, TIMER_OUTPUT_COMPARE);

    Servos_timer.setCompare(TIMER_CH1, SERVOS_TIMERS_OVERFLOW/2);
    Servos_timer.attachCompare1Interrupt(_servos_compare1_irq);

    Servos_timer.setCompare(TIMER_CH2, SERVOS_TIMERS_OVERFLOW/2);
    Servos_timer.attachCompare2Interrupt(_servos_compare2_irq);
    
    Servos_timer.setCompare(TIMER_CH3, SERVOS_TIMERS_OVERFLOW/2);
    Servos_timer.attachCompare3Interrupt(_servos_compare3_irq);

    Servos_timer.setCompare(TIMER_CH4, 0);
    Servos_timer.attachCompare4Interrupt(_servos_ovf);

    Servos_timer.refresh();
    Servos_timer.resume();
}

static void servos_configure_timer()
{
    initTimer(SERVOS_TIMER);
}

void button_pressed()
{
    servos_disable_all();
    dxl_disable_all();
}

void servos_init()
{
    /**
     * Disabling all servos entries
     */
    for (int i=0; i<SERVOS_MAX_NB; i++) {
        Servos[i].enabled = false;
    }

    /**
     * Initializing current servos for channels to 0
     */
    Servos_current = 0;

    /**
     * Set up hardware timer
     */
    servos_configure_timer();

    /**
     * Set up board led and button
     */
    pinMode(BOARD_BUTTON_PIN, INPUT_PULLDOWN);
    pinMode(BOARD_LED_PIN, OUTPUT);
    attachInterrupt(BOARD_BUTTON_PIN, button_pressed, RISING);
}

uint8_t servos_register(uint8_t pin, char* label)
{
    if (Servos_count < SERVOS_MAX_NB) {
        for (uint8_t i=0;i<Servos_count;i++) {
            if (
                strncmp((char*)Servos[i].label, label, SERVOS_ID_LENGTH) == 0 ||
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
            strncpy((char*)Servos[Servos_count].label, label, SERVOS_ID_LENGTH+1);
        } else {
            Servos[Servos_count].label[0] = '\0';
        }
        pinMode(Servos[Servos_count].pin, OUTPUT);
        digitalWrite(Servos[Servos_count].pin, LOW);
        Servos_count++;

        // Re-configuring timer, because some pins can cause it to fail
        servos_configure_timer();
    
        return Servos_count-1;
    } else {
        return -1;
    }
}

uint8_t servos_index(char* label)
{
    for (uint8_t i=0;i<Servos_count;i++) {
        if (strncmp((char*)Servos[i].label, label, SERVOS_ID_LENGTH) == 0) {
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
    if (index != -1 && index < Servos_count) return (char*)Servos[index].label;
    else return NULL;
}
float servos_get_command(uint8_t index)
{
    if (index != -1 && index < Servos_count) {
        float pos = (Servos[index].pos-Servos[index].init)/Servos[index].steps_per_degree;

        if (servos_is_reversed(index)) {
            pos *= -1;
        }

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
}

void servos_command(uint8_t index, float pos)
{
    if (index == -1 || index >= Servos_count) {
        return;
    }
    
    if (Servos_enable_smoothing) {
        float old_pos = servos_get_command(index);
        if (pos > old_pos + Servos_smooth) {
            pos = old_pos + Servos_smooth;
        }
        if (pos < old_pos - Servos_smooth) {
            pos = old_pos - Servos_smooth;
        }
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
        Servos[index].enabled = false;
        digitalWrite(Servos[index].pin, LOW);
    } else if (enabled == true && Servos[index].enabled == false) {
        servos_set_pos(index, Servos[index].pos);
        Servos[index].enabled = true;
    }
}

void servos_enable_all()
{
    for (int8_t i=0; i<Servos_count; i++) {
        servos_enable(i, true);
        delay(21);
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

TERMINAL_PARAMETER_INT(prescaler," Prescaler", 0);

void servos_attach_interrupt(voidFuncPtr func)
{
    HardwareTimer timer(4);
    timer.pause();
    timer.setPrescaleFactor(SERVOS_TIMERS_PRESCALE*SERVOS_PER_CHANNEL);
    timer.setOverflow(SERVOS_TIMERS_OVERFLOW);
    timer.setMode(TIMER_CH4, TIMER_OUTPUT_COMPARE);
    timer.setCompare(TIMER_CH4, 1);
    timer.attachInterrupt(TIMER_CH4, func);
    timer.refresh();
    timer.resume();
}

void servos_set_smoothing(double smooth)
{
    Servos_enable_smoothing = (smooth > 0);
    Servos_smooth = smooth;
}
