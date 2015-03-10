#include <stdlib.h>
#include <wirish/wirish.h>
#include <servos.h>
#include <terminal.h>
#include <main.h>
#include <dxl.h>
#if defined(RHOCK)
#include <rhock/stream.h>
#include <rhock/vm.h>
#endif
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

#if defined(RHOCK)
    // Initializing rhock
    rhock_vm_init();
#endif
    
    // Enabling 50hz interrupt
    servos_attach_interrupt(setFlag);
}

void terminal_to_rc()
{
    isUSB = false;
    terminal_init(&RC);
}

void terminal_to_usb()
{
    isUSB = true;
    terminal_init(&SerialUSB);
}

bool terminal_disabled = false;

/**
 * Disabling terminal
 */
void disable_terminal()
{
    terminal_disabled = true;
}

void enable_terminal()
{
    terminal_disabled = false;
}

#if defined(RHOCK)
const char rhock_exit[] = "!rhock\r";
const int rhock_exit_len = 7;
int rhock_exit_pos = 0;
bool rhock_mode = false;

bool is_rhock_mode()
{
    return rhock_mode;
}

void rhock_stream_send(uint8_t c)
{
    if (rhock_mode) {
        terminal_io()->write(&c, 1);
    }
}

TERMINAL_COMMAND(rhock, "Enter rhock mode")
{
    rhock_mode = true;
    terminal_disabled = true;
}
#endif

/**
 * Main loop
 */
static void internal_loop()
{
#if defined(RHOCK)
    if (rhock_mode) {
        while (terminal_io()->io->available()) {
            char c = terminal_io()->io->read();
            if (rhock_exit[rhock_exit_pos] == c) {
                rhock_exit_pos++;
                if (rhock_exit_pos >= rhock_exit_len) {
                    rhock_mode = false;
                    terminal_disabled = false;
                    terminal_reset();
                }
            } else {
                rhock_exit_pos = 0;
            }

            rhock_stream_recv(c);
        }
    }

    rhock_vm_tick();
#endif

    if (!terminal_disabled) {
        // Handling terminal
        terminal_tick();
    }

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

