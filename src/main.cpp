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

TERMINAL_COMMAND(hello, "Say hello!")
{
    terminal_io()->println("Hello!");
}

TERMINAL_COMMAND(twice, "Say twice the given number")
{
    float f;

    if (argc == 1) {
        f = atof(argv[0]);
        terminal_io()->print("Twice: ");
        terminal_io()->println(2*f);
    } else {
        terminal_io()->println("Usage: twice [number]");
    }
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
    pinMode(BOARD_LED_PIN, OUTPUT);
    servos_init();
    
    // Begining on WiFly Mode
    Serial3.begin(921600);
    terminal_init(&Serial3);
    
    // Attach the 50Hz interrupt
    servos_attach_interrupt(setFlag);
}

/**
 * Main loop
 */
void loop()
{
    terminal_tick();

    // If something is available on USB, switching to USB
    if (SerialUSB.available() && !isUSB) {
        isUSB = true;
        terminal_init(&SerialUSB);
    }

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

