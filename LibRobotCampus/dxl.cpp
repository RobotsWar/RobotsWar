#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wirish/wirish.h>
#include "dxl.h"

volatile int dxl_debug = 0;

void dxl_packet_init(volatile struct dxl_packet *packet)
{
    packet->dxl_state = 0;
    packet->process = false;
}

/**
 * Writes the given packet to the buffer
 */
int dxl_write_packet(volatile struct dxl_packet *packet, ui8 *buffer)
{
    int i;
    unsigned int pos = 0;

    buffer[pos++] = 0xff;
    buffer[pos++] = 0xff;
    buffer[pos++] = packet->id;
    buffer[pos++] = packet->parameter_nb+2;
    buffer[pos++] = packet->instruction;

    for (i=0; i<packet->parameter_nb; i++) {
        buffer[pos++] = packet->parameters[i];
    }

    buffer[pos++] = dxl_compute_checksum(packet);

    return pos;
}

void dxl_copy_packet(volatile struct dxl_packet *from, volatile struct dxl_packet *to)
{
    memcpy((void *)to, (void *)from, sizeof(struct dxl_packet));
}

ui8 dxl_compute_checksum(volatile struct dxl_packet *packet) {
    int i;
    unsigned int sum = 0;

    sum += packet->id;
    sum += packet->instruction;
    sum += packet->parameter_nb+2;
    
    for (i=0; i<packet->parameter_nb; i++) {
        sum += packet->parameters[i];
    }

    sum = ~(sum & 0xFF);
    
    return (ui8) sum;
}

void dxl_packet_push_byte(volatile struct dxl_packet *packet, ui8 b)
{
    switch (packet->dxl_state) {
        case 0:
        case 1:
            if (b != 0xFF) {
                goto pc_error;
            }
            break;
        case 2:
            packet->id = b;
            break;
        case 3:
            packet->parameter_nb = b - 2;
            break;
        case 4:
            packet->instruction = b;
            break;
        default:
            if (packet->dxl_state - 4 > packet->parameter_nb) {
                goto pc_ended;

            } else {
                packet->parameters[packet->dxl_state - 5] = b;
            }

            if (packet->dxl_state - 4 > DXL_MAX_PARAMS) {
                goto pc_error;
            }
    }
        
    packet->dxl_state++;
    return;

pc_ended:
    if (dxl_compute_checksum(packet) == b) {
        packet->process = true;
    }

    packet->dxl_state = 0;
    return;
pc_error:
    packet->dxl_state = 0;
}

void dxl_init(int baudrate)
{
#if defined(BOARD_opencm904)
    afio_remap(AFIO_REMAP_USART1);
//    pinMode(DXL_DIRECTION, OUTPUT);
//    digitalWrite(DXL_DIRECTION, LOW); // RX
//    digitalWrite(DXL_DIRECTION, HIGH); // RX

    // Initializing pins
    gpio_set_mode(GPIOB, 6, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOB, 7, GPIO_INPUT_FLOATING);

    // Direction pins
    pinMode(DXL_DIRECTION, OUTPUT);
    digitalWrite(DXL_DIRECTION, LOW);

    DXL_DEVICE.begin(baudrate);
#endif
}

void dxl_send(volatile struct dxl_packet *packet)
{
    ui8 buffer[DXL_BUFFER_SIZE];
    int n = dxl_write_packet(packet, buffer);

    digitalWrite(DXL_DIRECTION, HIGH); // TX
    asm("nop");
    DXL_DEVICE.write(buffer, n);
    DXL_DEVICE.waitDataToBeSent();
    asm("nop");
    digitalWrite(DXL_DIRECTION, LOW); // RX
}

volatile struct dxl_packet in_packet;

void dxl_tick()
{
    while (DXL_DEVICE.available()) {
        dxl_packet_push_byte(&in_packet, DXL_DEVICE.read());
    }
}

void dxl_forward()
{
    while (true) {
        volatile struct dxl_packet current_packet;

        // Receiving packets
        dxl_tick();
        if (in_packet.process) {
            in_packet.process = false;
            ui8 buffer[DXL_BUFFER_SIZE];
            int n = dxl_write_packet(&in_packet, buffer);
            SerialUSB.write(buffer, n);
        }

        // Sending packets
        while (SerialUSB.available()) {
            dxl_packet_push_byte(&current_packet, SerialUSB.read());

            if (current_packet.process) {
                current_packet.process = false;
                dxl_send(&current_packet);
            }
        }
    }
}
