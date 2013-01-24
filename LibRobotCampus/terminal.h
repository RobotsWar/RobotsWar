#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <wirish/wirish.h>

/**
 * Maximum length of a command line
 * and its argument
 */
#define TERMINAL_BUFFER_SIZE 32

/**
 * Maximum number of commands
 * which ca be registered
 */
#define TERMINAL_MAX_COMMANDS 16

/**
 * Terminal prompt
 */
#define TERMINAL_PROMPT "$ "

/**
 * To defined a new command, use the MACRO
 * TERMINAL_COMMAND(name, description)
 *
 * @param name : the name of the command, without space, without quotes
 * @param description : the description of the command, with quote
 *
 * use terminal_io() to print on output
 * use the variable parameters to get the string of given arguments
 *
 * Example :
 *
 * TERMINAL_COMMAND(hello, "Print a friendly warming welcoming message")
 * {
 *     if (parameters != NULL) {
 *         terminal_io()->print("Hello ");
 *         terminal_io()->println(parameters);
 *     } else {
 *         terminal_io()->println("Hello world");
 *     }
 * }
 */

/**
 * Initializing terminal
 * This function have to be called before using
 * terminal_tick()
 *
 * @param com : you have to provide to terminal_init
 * an initialized instance of USBSerial or HardwareSerial
 */
void terminal_init(Serial *serial);

/**
 * Terminal ticking
 * Call this function in your main loop 
 * to fetch serial port and handle terminal
 */
void terminal_tick();

/**
 * Returns the Print instance used 
 * to write on the serial port
 */
Print* terminal_io();


/**
 * ----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 */

/**
 * Prototype of a terminal command
 */
typedef void terminal_command_fn(char *parameters);

/**
 * A command definition for the terminal
 */
struct terminal_command
{
    char *name;
    char *description;
    terminal_command_fn *command;
};

/**
 * Registers a command
 */
void terminal_register(const struct terminal_command *command);

#define TERMINAL_COMMAND(name, description) terminal_command_fn terminal_command_ ## name; \
    \
    char terminal_command_name_ ## name [] = #name; \
    char terminal_command_description_ ## name [] = description; \
    \
    struct terminal_command terminal_command_definition_ ## name = { \
        terminal_command_name_ ## name , \
        terminal_command_description_ ## name , \
        terminal_command_ ## name \
    }; \
    \
    __attribute__((constructor)) \
    void terminal_command_init_ ## name () {  \
        terminal_register(&terminal_command_definition_ ## name ); \
    } \
    \
    void terminal_command_ ## name (char *parameters)

#endif // _TERMINAL_H

