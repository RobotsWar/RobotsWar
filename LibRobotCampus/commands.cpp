#include <stdlib.h>
#include <string.h>
#include "terminal.h"
#include "servos.h"

TERMINAL_COMMAND(command_ui, 
    "Set position of a servo with UI. Usage: command_ui [label]")
{
    if (argc == 1) {
        uint8_t i = servos_index(argv[0]);
        if (i != (uint8_t)-1) {
            terminal_bar_init(-100, 100, (int)(servos_get_command(i)*100.0));
            while (terminal_bar_escaped() == false) {
                int pos = terminal_bar_tick();
                servos_command(i, ((float)pos)/100.0);
            }
        } else {
            terminal_io()->println("Unknown label");
        }
    } else {
        terminal_io()->println("Bad usage");
    }
}

TERMINAL_COMMAND(calibrate_ui, 
    "Calibrate a servo with UI. Usage: calibrate_ui [label]")
{
    if (argc == 1) {
        uint8_t i = servos_index(argv[0]);
        if (i != (uint8_t)-1) {
            uint16_t min = 0;
            uint16_t max = SERVOS_TIMERS_OVERFLOW;
            uint16_t zero = SERVOS_TIMERS_OVERFLOW/10;
            servos_calibrate(i, min, zero, max, false);
            servos_enable(i, true);
            //Min
            terminal_io()->println("Select min position:");
            terminal_bar_init(0, 3*SERVOS_TIMERS_OVERFLOW/10, servos_get_pos(i));
            while (terminal_bar_escaped() == false) {
                min = (uint16_t)terminal_bar_tick();
                servos_set_pos(i, min);
            }
            //Max
            terminal_io()->println("Select max position:");
            terminal_bar_init(min, 3*SERVOS_TIMERS_OVERFLOW/10, min);
            while (terminal_bar_escaped() == false) {
                max = (uint16_t)terminal_bar_tick();
                servos_set_pos(i, max);
            }
            //Zero
            terminal_io()->println("Select zero position:");
            terminal_bar_init(min, max, max);
            while (terminal_bar_escaped() == false) {
                zero = (uint16_t)terminal_bar_tick();
                servos_set_pos(i, zero);
            }
            //Calibrate
            servos_enable(i, false);
            uint8_t code = servos_calibrate(i, min, zero, max, false);
            if (code == 0) {
                terminal_io()->println("OK");
            } else {
                terminal_io()->println("Error");
            }
        } else {
            terminal_io()->println("Unknown label");
        }
    } else {
        terminal_io()->println("Bad usage");
    }
}

TERMINAL_COMMAND(emergency, "Disable all servos")
{
    servos_emergency();
    terminal_io()->println("OK");
}

TERMINAL_COMMAND(reset, 
    "Reset all or one servo position to zero. Usage: reset [label|all]")
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

TERMINAL_COMMAND(calibrate, 
    "Calibrate a servo. Usage: calibrate [label] [min] [max] [zero]")
{
    if (argc == 4) {
        uint8_t i = servos_index(argv[0]);
        uint16_t min = atoi(argv[1]);
        uint16_t max = atoi(argv[2]);
        uint16_t zero = atoi(argv[3]);
        if (i != (uint8_t)-1) {
            uint8_t code = servos_calibrate(i, min, zero, max, false);
            if (code == 0) {
                terminal_io()->println("OK");
            } else {
                terminal_io()->println("Error");
            }
        } else {
            terminal_io()->println("Unknown label");
        }
    } else {
        terminal_io()->println("Bad usage");
    }
}

TERMINAL_COMMAND(position, 
    "Set (timer) position to a servo. Usage: position [label] [pos]")
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

TERMINAL_COMMAND(command, 
    "Set relative position to a servo. Usage: command [label] [pos]")
{
    if (argc == 2) {
        uint8_t i = servos_index(argv[0]);
        float pos = atof(argv[1]);
        if (i != (uint8_t)-1) {
            servos_command(i, pos);
            terminal_io()->println("OK");
        } else {
            terminal_io()->println("Unknown label");
        }
    } else {
        terminal_io()->println("Bad usage");
    }
}

TERMINAL_COMMAND(disable, 
    "Disable all or one servo. Usage: disable [label|all]")
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

TERMINAL_COMMAND(enable, 
    "Enable all or one servo. Usage: enable [label|all]")
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

TERMINAL_COMMAND(status, "Display servos informations")
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
        terminal_io()->print("    command  = ");
        terminal_io()->println(servos_get_command(i));
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

TERMINAL_COMMAND(flush, "Clear all registered servos")
{
    servos_flush();
    terminal_io()->println("OK");
}

TERMINAL_COMMAND(register, 
    "Register a new servos. Usage: register [pin] [label]")
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

