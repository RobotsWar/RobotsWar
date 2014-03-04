TERMINAL_COMMAND(dxl_scan,
        "Scans for dynamixel servos")
{
    int maxId = 240;
    if (argc) {
        maxId = atoi(argv[0]);
    }

    // Turning all the LEDs off
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
    for (int id=200; id>0; id--) {
        if (dxl_ping(id)) {
            maxId = id;
            break;
        }
    }

    while (!terminal_io()->io->available()) {
        terminal_io()->println();
        for (int id=1; id<=maxId; id++) {
            bool success;
            float position = dxl_get_position(id, &success);
            terminal_io()->print(id);
            terminal_io()->print(" is at ");
            if (success) {
                terminal_io()->println(position);
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

TERMINAL_COMMAND(dxl_init,
        "Initializes the dynamixel system")
{
    int baudrate = 1000000;
    if (argc) {
        baudrate = atoi(argv[0]);
    }

    terminal_io()->print("Starting dynamixel bus forwarding at ");
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
