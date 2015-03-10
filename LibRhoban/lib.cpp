#include <stdlib.h>
#include <string.h>
#include "terminal.h"
#include "servos.h"
#include "dxl.h"
#include "rc.h"

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
#include "function.h"

Function::Function() : nbPoints(0)
{
}

double Function::getXMax()
{
    if (nbPoints == 0) {
        return 0.0;
    }

    return points[nbPoints-1][0];
}

void Function::addPoint(double x, double y)
{
    if (nbPoints < FUNCTION_MAX_POINTS) {
        points[nbPoints][0] = x;
        points[nbPoints][1] = y;

        if (nbPoints > 0) {
            ds[nbPoints-1] = points[nbPoints][1] - points[nbPoints-1][1];
            ds[nbPoints-1] /= points[nbPoints][0] - points[nbPoints-1][0];
        }

        nbPoints++;
    }
}

double Function::get(double x)
{
    int i;
    
    for (i=0; i<nbPoints; i++) {
        if (points[i][0] >= x) {
            break;
        }
    }

    if (i == 0 || i == nbPoints) {
        return 0.0;
    }

    return points[i-1][1]+ds[i-1]*(x-points[i-1][0]);
}

double Function::getMod(double x)
{
    double maxX = getXMax();

    if (x < 0.0 || x > maxX) {
        x -= maxX*(int)(x/maxX);
    }
    
    return get(x);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "servos.h"
#include "terminal.h"

/**
 * Servo structure
 * All position are stored as timer values
 */
struct servo_t
{
    uint8_t pin; //PWM output pin number
    uint16_t min; //Position min
    uint16_t max; //Positin max
    uint16_t init; //Position origin
    uint16_t pos; //Current position
    bool reversed; //True if direction are reversed (-1 gain)
    bool enabled; //True if servo are driven
    char label[SERVOS_ID_LENGTH+1]; //String shortcut
    double steps_per_degree;
};

/**
 * Global servos container
 */
static servo_t Servos[SERVOS_MAX_NB];
static uint8_t Servos_count = 0;
static bool Servos_enable_smoothing = false;
static double Servos_smooth = 0.0;

/**
 * Initialize timer
 * @param i : timer number
 */
void initTimer(uint8_t i)
{
    HardwareTimer timer(i);
    timer.pause();
    timer.setPrescaleFactor(SERVOS_TIMERS_PRESCALE);
    timer.setOverflow(SERVOS_TIMERS_OVERFLOW);
    timer.refresh();
    timer.resume();
}

void button_pressed()
{
    servos_disable_all();
    dxl_disable_all();
    terminal_io()->println("Booyah!");
}

TERMINAL_COMMAND(btn, "")
{
    terminal_io()->println(digitalRead(BOARD_BUTTON_PIN));
}

void servos_init()
{
    /**
     * Set up hardware timer
     */
    initTimer(1);
    initTimer(2);
    initTimer(3);
    initTimer(4);

    /**
     * Set up board led and button
     */
    pinMode(BOARD_BUTTON_PIN, INPUT_PULLDOWN);
    pinMode(BOARD_LED_PIN, OUTPUT);
    attachInterrupt(BOARD_BUTTON_PIN, button_pressed, RISING);
}

uint8_t servos_register(uint8_t pin, char* label)
{
    if (Servos_count < SERVOS_MAX_NB) {
        for (uint8_t i=0;i<Servos_count;i++) {
            if (
                strncmp(Servos[i].label, label, SERVOS_ID_LENGTH) == 0 ||
                Servos[i].pin == pin
            ) {
                return -1;
            }
        }

        Servos[Servos_count].pin = pin;
        Servos[Servos_count].min = 0;
        Servos[Servos_count].max = SERVOS_TIMERS_OVERFLOW;
        Servos[Servos_count].init = SERVOS_TIMERS_OVERFLOW/20;
        Servos[Servos_count].pos = Servos[Servos_count].init;
        Servos[Servos_count].reversed = false;
        Servos[Servos_count].enabled = false;
        Servos[Servos_count].steps_per_degree = DEFAULT_STEPS_PER_DEGREE;
        if (label != NULL) {
            strncpy(Servos[Servos_count].label, label, SERVOS_ID_LENGTH+1);
        } else {
            Servos[Servos_count].label[0] = '\0';
        }
        Servos_count++;
    
        return Servos_count-1;
    } else {
        return -1;
    }
}

uint8_t servos_index(char* label)
{
    for (uint8_t i=0;i<Servos_count;i++) {
        if (strncmp(Servos[i].label, label, SERVOS_ID_LENGTH) == 0) {
            return i;
        }
    }

    return -1;
}

uint8_t servos_count()
{
    return Servos_count;
}

uint8_t servos_get_pin(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].pin;
    else return -1;
}
uint16_t servos_get_min(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].min;
    else return -1;
}
uint16_t servos_get_init(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].init;
    else return -1;
}
uint16_t servos_get_max(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].max;
    else return -1;
}
uint16_t servos_get_pos(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].pos;
    else return -1;
}
bool servos_is_reversed(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].reversed;
    else return false;
}
bool servos_is_enabled(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].enabled;
    else return false;
}
char* servos_get_label(uint8_t index)
{
    if (index != -1 && index < Servos_count) return Servos[index].label;
    else return NULL;
}
float servos_get_command(uint8_t index)
{
    if (index != -1 && index < Servos_count) {
        float pos = (Servos[index].pos-Servos[index].init)/Servos[index].steps_per_degree;

        if (servos_is_reversed(index)) {
            pos *= -1;
        }

        return pos;
    } else {
        return 0.0;
    }
}

