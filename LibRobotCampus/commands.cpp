#include <stdlib.h>
#include <string.h>
#include "terminal.h"
#include "servos.h"
#include "dxl.h"
#include "rc.h"
#include "commands.h"
#include "main.h"

bool started = false;

#if !defined(DISABLE_ALL_COMMANDS)

TERMINAL_COMMAND(mute, "Mute/Unmute the terminal")
{
    if (argc == 1) {
        int mute = atoi(argv[0]);
        terminal_silent(mute != 0);
    }
}

TERMINAL_COMMAND(forward,
        "Go to forward mode, the RC will be forwarded to USB and vice-versa. Usage: forward [baudrate]")
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

    RC.begin(baudrate);

    while (1) {
        pos = 0;
        while (RC.available() && pos < sizeof(buffer)) {
            buffer[pos++] = RC.read();
        }

        if (pos > 0) {
            SerialUSB.write(buffer, pos);
        }

        while (SerialUSB.available()) {
            RC.write(SerialUSB.read());
        }
    }
}

void start()
{
    servos_enable_all();
    started = true;
#if defined(DXL_AVAILABLE)
    tick();
    dxl_flush();
    dxl_configure_all();
    dxl_wakeup();

    float voltage = dxl_average_voltage();
    if (voltage < 6) {
        terminal_io()->print("Warning: dynamixel voltage is low (");
        terminal_io()->print(voltage);
        terminal_io()->println(")");
    }
#endif
    terminal_io()->println("OK");
}

void stop()
{
    servos_disable_all();
#if defined(DXL_AVAILABLE)
    dxl_disable_all();
#endif
    terminal_io()->println("OK");
    started = false;
}

void start_stop()
{
    if (!started) {
        start();
    } else {
        stop();
    }
}

TERMINAL_COMMAND(start, "Enable all the servos")
{
    start();
}

TERMINAL_COMMAND(stop, "Disable all servos")
{
    stop();
}

#endif
