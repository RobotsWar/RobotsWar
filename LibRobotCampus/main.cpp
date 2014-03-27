#include <cstdlib>
#include <wirish/wirish.h>
#include <servos.h>
#include <terminal.h>
#include <main.h>
#include <dxl.h>
#include "wifly.h"

volatile bool flag = false;
volatile bool isUSB = false;

/**
 * Interruption @50hz
 */
static void setFlag()
{
    // Ne pas écrire de code ici à moins de vraiment
    // savoir ce que vous faites
    flag = true;
}

/**
 * Setting up the board
 */
static void internal_setup()
{
    // Intialise les servomoteurs
    servos_init();
    
    // Initialise le mode WiFly
    WiFly.begin(921600);
    terminal_init(&WiFly);

    // Configure la led de la board
    pinMode(BOARD_LED_PIN, OUTPUT);
    digitalWrite(BOARD_LED_PIN, LOW);

    // Lance la configuration de l'utilisateur
    setup();

    // Passage du bus dxl en asynchrone
#if defined(DXL_AVAILABLE)
    dxl_async(true);
#endif
    
    // Définit l'interruption @50hz
    servos_attach_interrupt(setFlag);
}

/**
 * Boucle principale
 */
static void internal_loop()
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
        dxl_flush();
    }

    // Appelle la fonction loop de l'utilisateur
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

