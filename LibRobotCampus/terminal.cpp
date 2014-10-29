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

static bool terminal_last_ok = false;
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
bool terminal_execute(char *command_name, unsigned int command_name_length, 
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
                    return false;
                }
            }
        }
    }

    // If it fails again, display the "unknown command" message
    if (command == NULL) {
        terminalIO.print("Unknown command: ");
        terminalIO.write(command_name, command_name_length);
        terminalIO.println();
        return false;
    }

    return true;
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
        terminal_last_ok = terminal_execute(terminal_buffer, command_name_length, argc, argv);
    } else {
        terminal_last_ok = false;
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
            if (terminal_pos == 0 && terminal_last_ok) { 
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
            while (!terminalIO.io->available());
            terminalIO.io->read();
            while (!terminalIO.io->available());
            terminalIO.io->read();
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
