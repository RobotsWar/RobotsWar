#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wirish/wirish.h>
#include "terminal.h"

static char terminal_buffer[TERMINAL_BUFFER_SIZE];

static unsigned int terminal_size = 0;

static unsigned int terminal_pos = 0;

static const struct terminal_command *terminal_commands[TERMINAL_MAX_COMMANDS];

static unsigned int terminal_command_count = 0;

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

    SerialUSB.println("Available commands :");
    SerialUSB.println();

    for (i=0; i<terminal_command_count; i++) {
        const struct terminal_command *command = terminal_commands[i];

        SerialUSB.print(command->name);
        SerialUSB.print(": ");
        SerialUSB.print(command->description);
        SerialUSB.println();
    }
}

void terminal_prompt()
{
    SerialUSB.print("$ ");
}

void terminal_init()
{
    terminal_prompt();
}

/***
 * Executes the given command with given parameters
 */
void terminal_execute(char *command_name, unsigned int command_name_length, char *parameters)
{
    unsigned int i;
    char command_found = 0;

    for (i=0; i<terminal_command_count; i++) {
        const struct terminal_command *command = terminal_commands[i];

        if (strlen(command->name) == command_name_length && strncmp(terminal_buffer, command->name, command_name_length) == 0) {
            command->command(parameters);

            command_found = 1;
            break;
        }
    }

    if (!command_found) {
        SerialUSB.print("Unknwon command: ");
        for (i=0; i<command_name_length; i++) {
            SerialUSB.print((char)terminal_buffer[i]);
        }
        SerialUSB.println();
    }
}

/***
 * Process the receive buffer to parse the command and executes it
 */
void terminal_process()
{
    char *saveptr ,*parameters;
    unsigned int command_name_length;
    
    SerialUSB.println();

    strtok_r(terminal_buffer, " ", &saveptr);
    parameters = strtok_r(NULL, " ", &saveptr);
    
    if (saveptr != NULL) {
        *(saveptr - 1) = ' ';
    }
    
    if (parameters != NULL) {
        command_name_length = parameters - terminal_buffer - 1;
    } else {
        command_name_length = strlen(terminal_buffer);
    }

    if (command_name_length > 0) {
        terminal_execute(terminal_buffer, command_name_length, parameters);
    }

    terminal_pos = 0;
    terminal_size = 0;
    terminal_prompt();
}

/**
 * Ticking the terminal, this will cause lookup for characters and eventually
 * a call to the process function on new lines
 */
void terminal_tick()
{
    char c;
    uint8 input;

    while (SerialUSB.available()) {
        input = SerialUSB.read();
        c = (char)input;

        if (c == '\r' || c == '\n') {
            terminal_buffer[terminal_pos] = '\0';
            terminal_process();
        } else if (c == '\x7f') {
            if (terminal_pos > 0) {
                terminal_pos--;
                terminal_size--;
                SerialUSB.print("\x8 \x8");
            }
        } else {
            terminal_buffer[terminal_pos] = c;
            SerialUSB.print(c);

            if (terminal_pos < TERMINAL_BUFFER_SIZE-1) {
                terminal_pos++;
            }

            if (terminal_size < terminal_pos) {
                terminal_size = terminal_pos;
            }
        }
    }
}
