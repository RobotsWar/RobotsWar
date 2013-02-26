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
            terminal_bar_init(-90, 90, (int)(servos_get_command(i)));
            while (terminal_bar_escaped() == false) {
                int pos = terminal_bar_tick();
                servos_command(i, pos);
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
            uint16_t init = SERVOS_TIMERS_OVERFLOW/20;
            servos_calibrate(i, min, init, max, servos_is_reversed(i));
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
            terminal_io()->println("Select init position:");
            terminal_bar_init(min, max, max);
            while (terminal_bar_escaped() == false) {
                init = (uint16_t)terminal_bar_tick();
                servos_set_pos(i, init);
            }
            //Calibrate
            servos_enable(i, false);
            uint8_t code = servos_calibrate(i, min, init, max, servos_is_reversed(i));
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

TERMINAL_COMMAND(start, "Enable all the servos")
{
    servos_enable_all();
    terminal_io()->println("OK");
}

TERMINAL_COMMAND(stop, "Disable all servos")
{
    servos_disable_all();
    terminal_io()->println("OK");
}

TERMINAL_COMMAND(reset, 
    "Reset all or one servo position to init. Usage: reset [label|all]")
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
    "Calibrate a servo. Usage: calibrate [label] [min] [max] [init]")
{
    if (argc == 4) {
        uint8_t i = servos_index(argv[0]);
        uint16_t min = atoi(argv[1]);
        uint16_t max = atoi(argv[2]);
        uint16_t init = atoi(argv[3]);
        if (i != (uint8_t)-1) {
            uint8_t code = servos_calibrate(i, min, init, max, false);
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
    "Disable one servo. Usage: disable [label]")
{
    if (argc == 1) {
        uint8_t i = servos_index(argv[0]);
        if (i != (uint8_t)-1) {
            servos_enable(i, false);
            terminal_io()->println("OK");
        } else {
            terminal_io()->println("Unknown label");
        }
    } else {
        terminal_io()->println("Bad usage");
    }
}

TERMINAL_COMMAND(enable, 
    "Enable all or one servo. Usage: enable [label]")
{
    if (argc == 1) {
        uint8_t i = servos_index(argv[0]);
        if (i != (uint8_t)-1) {
            servos_enable(i, true);
            terminal_io()->println("OK");
        } else {
            terminal_io()->println("Unknown label");
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

    for (int i=0; i<count; i++) {
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
        terminal_io()->print("    init     = ");
        terminal_io()->println(servos_get_init(i));
        terminal_io()->print("    reversed = ");
        terminal_io()->println(servos_is_reversed(i));
        terminal_io()->print("    enabled  = ");
        terminal_io()->println(servos_is_enabled(i));
    }
}

TERMINAL_COMMAND(dumpcommand, "Displays the servo initialization commands")
{
    int count = servos_count();
    if (count == 0) {
        terminal_io()->println("no servo registered");
        return;
    }

    for (int i=0;i<count;i++) {
        // Printing the register code
        terminal_io()->print("register ");
        terminal_io()->print(servos_get_pin(i));
        terminal_io()->print(" ");
        terminal_io()->print(servos_get_label(i));
        terminal_io()->println();
        
        // Printing the calibration code
        terminal_io()->print("calibrate ");
        terminal_io()->print(servos_get_label(i));
        terminal_io()->print(" ");
        terminal_io()->print(servos_get_min(i));
        terminal_io()->print(" ");
        terminal_io()->print(servos_get_max(i));
        terminal_io()->print(" ");
        terminal_io()->print(servos_get_init(i));
        terminal_io()->println();
    }
}

TERMINAL_COMMAND(dumpcode, "Displays the servo initialization C code")
{
    int count = servos_count();
    if (count == 0) {
        terminal_io()->println("no servo registered");
        return;
    }

    for (int i=0;i<count;i++) {
        // Definine the index
        terminal_io()->print("#define SERVO_");
        terminal_io()->print(servos_get_label(i));
        terminal_io()->print(" ");
        terminal_io()->println(i);
    }

    for (int i=0;i<count;i++) {
        // Printing the register code
        terminal_io()->print("servos_register(");
        terminal_io()->print(servos_get_pin(i));
        terminal_io()->print(", \"");
        terminal_io()->print(servos_get_label(i));
        terminal_io()->println("\");");
        
        // Printing the calibration code
        terminal_io()->print("servos_calibrate(");
        terminal_io()->print(i);
        terminal_io()->print(", ");
        terminal_io()->print(servos_get_min(i));
        terminal_io()->print(", ");
        terminal_io()->print(servos_get_init(i));
        terminal_io()->print(", ");
        terminal_io()->print(servos_get_max(i));
        terminal_io()->print(", ");
        if (servos_is_reversed(i)) {
            terminal_io()->print("true");
        } else {
            terminal_io()->print("false");
        }
        terminal_io()->println(");");
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

TERMINAL_COMMAND(smooth,
        "Sets the smoothing")
{
    if (argc == 1) {
        double smooth = atof(argv[0]);
        servos_set_smoothing(smooth);
        terminal_io()->println("Smoothing set");
    }
}

TERMINAL_COMMAND(mute, "Mute/Unmute the terminal")
{
    if (argc == 1) {
        int mute = atoi(argv[0]);
        terminal_silent(mute != 0);
    }
}

TERMINAL_COMMAND(forward,
        "Go to forward mode, the Serial3 will be forwarded to USB and vice-versa. Usage: forward [baudrate]")
{
    int baudrate = 921600;
    char buffer[512];
    unsigned int pos;
    terminal_io()->print("The forward mode will be enabled, ");
    terminal_io()->println("you'll need to reboot the board to return to normal operation");

    if (argc) {
        baudrate = atoi(argv[0]);
    }
    terminal_io()->print("Using baudrate ");
    terminal_io()->println(baudrate);

    Serial3.begin(baudrate);

    while (1) {
        pos = 0;
        while (Serial3.available() && pos < sizeof(buffer)) {
            buffer[pos++] = Serial3.read();
        }

        if (pos > 0) {
            SerialUSB.write(buffer, pos);
        }

        while (SerialUSB.available()) {
            Serial3.write(SerialUSB.read());
        }
    }
}