uint8_t servos_calibrate(uint8_t index,
    uint16_t min, uint16_t init, uint16_t max, bool reversed)
{
    if (
        index == -1 || index >= Servos_count || max <= min || 
        init < min || init > max
    ) {
        return 1;
    }

    Servos[index].min = min;
    Servos[index].max = max;
    Servos[index].init = init;
    Servos[index].pos = init;
    Servos[index].reversed = reversed;
    servos_set_pos(index, Servos[index].pos);

    return 0;
}

void servos_set_pos(uint8_t index, uint16_t pos)
{
    if (index == -1 || index >= Servos_count) {
        return;
    }
    if (pos < Servos[index].min) {
        pos = Servos[index].min;
    }
    if (pos > Servos[index].max) {
        pos = Servos[index].max;
    }
    
    Servos[index].pos = pos;
    pwmWrite(Servos[index].pin, Servos[index].pos);
}

void servos_command(uint8_t index, float pos)
{
    if (index == -1 || index >= Servos_count) {
        return;
    }
    
    if (Servos_enable_smoothing) {
        float old_pos = servos_get_command(index);
        if (pos > old_pos + Servos_smooth) {
            pos = old_pos + Servos_smooth;
        }
        if (pos < old_pos - Servos_smooth) {
            pos = old_pos - Servos_smooth;
        }
    }

    if (Servos[index].reversed) {
        pos *= -1;
    }

    uint16_t p = (int)(Servos[index].init + pos*Servos[index].steps_per_degree);

    servos_set_pos(index, p);
}

void servos_reset(uint8_t index)
{
    if (index == -1 || index >= Servos_count) {
        return;
    }
    servos_set_pos(index, Servos[index].init);
}

void servos_enable(uint8_t index, bool enabled)
{
    if (index == -1 || index >= Servos_count) {
        return;
    }
    if (enabled == false && Servos[index].enabled == true) {
        pinMode(Servos[index].pin, INPUT_FLOATING);
        Servos[index].enabled = false;
    } else if (enabled == true && Servos[index].enabled == false) {
        servos_set_pos(index, Servos[index].pos);
        pinMode(Servos[index].pin, PWM);
        Servos[index].enabled = true;
    }
}

void servos_enable_all()
{
    for (int8_t i=0; i<Servos_count; i++) {
        servos_enable(i, true);
        delay(21);
    }
    
    digitalWrite(BOARD_LED_PIN, HIGH);
}

