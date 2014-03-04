#ifndef DXL_H
#define DXL_H

// Protocol definition
#define DXL_BROADCAST   0xFE

// Size limit for a buffer containing a dynamixel packet
#define DXL_BUFFER_SIZE 300

// Maximum parameters in a packet
#define DXL_MAX_PARAMS 140

// Direction pin
#if defined(BOARD_opencm904)
// 1: TX
// 0: RX
#define DXL_DIRECTION  28
#define DXL_DEVICE     Serial1
#define DXL_AVAILABLE
#endif

typedef unsigned char ui8;

/**
 * A dynamixel packet
 */
struct dxl_packet {
    ui8 id; 
    union {
        ui8 instruction;
        ui8 error;
    };  
    ui8 parameter_nb;
    ui8 parameters[DXL_MAX_PARAMS];
    bool process;
    ui8 dxl_state;
};

void dxl_packet_init(volatile struct dxl_packet *packet);
void dxl_packet_push_byte(volatile struct dxl_packet *packet, ui8 b);
int dxl_write_packet(volatile struct dxl_packet *packet, ui8 *buffer);
void dxl_copy_packet(volatile struct dxl_packet *from, volatile struct dxl_packet *to);
ui8 dxl_compute_checksum(volatile struct dxl_packet *packet);

void dxl_send(volatile struct dxl_packet *packet);

void dxl_init(int baudrate);
void dxl_forward();

#endif // DXL_H
