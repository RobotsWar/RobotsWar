#include <cstdlib>
#include <wirish/wirish.h>
#include <servos.h>
#include <terminal.h>
#include <main.h>

TERMINAL_PARAMETER_DOUBLE(t, "Temps", 0.0);

/**
 * Vous pouvez écrire du code qui sera exécuté à 
 * l'initialisation ici
 */
void setup()
{
}

/**
 * Foncton appellée à 50hz, c'est ici que vous pouvez mettre
 * à jour les angles moteurs etc.
 */
void tick()
{
    t += 0.02; // 20ms
}

/**
 * Si vous souhaitez écrire ici du code, cette fonction sera
 * apellée en boucle
 */
void loop()
{
}
