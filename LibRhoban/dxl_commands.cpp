#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "terminal.h"
#include "servos.h"
#include "dxl.h"

// Dynamixel specific commands

#if defined(DXL_AVAILABLE) and !defined(DISABLE_ROBOTCAMPUS_COMMANDS) and !defined(DISABLE_ALL_COMMANDS)

TERMINAL_COMMAND(dxl_scan,
        "Scans for dynamixel servos")
{
    int maxId = DXL_MAX_ID;
    if (argc) {
        maxId = atoi(argv[0]);
    }

    // Turning all the LEDs red
    dxl_write_byte(DXL_BROADCAST, DXL_LED, 0);

    terminal_io()->print("Scanning for servos up to ");
    terminal_io()->println(maxId);

    for (int id=1; id<=maxId; id++) {
        if (dxl_ping(id)) {
            terminal_io()->print(id);
            terminal_io()->println(" is present.");
            dxl_write_byte(id, DXL_LED, 1);
        }
    }
}

TERMINAL_COMMAND(dxl_monitor,
        "Monitors servos")
{
    int maxId = 0;
    for (int id=DXL_MAX_ID; id>0; id--) {
        if (dxl_ping(id)) {
            maxId = id;
            break;
        }
    }

    while (!terminal_io()->io->available()) {
        terminal_io()->println();
        for (int id=1; id<=maxId; id++) {

#ifdef DXL_VERSION_1
            char buffer[8];
            bool success;
            success = dxl_read(id, DXL_POSITION, buffer, sizeof(buffer));
            float position = dxl_value_to_position(id, dxl_makeword(buffer[0], buffer[1]));
            float voltage = buffer[6]/10.0;
            float temperature = buffer[7];
#endif

#ifdef DXL_VERSION_2
            char buffer[10];
            bool success;
            success = dxl_read(id, DXL_POSITION, buffer, sizeof(buffer));
            float position = dxl_value_to_position(id, dxl_makeword(buffer[0], buffer[1]));
            float voltage = buffer[8]/10.0;
            float temperature = buffer[9];
#endif

            terminal_io()->print(id);
            terminal_io()->print(" is at ");
            if (success) {
                terminal_io()->print(position);
                terminal_io()->print("\t\t");
                terminal_io()->print(voltage);
                terminal_io()->print("V\t");
                terminal_io()->print(temperature);
                terminal_io()->println("C");
            } else {
                terminal_io()->println("???");
            }
        }
        delay(100);
    }
}

TERMINAL_COMMAND(dxl_release,
        "Releases all the dynamixel servos")
{
    terminal_io()->println("Releasing torque of all servos.");
    dxl_write_word(DXL_BROADCAST, DXL_GOAL_TORQUE, 0);
}

TERMINAL_COMMAND(dxl_id,
        "Identify a servo")
{
    if (argc == 1) {
        int id = atoi(argv[0]);
        int on = 1;
        while (!terminal_io()->io->available()) {
            dxl_write_byte(id, DXL_LED, on);
            delay(500);
            on = !on;
        }
        dxl_write_byte(id, DXL_LED, 1);
    } else {
        terminal_io()->println("Usage: dxl_id <id>");
    }
}

TERMINAL_COMMAND(dxl_setbaud,
        "Sets the correct baudrate")
{
    if (argc == 1) {
        int id = atoi(argv[0]);
        terminal_io()->print("Setting baudrate 10000 to servo ");
        terminal_io()->println(id);
        dxl_write_byte(id, DXL_BAUD, 1);
    } else {
        terminal_io()->println("Usage: dxl_setbaud [id]");
    }
}

TERMINAL_COMMAND(dxl_init,
        "Initializes the dynamixel system")
{
    int baudrate = 1000000;
    if (argc) {
        baudrate = atoi(argv[0]);
    }

    terminal_io()->print("Starting dynamixel bus at ");
    terminal_io()->print(baudrate);
    terminal_io()->println(" bauds.");
    dxl_init(baudrate);
}

TERMINAL_COMMAND(dxl_enable,
        "Enable a dxl servo")
{
    if (argc == 1) {
        int id = atoi(argv[0]);

        dxl_enable(id);
    } else {
        terminal_io()->println("Usage: dxl_enable <id>");
    }
}

TERMINAL_COMMAND(dxl_disable,
        "DIsable a dxl servo")
{
    if (argc == 1) {
        int id = atoi(argv[0]);

        dxl_disable(id);
    } else {
        terminal_io()->println("Usage: dxl_disable <id>");
    }
}

TERMINAL_COMMAND(dxl_position,
        "Go to a given position")
{
    if (argc == 2) {
        int id = atoi(argv[0]);
        float position = atof(argv[1]);

        terminal_io()->print("Setting dynamixel position of actuator ");
        terminal_io()->print(id);
        terminal_io()->print(" to position ");
        terminal_io()->println(position);
        dxl_set_position(id, position);
    } else {
        terminal_io()->println("Usage: dxl_position <id> <position in °>");
    }
}

TERMINAL_COMMAND(dxl_ping,
        "Pings a dynamixel servo")
{
    if (argc == 1) {
        int id = atoi(argv[0]);
        terminal_io()->print("Pinging ");
        terminal_io()->println(id);

        if (dxl_ping(id)) {
            terminal_io()->println("Got a response.");
        } else {
            terminal_io()->println("No response.");
        }
    } else {
        terminal_io()->println("Usage: ping <id>");
    }
}