void servos_disable_all()
{
    for (int8_t i=0; i<Servos_count; i++) {
        servos_enable(i, false);
    }

    digitalWrite(BOARD_LED_PIN, LOW);
}

void servos_flush()
{
    for (int8_t i=0; i<Servos_count; i++) {
        servos_enable(i, false);
    }
    Servos_count = 0;
}

void servos_attach_interrupt(voidFuncPtr func)
{
    HardwareTimer timer(4);
    timer.setChannelMode(4, TIMER_OUTPUT_COMPARE);
    timer.setCompare(4, 3*SERVOS_TIMERS_OVERFLOW/10);
    timer.attachInterrupt(4, func);
    timer.refresh();
    timer.resume();
}

void servos_set_smoothing(double smooth)
{
    Servos_enable_smoothing = (smooth > 0);
    Servos_smooth = smooth;
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "terminal.h"

/**
 * Control bar structure
 */
struct terminal_bar_t
{
    int min;
    int max;
    int pos;
    bool escape;
};

/**
 * Global variables terminal
 */

static char terminal_buffer[TERMINAL_BUFFER_SIZE];

static unsigned int terminal_last_pos = 0;
static unsigned int terminal_pos = 0;

static const struct terminal_command *terminal_commands[TERMINAL_MAX_COMMANDS];

static unsigned int terminal_command_count = 0;

static TerminalIO terminalIO;

static terminal_bar_t terminal_bar;

static bool terminal_echo_mode = true;

TerminalIO::TerminalIO() : io(NULL), silent(false)
{
}

bool TerminalIO::hasIO()
{
    return io != NULL;
}

void TerminalIO::setIO(Serial *io_)
{
    io = io_;
}

void TerminalIO::write(uint8 c)
{
    if (!silent) {
        io->write(c);
    }
}

void TerminalIO::write(const void *buf, uint32 len)
{
    if (!silent) {
        io->write(buf, len);
    }
}

/**
 * Registers a command
 */
void terminal_register(const struct terminal_command *command)
{
    terminal_commands[terminal_command_count++] = command;
}

static void displayHelp(bool parameter)
{
    char buffer[256];
    unsigned int i;

    if (parameter) {
        terminal_io()->println("Available parameters:");
    } else {
        terminal_io()->println("Available commands:");
    }
    terminal_io()->println();

    for (i=0; i<terminal_command_count; i++) {
        const struct terminal_command *command = terminal_commands[i];

        if (command->parameter != parameter) {
            continue;
        }

        int namesize = strlen(command->name);
        int descsize = strlen(command->description);
        int typesize = (command->parameter_type == NULL) ? 0 : strlen(command->parameter_type);

        memcpy(buffer, command->name, namesize);
        buffer[namesize++] = ':';
        buffer[namesize++] = '\r';
        buffer[namesize++] = '\n';
        buffer[namesize++] = '\t';
        memcpy(buffer+namesize, command->description, descsize);
        if (typesize) {
            buffer[namesize+descsize++] = ' ';
            buffer[namesize+descsize++] = '(';
            memcpy(buffer+namesize+descsize, command->parameter_type, typesize);
            buffer[namesize+descsize+typesize++] = ')';
        }
        buffer[namesize+descsize+typesize++] = '\r';
        buffer[namesize+descsize+typesize++] = '\n';
        terminal_io()->write(buffer, namesize+descsize+typesize);
    }
}

/**
 * Internal helping command
 */
TERMINAL_COMMAND(help, "Displays the help about commands")
{
    displayHelp(false);
}

void terminal_params_show()
{
    unsigned int i;

    for (i=0; i<terminal_command_count; i++) {
        const struct terminal_command *command = terminal_commands[i];

        if (command->parameter) {
            command->command(0, NULL);
        }
    }
}

/**
 * Display available parameters
 */
TERMINAL_COMMAND(params, "Displays the available parameters. Usage: params [show]")
{
    if (argc && strcmp(argv[0], "show")==0) {
        terminal_params_show();
    } else {
        displayHelp(true);
    }
}

/**
 * Switch echo mode
 */
TERMINAL_COMMAND(echo, "Switch echo mode. Usage echo [on|off]")
{
    if ((argc == 1 && strcmp("on", argv[0])) || terminal_echo_mode == false) {
        terminal_echo_mode = true;
        terminal_io()->println("Echo enabled");
    } else {
        terminal_echo_mode = false;
        terminal_io()->println("Echo disabled");
    }
}

/**
 * Write the terminal prompt
 */
void terminal_prompt()
{
    terminalIO.print(TERMINAL_PROMPT);
}

const struct terminal_command *terminal_find_command(char *command_name, unsigned int command_name_length)
{
    unsigned int i;

    for (i=0; i<terminal_command_count; i++) {
        const struct terminal_command *command = terminal_commands[i];

        if (strlen(command->name) == command_name_length && strncmp(terminal_buffer, command->name, command_name_length) == 0) {
            return command;
        }
    }

    return NULL;
}

/***
 * Executes the given command with given parameters
 */
void terminal_execute(char *command_name, unsigned int command_name_length, 
    unsigned int argc, char **argv)
{
    unsigned int i;
    const struct terminal_command *command;
    
    // Try to find and execute the command
    command = terminal_find_command(command_name, command_name_length);
    if (command != NULL) {
        command->command(argc, argv);
    }

    // If it fails, try to parse the command as an allocation (a=b)
    if (command == NULL) {
        for (i=0; i<command_name_length; i++) {
            if (command_name[i] == '=') {
                command_name[i] = '\0';
                command_name_length = strlen(command_name);
                command = terminal_find_command(command_name, command_name_length);

                if (command && command->parameter) {
                    argv[0] = command_name+i+1;
                    argv[1] = NULL;
                    argc = 1;
                    command->command(argc, argv);
                } else {
                    command = NULL;
                }

                if (!command) {
                    terminalIO.print("Unknown parameter: ");
                    terminalIO.write(command_name, command_name_length);
                    terminalIO.println();
                    return;
                }
            }
        }
    }

    // If it fails again, display the "unknown command" message
    if (command == NULL) {
        terminalIO.print("Unknown command: ");
        terminalIO.write(command_name, command_name_length);
        terminalIO.println();
    }
}

/***
 * Process the receive buffer to parse the command and executes it
 */
void terminal_process()
{
    char *saveptr;
    unsigned int command_name_length;

    unsigned int argc = 0;
    char* argv[TERMINAL_MAX_ARGUMENTS+1];
    
    terminalIO.println();

    strtok_r(terminal_buffer, " ", &saveptr);
    while (
        (argv[argc] = strtok_r(NULL, " ", &saveptr)) != NULL && 
        argc < TERMINAL_MAX_ARGUMENTS
    ) {
        *(argv[argc]-1) = '\0';
        argc++;
    }
    
    if (saveptr != NULL) {
        *(saveptr - 1) = ' ';
    }
    
    command_name_length = strlen(terminal_buffer);

    if (command_name_length > 0) {
        terminal_execute(terminal_buffer, command_name_length, argc, argv);
    }

    terminal_last_pos = terminal_pos;
    terminal_pos = 0;
    terminal_prompt();
}

/**
 * Save the Serial object globaly
 */
void terminal_init(Serial *serial)
{
    if (serial != NULL) {
        terminalIO.setIO(serial);
        terminalIO.silent = false;
    }
    terminal_prompt();
    terminal_bar.escape = true;
}

/**
 * Ticking the terminal, this will cause lookup for characters 
 * and eventually a call to the process function on new lines
 */
void terminal_tick()
{
    if (!terminalIO.hasIO()) {
        return;
    }

    char c;
    uint8 input;

    while (terminalIO.io->available()) {
        input = terminalIO.io->read();
        c = (char)input;
        if (c == '\0') {
            continue;
        }

        //Return key
        if (c == '\r' || c == '\n') {
            if (terminal_pos == 0) { 
                // If the user pressed no keys, restore the last 
                // command and run it again
                unsigned int i;
                for (i=0; i<terminal_last_pos; i++) {
                    if (terminal_buffer[i] == '\0') {
                        terminal_buffer[i] = ' ';
                    }
                }
                terminal_pos = terminal_last_pos;
            }
            terminal_buffer[terminal_pos] = '\0';
            terminal_process();
        //Back key
        } else if (c == '\x7f') {
            if (terminal_pos > 0) {
                terminal_pos--;
                terminalIO.print("\x8 \x8");
            }
        //Special key
        } else if (c == '\x1b') {
            char code[2];
            while (!terminalIO.io->available());
            code[0] = terminalIO.io->read();
            while (!terminalIO.io->available());
            code[1] = terminalIO.io->read();
        //Others
        } else {
            terminal_buffer[terminal_pos] = c;
            if (terminal_echo_mode) {
                terminalIO.print(c);
            }

            if (terminal_pos < TERMINAL_BUFFER_SIZE-1) {
                terminal_pos++;
            }
        }
    }
}

/**
 * Returns Print and Read instance enabling user's command
 * to write and read on serial port
 */
TerminalIO* terminal_io()
{
    return &terminalIO;
}

void terminal_bar_init(int min, int max, int pos)
{
    if (min < max && pos >= min && pos <= max) {
        terminal_bar.min = min;
        terminal_bar.max = max;
        terminal_bar.pos = pos;
        terminal_bar.escape = false;
    }
}

int terminal_bar_tick()
{
    int step = terminal_bar.max - terminal_bar.min;
    step = step / TERMINAL_BAR_STEP;
    if (step == 0) {
        step = 1;
    }

    if (terminal_bar.escape == false) {
        terminal_io()->print("\x8\x8\x8\x8    ");
        terminal_io()->print("\r");
        terminal_io()->print("Bar: ");
        terminal_io()->print(terminal_bar.pos);
        terminal_io()->print(" | ");
        terminal_io()->print(terminal_bar.min);
        terminal_io()->print(" ");
        for (int i=terminal_bar.min;i<=terminal_bar.max;i+=step) {
            if (i <= terminal_bar.pos) {
                terminal_io()->print("=");
            } else {
                terminal_io()->print(".");
            }
        }
        terminal_io()->print(" ");
        terminal_io()->print(terminal_bar.max);

        while (true) {
            while (!terminal_io()->io->available());
            int controlKey = 0;
            bool specialKey = false;
            char input = (char)terminal_io()->io->read();
            //Detect control characters
            if (input == '\x1b') {
                specialKey = true;
            } else if (input == '^') {
                while (!terminalIO.io->available());
                input = (char)terminal_io()->io->read();
                if (input == '[') {
                    specialKey = true;
                }
            }
            //Arrow keys
            if (specialKey) {
                char code[2];
                while (!terminalIO.io->available());
                code[0] = terminalIO.io->read();
                while (!terminalIO.io->available());
                code[1] = terminalIO.io->read();
                //Left
                if (code[0] == '[' && code[1] == 'D') {
                    controlKey = -1;
                //Right
                } else if (code[0] == '[' && code[1] == 'C') {
                    controlKey = 1;
                }
            //Alias keys for arrows left
            } else if (input == 'h') {
                controlKey = -1;
            //Alias keys for arrows right
            } else if (input == 'l') {
                controlKey = 1;
            }
            //Apply control
            if (controlKey == 1) {
                terminal_bar.pos += step;
                break;
            } else if (controlKey == -1) {
                terminal_bar.pos -= step;
                break;
            } else if (input == '\r' || input == '\n') {
                terminal_io()->println();
                terminal_bar.escape = true;
                break;
            }
        }
        if (terminal_bar.pos < terminal_bar.min) {
            terminal_bar.pos = terminal_bar.min;
        } else if (terminal_bar.pos > terminal_bar.max) {
            terminal_bar.pos = terminal_bar.max;
        }
    } 

    return terminal_bar.pos;
}

bool terminal_bar_escaped()
{
    return terminal_bar.escape;
}


void terminal_silent(bool silent)
{
    terminalIO.silent = silent;
}
