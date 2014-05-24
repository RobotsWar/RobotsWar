#include <cstdlib>
#include <wirish/wirish.h>
#include <servos.h>
#include <terminal.h>
#include <main.h>
#include <dxl.h>
#include "rc.h"

volatile bool flag = false;
volatile bool isUSB = false;

/**
 * 50hz interrupt
 */
static void setFlag()
{
    flag = true;
}

/**
 * Setting up the board
 */
static void internal_setup()
{
    // Initializing servos
    servos_init();
   
    // Initializing RC
    RC.begin(921600);
    terminal_init(&RC);

    // Configuring board LED
    pinMode(BOARD_LED_PIN, OUTPUT);
    digitalWrite(BOARD_LED_PIN, LOW);

    // Runing user setup
    setup();

#if defined(DXL_AVAILABLE)
    // Enabling asychronous dynamixel
    dxl_async(true);
#endif
    
    // Enabling 50hz interrupt
    servos_attach_interrupt(setFlag);
}

void terminal_to_usb()
{
    isUSB = true;
    terminal_init(&SerialUSB);
}

/**
 * Main loop
 */
static void internal_loop()
{
    // Handling terminal
    terminal_tick();

    // Switching to USB mode
    if (SerialUSB.available() && !isUSB) {
        terminal_to_usb();
    }

    // Executing 50hz tick
    if (flag) {
        flag = false;
        tick();
        dxl_flush();
    }

    // Running user loop
    loop();
}

// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
__attribute__((constructor)) void premain()
{
    init();
}

int main(void)
{
    internal_setup();

    while (true) {
        internal_loop();
    }

    return 0;
}