static void save_zero(ui8 id)
{
    bool success;
    float zero = dxl_get_position(id, &success);

    if (success) {
        dxl_set_zero(id, zero);
        terminal_io()->print("Zero of ");
        terminal_io()->print(id);
        terminal_io()->print(" set to ");
        terminal_io()->println(zero);
    }
}

TERMINAL_COMMAND(dxl_zero,
        "Sets the current position as zero")
{
    if (argc == 1) {
        save_zero(atoi(argv[0]));
    } else {
        for (int id=1; id<=DXL_MAX_ID; id++) {
            save_zero(id);
        }
    }
}

TERMINAL_COMMAND(dxl_calibrate,
        "Calibrates a dynamixel servo")
{
    if (argc == 1) {
        int id = atoi(argv[0]);

        if (dxl_ping(id)) {
            float min = 180;
            float max = -180;
            dxl_disable(id);
            terminal_io()->println("First, let's get some min and maxes, move the servo");
            while (!terminal_io()->io->available()) {
                bool success;
                float position = dxl_get_position(id, &success);
                if (position < min) min = position;
                if (position > max) max = position;
                terminal_io()->print("\rMin: ");
                terminal_io()->print(min);
                terminal_io()->print("\tMax: ");
                terminal_io()->print(max);
                terminal_io()->print("                             ");
                delay(100);
            }
            while (terminal_io()->io->available()) {
                terminal_io()->io->read();
            }
            terminal_io()->println();
            terminal_io()->println("Min and max saved!");
            dxl_set_min_max(id, min, max);
        } else {
            terminal_io()->println("Servo did not respond to ping");
        }
    } else {
        terminal_io()->println("Usage: dxl_calibrate <id>");
    }
}

TERMINAL_COMMAND(dxl_dump,
        "Dumps the dynamixel config")
{
    for (int id=1; id<=DXL_MAX_ID; id++) {
        struct dxl_config *config = dxl_get_config(id);

        if (config!=NULL && config->configured) {
            terminal_io()->print("dxl_set_zero(");
            terminal_io()->print(id);
            terminal_io()->print(", ");
            terminal_io()->print(config->zero);
            terminal_io()->println(");");

            terminal_io()->print("dxl_set_min_max(");
            terminal_io()->print(id);
            terminal_io()->print(", ");
            terminal_io()->print(config->min);
            terminal_io()->print(", ");
            terminal_io()->print(config->max);
            terminal_io()->println(");");
        }
    }
}

TERMINAL_COMMAND(dxl_snapshot,
        "Snapshots the current position")
{
    for (int id=1; id<=DXL_MAX_ID; id++) {
        struct dxl_config *config = dxl_get_config(id);

        if (config!=NULL && config->configured) {
            bool success;
            float position = dxl_get_position(id, &success);
            
            if (success) {
                position -= config->zero;
                if (!argc) {
                    terminal_io()->print("dxl_set_position(");
                    terminal_io()->print(id);
                    terminal_io()->print(", ");
                }
                terminal_io()->print(position);
                if (!argc) {
                    terminal_io()->print(")");
                }
                terminal_io()->print(" ");
                if (!argc) {
                    terminal_io()->println();
                }
            }
        }
    }
    if (argc) {
        terminal_io()->println();
    }
}

TERMINAL_COMMAND(dxl_voltage,
        "Prints the dynamixel average voltage")
{
    terminal_io()->print("Average voltage: ");
    terminal_io()->println(dxl_average_voltage());
}

TERMINAL_COMMAND(dxl_p, "Sets the P of the PIDs")
{
    if (argc == 1) {
        dxl_pidp(atoi(argv[0]));
    }
}

TERMINAL_COMMAND(dxl_compliance,
        "Sets the compliance slope")
{
    if (argc == 1) {
        dxl_compliance_slope(atoi(argv[0]));
    }
}

TERMINAL_COMMAND(dxl_margin,
        "Sets the compliance margin")
{
    if (argc == 1) {
        dxl_compliance_margin(atoi(argv[0]));
    }
}

TERMINAL_COMMAND(dxl_configure,
        "Configures a servo")
{
    if (argc < 1) {
        terminal_io()->println("Usage: dxl_configure <newId> <id=1>");
    } else {
        int newId = atoi(argv[0]);
        int id = 1;

        if (argc == 2) {
            id = atoi(argv[1]);
        }

        if (!dxl_ping(id)) {
            terminal_io()->print("Servo ");
            terminal_io()->print(id);
            terminal_io()->println(" did not respond");
        } else {
            terminal_io()->print("Reconfiguring ");
            terminal_io()->print(id);
            terminal_io()->print(" to ");
            terminal_io()->println(newId);
            dxl_configure(id, newId);
        }
    }
}

#endif

TERMINAL_COMMAND(dxl_forward,
        "Dynamixel forward mode")
{
    int baudrate = 1000000;
    if (argc) {
        baudrate = atoi(argv[0]);
    }
    terminal_io()->print("Starting dynamixel bus forwarding at ");
    terminal_io()->print(baudrate);
    terminal_io()->println(" bauds.");

    dxl_init(baudrate);
    while (true) {
        dxl_forward();
    }
}

