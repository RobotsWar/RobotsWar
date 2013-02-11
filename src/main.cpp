#include <wirish/wirish.h>
#include <servos.h>
#include <terminal.h>

#define EYE1 6
#define EYE2 7

volatile bool isUSB = false;
volatile int counter = 0;

void tick()
{
    counter++;
}

TERMINAL_COMMAND(counter, "See the counter")
{
    terminal_io()->print("Counter: ");
    terminal_io()->println(counter);
}

void setup()
{
    pinMode(BOARD_LED_PIN, OUTPUT);
    servos_init();
    
    // Begining on WiFly Mode
    Serial3.begin(115200);
    terminal_init(&Serial3);
    
    // Attach the 50Hz interrupt
    servos_attach_interrupt(tick);
}

void loop()
{
    terminal_tick();

    // If something is available on USB, switching to USB
    if (SerialUSB.available() && !isUSB) {
        isUSB = true;
        terminal_init(&SerialUSB);
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

