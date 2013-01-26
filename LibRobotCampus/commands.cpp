#include <stdlib.h>
#include <string.h>
#include "terminal.h"
#include "servos.h"

TERMINAL_COMMAND(emergency, "Disable all servos")
{
    servos_emergency();
    terminal_io()->println("OK");
}

TERMINAL_COMMAND(servos_reset, 
    "Reset all or one servo position to zero. Usage: servos_reet [label|all]")
{
    if (argc == 1) {
        if (strcmp(argv[0], "all") == 0) {
            uint8_t i;
            for (i=0;i<servos_count();i++) {
                servos_reset(i);
            }
            terminal_io()->println("OK");
        } else {
            uint8_t i = servos_index(argv[0]);
            if (i != (uint8_t)-1) {
                servos_reset(i);
                terminal_io()->println("OK");
            } else {
                terminal_io()->println("Unknown label");
            }
        }
    } else {
        terminal_io()->println("Bad usage");
    }
}

TERMINAL_COMMAND(servos_position, 
    "Set (timer) position to a servo. Usage: servos_position [label] [pos]")
{
    if (argc == 2) {
        uint8_t i = servos_index(argv[0]);
        uint16_t pos = atoi(argv[1]);
        if (i != (uint8_t)-1) {
            servos_set_pos(i, pos);
            terminal_io()->println("OK");
        } else {
            terminal_io()->println("Unknown label");
        }
    } else {
        terminal_io()->println("Bad usage");
    }
}

TERMINAL_COMMAND(servos_disable, 
    "Disable all or one servo. Usage: servos_disable [label|all]")
{
    if (argc == 1) {
        if (strcmp(argv[0], "all") == 0) {
            uint8_t i;
            for (i=0;i<servos_count();i++) {
                servos_enable(i, false);
            }
            terminal_io()->println("OK");
        } else {
            uint8_t i = servos_index(argv[0]);
            if (i != (uint8_t)-1) {
                servos_enable(i, false);
                terminal_io()->println("OK");
            } else {
                terminal_io()->println("Unknown label");
            }
        }
    } else {
        terminal_io()->println("Bad usage");
    }
}

TERMINAL_COMMAND(servos_enable, 
    "Enable all or one servo. Usage: servos_enable [label|all]")
{
    if (argc == 1) {
        if (strcmp(argv[0], "all") == 0) {
            uint8_t i;
            for (i=0;i<servos_count();i++) {
                servos_enable(i, true);
            }
            terminal_io()->println("OK");
        } else {
            uint8_t i = servos_index(argv[0]);
            if (i != (uint8_t)-1) {
                servos_enable(i, true);
                terminal_io()->println("OK");
            } else {
                terminal_io()->println("Unknown label");
            }
        }
    } else {
        terminal_io()->println("Bad usage");
    }
}

TERMINAL_COMMAND(servos_status, "Display servos informations")
{
    int count = servos_count();
    terminal_io()->print("> ");
    terminal_io()->print(count);
    terminal_io()->println(" servos registered");
    for (int i=0;i<count;i++) {
        terminal_io()->print("[");
        terminal_io()->print(servos_get_label(i));
        terminal_io()->println("]");
        
        terminal_io()->print("    index    = ");
        terminal_io()->println(i);
        terminal_io()->print("    pin      = ");
        terminal_io()->println(servos_get_pin(i));
        terminal_io()->print("    pos      = ");
        terminal_io()->println(servos_get_pos(i));
        terminal_io()->print("    min      = ");
        terminal_io()->println(servos_get_min(i));
        terminal_io()->print("    max      = ");
        terminal_io()->println(servos_get_max(i));
        terminal_io()->print("    zero     = ");
        terminal_io()->println(servos_get_zero(i));
        terminal_io()->print("    reversed = ");
        terminal_io()->println(servos_is_reversed(i));
        terminal_io()->print("    enabled  = ");
        terminal_io()->println(servos_is_enabled(i));
    }
}

TERMINAL_COMMAND(servos_add, 
    "Register a new servos. Usage: servos_add [pin] [label]")
{
    if (argc == 2) {
        uint8_t pin = atoi(argv[0]);
        char* label = argv[1];
        if (servos_register(pin, label) != (uint8_t)-1) {
            terminal_io()->println("OK");
        } else {
            terminal_io()->println("Error");
        }
    } else {
        terminal_io()->println("Bad usage");
    }
}

