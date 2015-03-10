#include <stdio.h>
#include <rhock/clock.h>
#ifndef __EMSCRIPTEN__
#include <wirish/wirish.h>
#endif

#ifndef __EMSCRIPTEN__
uint32_t rhock_gettime()
{
    return millis();
}
#endif
