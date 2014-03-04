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

// Registers
#define DXL_LED             0x19
#define DXL_GOAL_POSITION   0x1E
#define DXL_GOAL_SPEED      0x20
#define DXL_GOAL_TORQUE     0x22
#define DXL_POSITION        0x24
#define DXL_SPEED           0x26
#define DXL_TORQUE          0x28

// Instructions
#define DXL_CMD_PING    0x01
#define DXL_CMD_READ    0x02
#define DXL_CMD_WRITE   0x03

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

void dxl_packet_init(struct dxl_packet *packet);
void dxl_packet_push_byte(struct dxl_packet *packet, ui8 b);
int dxl_write_packet(struct dxl_packet *packet, ui8 *buffer);
void dxl_copy_packet(struct dxl_packet *from, struct dxl_packet *to);
ui8 dxl_compute_checksum(struct dxl_packet *packet);

// Send a packet on the dynamixel bus
void dxl_send(struct dxl_packet *packet);

// Initialize dynamixel system (if available)
void dxl_init(int baudrate);

// Run the dynamixel forward
void dxl_forward();

// Incoming packet
extern struct dxl_packet incoming_packet;

// Get the reply of last packet sent
struct dxl_packet *dxl_get_reply();

// Tick (read data from the bus)
void dxl_tick();

// Sends a packet and wait for the reply
struct dxl_packet *dxl_send_reply(struct dxl_packet *packet);

// Pings a servo with the given ID
bool dxl_ping(ui8 id);

// Write some data to a dynamixel servo
void dxl_write(ui8 id, ui8 addr, char *data, int size);
void dxl_write_byte(ui8 id, ui8 addr, ui8 value);
void dxl_write_word(ui8 id, ui8 addr, int value);

// Reading
bool dxl_read(ui8 id, ui8 addr, char *output, int size);
ui8 dxl_read_byte(ui8 id, ui8 addr, bool *success = NULL);
int dxl_read_word(ui8 id, ui8 addr, bool *success = NULL);

// Position
float dxl_get_position(ui8 id, bool *success = NULL);
void dxl_set_position(ui8 id, float position);
int dxl_position_to_value(ui8 id, float position);
float dxl_value_to_position(ui8 id, int value);

// Torque
void dxl_disable(ui8 id);
void dxl_enable(ui8 id);

#endif // DXL_H
