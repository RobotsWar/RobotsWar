#include <stdlib.h>
#include <wirish/wirish.h>
#include <terminal.h>
#include <main.h>

TERMINAL_PARAMETER_DOUBLE(t, "Temps", 0.0);

/**
 * Writing initialization code here
 */
void setup()
{
}

/**
 * 50hz function
 */
void tick()
{
    t += 0.02; // 20ms
}

/**
 * Loop method, called everytime
 */
void loop()
{
}
