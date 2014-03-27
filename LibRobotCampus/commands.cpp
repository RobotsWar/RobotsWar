#include <stdlib.h>
#include <string.h>
#include "terminal.h"
#include "servos.h"
#include "dxl.h"
#include "wifly.h"
#include "commands.h"

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
        "Go to forward mode, the WiFly will be forwarded to USB and vice-versa. Usage: forward [baudrate]")
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

    WiFly.begin(baudrate);

    while (1) {
        pos = 0;
        while (WiFly.available() && pos < sizeof(buffer)) {
            buffer[pos++] = WiFly.read();
        }

        if (pos > 0) {
            SerialUSB.write(buffer, pos);
        }

        while (SerialUSB.available()) {
            WiFly.write(SerialUSB.read());
        }
    }
}

TERMINAL_COMMAND(start, "Enable all the servos")
{
    servos_enable_all();
#if defined(DXL_AVAILABLE)
    dxl_wakeup();

    float voltage = dxl_average_voltage();
    if (voltage < 6) {
        terminal_io()->print("Warning: dynamixel voltage is low (");
        terminal_io()->print(voltage);
        terminal_io()->println(")");
    }
#endif
    terminal_io()->println("OK");
    started = true;
}

TERMINAL_COMMAND(stop, "Disable all servos")
{
    servos_disable_all();
#if defined(DXL_AVAILABLE)
    dxl_disable_all();
#endif
    terminal_io()->println("OK");
    started = false;
}

#endif
