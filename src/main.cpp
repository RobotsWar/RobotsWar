#include <wirish/wirish.h>
#include <servos.h>
#include <terminal.h>

void setup() {
    servos_init();
    terminal_init(&SerialUSB);
}

void loop() {
    terminal_tick();
}

// Force init to be called *first*, i.e. before static object allocation.
// Otherwise, statically allocated objects that need libmaple may fail.
__attribute__((constructor)) void premain() {
    init();
}

int main(void) {
    setup();

    while (true) {
        loop();
    }

    return 0;
}

