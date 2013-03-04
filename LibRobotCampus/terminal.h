#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <wirish/wirish.h>
#include <wirish/Serial.h>
#include <wirish/Print.h>

class TerminalIO : public Print
{
    public:
        TerminalIO();
        virtual void write(uint8 ch);
        virtual void write(const void *buf, uint32 len);

        void setIO(Serial *io_);
        bool hasIO();

        Serial *io;
        bool silent;
};

/**
 * Maximum length of a command line
 * and its argument
 */
#define TERMINAL_BUFFER_SIZE 64

/**
 * Maximum number of command arguments
 */
#define TERMINAL_MAX_ARGUMENTS 8

/**
 * Maximum number of commands
 * which ca be registered
 */
#define TERMINAL_MAX_COMMANDS 50

/**
 * The number of terminal bar step
 */
#define TERMINAL_BAR_STEP 50

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
 * use the variables arc and argv to get the string of given arguments
 *
 * Example :
 *
 * TERMINAL_COMMAND(hello, "Print a friendly warming welcoming message")
 * {
 *     if (argc > 0) {
 *         terminal_io()->print("Hello ");
 *         terminal_io()->println(argv[0]);
 *         terminal_io()->print("Other params: ");
 *         for (unsigned i=1;i<argc;i++) {
 *             terminal_io()->print(argv[i]);
 *             terminal_io()->print(" ");
 *         }
 *         terminal_io()->println();
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
 * Returns the Print and Read instance used 
 * to write on the serial port
 */
TerminalIO* terminal_io();

/**
 * Initialize the termanl bar structure
 * @param min : minimum value
 * @param max : maximum value
 * @param pos : initial position
 */
void terminal_bar_init(int min, int max, int pos);

/**
 * Terminal bar ticking
 * Fetch the serial port and handle the bar
 * @return the bar position
 */
int terminal_bar_tick();

/**
 * Returns true if the bar session
 * have been interrupted
 */
bool terminal_bar_escaped();

/**
 * Mute the terminal
 */
void terminal_silent(bool silent);

/**
 * ----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 * ----------------------------------------------------------------------------
 */

/**
 * Prototype of a terminal command
 */
typedef void terminal_command_fn(unsigned int argc, char *argv[]);

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
    void terminal_command_ ## name (unsigned int argc, char *argv[])

#endif // _TERMINAL_H

