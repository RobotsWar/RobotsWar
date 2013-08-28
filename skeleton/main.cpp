#include <cstdlib>
#include <wirish/wirish.h>
#include <servos.h>
#include <terminal.h>

volatile bool flag = false;
volatile bool isUSB = false;

TERMINAL_PARAMETER_DOUBLE(t, "Temps", 0.0);

/**
 * Foncton appellée à 50hz, c'est ici que vous pouvez mettre
 * à jour les angles moteurs etc.
 */
void tick()
{
    t += 0.02; // 20ms
}

/**
 * Interruption @50hz
 */
void setFlag()
{
    // Ne pas écrire de code ici à moins de vraiment
    // savoir ce que vous faites
    flag = true;
}

/**
 * Setting up the board
 */
void setup()
{
    // Intialise les servomoteurs
    servos_init();
    
    // Initialise le mode WiFly
    Serial3.begin(921600);
    terminal_init(&Serial3);
    
    // Définit l'interruption @50hz
    servos_attach_interrupt(setFlag);

    // Configure la led de la board
    pinMode(BOARD_LED_PIN, OUTPUT);
    digitalWrite(BOARD_LED_PIN, LOW);
}

/**
 * Boucle principale
 */
void loop()
{
    // Gère la communication du terminal
    terminal_tick();

    // Si quelque chose est disponible au niveau de l'USB, switch
    // sur le port
    if (SerialUSB.available() && !isUSB) {
        isUSB = true;
        terminal_init(&SerialUSB);
    }

    // Exécute le code @50hz
    if (flag) {
        flag = false;
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

