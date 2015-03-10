#include <terminal.h>
#include <main.h>
#include <rhock/print.h>

void rhock_print(uint8_t c)
{
    if (is_rhock_mode()) {
        rhock_print_stream(c);
    } else {
        terminal_io()->write(c);
    }
}

void rhock_print_value(float value)
{
    if (is_rhock_mode()) {
        rhock_print_value_stream(value);
    } else {
        terminal_io()->print(value);
    }
}

void rhock_println()
{
    if (is_rhock_mode()) {
        rhock_println_stream();
    } else {
        terminal_io()->println();
    }
}
