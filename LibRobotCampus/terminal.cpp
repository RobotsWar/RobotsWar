#include <stdlib.h>
#include <string.h>
#include "terminal.h"

static char terminal_buffer[TERMINAL_BUFFER_SIZE];

static unsigned int terminal_pos = 0;

static const struct terminal_command *terminal_commands[TERMINAL_MAX_COMMANDS];

static unsigned int terminal_command_count = 0;

static Serial *SerialIO = 0;

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
        terminal_io()->print(": ");
        terminal_io()->print(command->description);
        terminal_io()->println();
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
            SerialIO->print(c);

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

