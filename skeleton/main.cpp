#include <cstdlib>
#include <wirish/wirish.h>
#include <servos.h>
#include <terminal.h>

volatile bool flag = false;
volatile bool isUSB = false;
volatile int counter = 0;

/**
 * Example counter, incremented @50hz
 */
TERMINAL_COMMAND(counter, "See the counter")
{
    terminal_io()->print("Counter: ");
    terminal_io()->println(counter);
}

/**
 * Function called @50Hz
 */
void tick()
{
    counter++;
}

/**
 * Interrupt @50hz, set the flag
 */
void setFlag()
{
    flag = true;
}

/**
 * Setting up the board
 */
void setup()
{
    //Initialize robotcampus servos
    servos_init();
    
    //Begining on WiFly Mode
    Serial3.begin(921600);
    terminal_init(&Serial3);
    
    //Attach the 50Hz interrupt
    servos_attach_interrupt(setFlag);
}

/**
 * Main loop
 */
void loop()
{
    //Handle terminal communication
    terminal_tick();

    //If something is available on USB, switching to USB
    if (SerialUSB.available() && !isUSB) {
        isUSB = true;
        terminal_init(&SerialUSB);
    }

    //Handle @50Hz interrupt
    if (flag) {
        tick();
    }
}

// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
__attribute__((constructor)) void premain()
{
    init();
}

int main(void)
{
    setup();

    while (true) {
        loop();
    }

    return 0;
}

