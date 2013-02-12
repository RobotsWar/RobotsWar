#include <stdlib.h>
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

static unsigned int terminal_pos = 0;

static const struct terminal_command *terminal_commands[TERMINAL_MAX_COMMANDS];

static unsigned int terminal_command_count = 0;

static Serial *SerialIO = 0;

static terminal_bar_t terminal_bar;

static bool terminal_echo_mode = true;

/**
 * Registers a command
 */
void terminal_register(const struct terminal_command *command)
{
    terminal_commands[terminal_command_count++] = command;
}

/**
 * Internal helping command
 */
TERMINAL_COMMAND(help, "Displays the help about commands")
{
    unsigned int i;

    terminal_io()->println("Available commands:");
    terminal_io()->println();

    for (i=0; i<terminal_command_count; i++) {
        const struct terminal_command *command = terminal_commands[i];

        terminal_io()->print(command->name);
        terminal_io()->println(":");
        terminal_io()->print("    ");
        terminal_io()->print(command->description);
        terminal_io()->println();
    }
}

/**
 * Switch echo mode
 */
TERMINAL_COMMAND(echo, "Switch echo mode")
{
    if (terminal_echo_mode == false) {
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
    SerialIO->print(TERMINAL_PROMPT);
}

/***
 * Executes the given command with given parameters
 */
void terminal_execute(char *command_name, unsigned int command_name_length, 
    unsigned int argc, char **argv)
{
    unsigned int i;
    char command_found = 0;

    for (i=0; i<terminal_command_count; i++) {
        const struct terminal_command *command = terminal_commands[i];

        if (strlen(command->name) == command_name_length && strncmp(terminal_buffer, command->name, command_name_length) == 0) {
            command->command(argc, argv);

            command_found = 1;
            break;
        }
    }

    if (!command_found) {
        SerialIO->print("Unknwon command: ");
        for (i=0; i<command_name_length; i++) {
            SerialIO->print((char)terminal_buffer[i]);
        }
        SerialIO->println();
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
    
    SerialIO->println();

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

    terminal_pos = 0;
    terminal_prompt();
}

/**
 * Save the Serial object globaly
 */
void terminal_init(Serial *serial)
{
    if (serial != 0) {
        SerialIO = serial;
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
    if (SerialIO == 0) {
        return;
    }

    char c;
    uint8 input;

    while (SerialIO->available()) {
        input = SerialIO->read();
        c = (char)input;

        //Return key
        if (c == '\r' || c == '\n') {
            terminal_buffer[terminal_pos] = '\0';
            terminal_process();
        //Back key
        } else if (c == '\x7f') {
            if (terminal_pos > 0) {
                terminal_pos--;
                SerialIO->print("\x8 \x8");
            }
        //Special key
        } else if (c == '\x1b') {
            char code[2];
            while (!SerialIO->available());
            code[0] = SerialIO->read();
            while (!SerialIO->available());
            code[1] = SerialIO->read();
        //Others
        } else {
            terminal_buffer[terminal_pos] = c;
            if (terminal_echo_mode) {
                SerialIO->print(c);
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
Serial* terminal_io()
{
    return SerialIO;
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
            while (!terminal_io()->available());
            bool specialKey = false;
            char input = (char)terminal_io()->read();
            //Detect arrow keys
            if (input == '\x1b') {
                specialKey = true;
            } else if (input == '^') {
                while (!SerialIO->available());
                input = (char)terminal_io()->read();
                if (input == '[') {
                    specialKey = true;
                }
            }
            //Arrow keys
            if (specialKey) {
                char code[2];
                while (!SerialIO->available());
                code[0] = SerialIO->read();
                while (!SerialIO->available());
                code[1] = SerialIO->read();
                //Left
                if (code[0] == '[' && code[1] == 'D') {
                    terminal_bar.pos -= step;
                    break;
                //Right
                } else if (code[0] == '[' && code[1] == 'C') {
                    terminal_bar.pos += step;
                    break;
                }
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

