#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wirish/wirish.h>
#include "terminal.h"
#include "dxl.h"

#ifdef DXL_VERSION_2
#include "crc16.cpp"
#endif

#define DXL_WRITE_DELAY 5

volatile static bool initialized = false;
volatile static unsigned int dxl_timeout;
struct dxl_packet incoming_packet;
struct dxl_config dxl_configs[DXL_MAX_ID];

// Initializes a packet
void dxl_packet_init(struct dxl_packet *packet)
{
    packet->crc16 = 0;
    packet->dxl_state = 0;
    packet->process = false;
}

__attribute__((constructor))
void dxl_init_configs()
{
    for (int id=0; id<DXL_MAX_ID; id++) {
        dxl_configs[id].configured = false;
        dxl_configs[id].zero = 0.0;
        dxl_configs[id].min = -150;
        dxl_configs[id].max = 150;
        dxl_configs[id].dirty = false;
    }
}

#ifdef DXL_VERSION_1
/**
 * Writes the given packet to the buffer
 */
int dxl_write_packet(struct dxl_packet *packet, ui8 *buffer)
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
#endif

#ifdef DXL_VERSION_2
/**
 * Writes the given packet to the buffer
 */
int dxl_write_packet(struct dxl_packet *packet, ui8 *buffer)
{
    unsigned int i;
    unsigned int pos = 0;
    unsigned int length;

    buffer[pos++] = 0xff;
    buffer[pos++] = 0xff;
    buffer[pos++] = 0xfd;
    buffer[pos++] = 0x00;
    buffer[pos++] = packet->id;
    length = pos;
    pos += 2;
    buffer[pos++] = packet->instruction;

    // Stuffing
    int ff = 0;
    int stuffing = 0;
    for (i=0; i<packet->parameter_nb; i++) {
        if (packet->parameters[i] == 0xfd && ff>=2) {
            ff = 0;
            // 0xff 0xff 0xfd becomes 0xff 0xff 0xfd 0xfd
            buffer[pos++] = 0xfd;
            buffer[pos++] = 0xfd;
            stuffing++;
        } else {
            if (packet->parameters[i] == 0xff) {
                ff++;
            } else {
                ff = 0;
            }
            buffer[pos++] = packet->parameters[i];
        }
    }
    
    buffer[length] = (packet->parameter_nb+3+stuffing)&0xff;
    buffer[length+1] = ((packet->parameter_nb+3+stuffing)>>8)&0xff;

    unsigned short crc16 = update_crc(0, buffer, pos);
    buffer[pos++] = crc16&0xff;
    buffer[pos++] = (crc16>>8)&0xff;

    return pos;
}
#endif

void dxl_copy_packet(struct dxl_packet *from, struct dxl_packet *to)
{
    memcpy((void *)to, (void *)from, sizeof(struct dxl_packet));
}

ui8 dxl_compute_checksum(struct dxl_packet *packet) {
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

#ifdef DXL_VERSION_1
void dxl_packet_push_byte(struct dxl_packet *packet, ui8 b)
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
#endif

#ifdef DXL_VERSION_2
static int dxl_unstuff(unsigned char *packet, int n)
{
    int pos = 0;
    int ff = 0;
    int ff_fd = 0;
    int k = 0;
    for (int i=0; i<n; i++) {
        bool copy = true;
        if (ff_fd) {
            ff_fd = 0;
            if (packet[i] == 0xfd) {
                k++;
                copy = false;
            }   
        }   
        if (ff >= 2 && packet[i] == 0xfd) {
            ff_fd++;
        }   
        if (packet[i] == 0xff) {
            ff++;
        } else {
            ff = 0;
        }   
     
        if (k == 0) {
            pos++;
        } else {
            if (copy) {
                packet[pos++] = packet[i];
            }   
        }   
    }   

    return n-k;
}

void dxl_packet_push_byte(struct dxl_packet *packet, ui8 b)
{
    switch (packet->dxl_state) {
        case 0:
        case 1:
            if (b != 0xff) {
                goto pc_error;
            }
            break;
        case 2:
            if (b != 0xfd) {
                goto pc_error;
            }
            break;
        case 3:
            if (b != 0x00) {
                goto pc_error;
            }
            break;
        case 4:
            packet->id = b;
            break;
        case 5:
            packet->parameter_nb = b;
            break;
        case 6:
            packet->parameter_nb += (b<<8);
            packet->parameter_nb -= 3;
            break;
        case 7:
            packet->instruction = b;
            break;
        case 0x10000:
            packet->crc16 -= b&0xff;
            break;
        case 0x10001:
            packet->crc16 -= (b<<8)&0xff00;
            goto pc_ended;
            break;
        default:
            packet->parameters[packet->dxl_state - 8] = b;

            if (packet->dxl_state - 8 > DXL_MAX_PARAMS) {
                goto pc_error;
            }

            if (packet->dxl_state-7 >= packet->parameter_nb) {
                packet->dxl_state = 0xffff;
            }

            break;
    }

    if (packet->dxl_state < 0x10000) {
        packet->crc16 = update_crc(packet->crc16, &b, 1);
    }
        
    packet->dxl_state++;
    return;

pc_ended:
    if (packet->crc16 == 0) {
        packet->parameter_nb = dxl_unstuff(packet->parameters, packet->parameter_nb);
        packet->process = true;
    }

    packet->crc16 = 0;
    packet->dxl_state = 0;
    return;
pc_error:
    packet->crc16 = 0;
    packet->dxl_state = 0;
}
#endif

void dxl_init(int baudrate)
{
#if defined(DXL_AVAILABLE)
    dxl_timeout = 10000000/baudrate;
    initialized = true;
    afio_remap(AFIO_REMAP_USART1);

    // Initializing pins
    gpio_set_mode(GPIOB, 6, GPIO_AF_OUTPUT_PP);
    gpio_set_mode(GPIOB, 7, GPIO_INPUT_FLOATING);

    // Direction pins
    pinMode(DXL_DIRECTION, OUTPUT);
    digitalWrite(DXL_DIRECTION, LOW);

    DXL_DEVICE.begin(baudrate);
    dxl_disable_all();
#endif
}

void dxl_write_serial(ui8 *buffer, int n)
{
#if defined(DXL_AVAILABLE)
    // Flushing incoming data
    while (DXL_DEVICE.available()) {
        char dummy = DXL_DEVICE.read();
        (void)dummy;
    }

    // Sending packet
    digitalWrite(DXL_DIRECTION, HIGH); // TX
    asm("nop");
    DXL_DEVICE.write(buffer, n);
    DXL_DEVICE.waitDataToBeSent();
    asm("nop");
    digitalWrite(DXL_DIRECTION, LOW); // RX
#endif
}

// Sends a packet to the dynamixel bus
void dxl_send(struct dxl_packet *packet)
{
    if (!initialized) {
        return;
    }

    ui8 buffer[DXL_BUFFER_SIZE];
    int n = dxl_write_packet(packet, buffer);

    dxl_write_serial(buffer, n);

    incoming_packet.process = false;
}

// Wait and get the reply of a request
struct dxl_packet *dxl_get_reply()
{
    if (!initialized) {
        return NULL;
    }

    int now = millis();

    while ((millis()-now) < dxl_timeout) {
        dxl_tick();
        if (incoming_packet.process) {
            return &incoming_packet;
        }
    }

    // Timeout without any reply
    return NULL;
}

// Sends a request and get the reply
struct dxl_packet *dxl_send_reply(struct dxl_packet *request)
{
    dxl_send(request);

    return dxl_get_reply();
}

// Tick, reading the incoming packet from the dynamixel device
void dxl_tick()
{
#if defined(DXL_AVAILABLE)
    if (initialized) {
        while (DXL_DEVICE.available()) {
            ui8 c = DXL_DEVICE.read();
            dxl_packet_push_byte(&incoming_packet, c);
        }
    }
#endif
}

// Forwarding USB to Dynamixel
void dxl_forward()
{
#if defined(DXL_AVAILABLE)
    while (true && initialized) {
        // struct dxl_packet current_packet;

        // Receiving packets
        /*
        dxl_tick();
        if (incoming_packet.process) {
            incoming_packet.process = false;
            ui8 buffer[DXL_BUFFER_SIZE];
            int n = dxl_write_packet(&incoming_packet, buffer);
            SerialUSB.write(buffer, n);
        }
        */
        while (DXL_DEVICE.available()) {
            SerialUSB.write(DXL_DEVICE.read());
        }

        // Sending packets
        ui8 buffer[128];
        unsigned int n = 0;
        while (SerialUSB.available() && n<sizeof(buffer)) {
            buffer[n++] = SerialUSB.read();
            /*
            dxl_packet_push_byte(&current_packet, SerialUSB.read());

            if (current_packet.process) {
                current_packet.process = false;
                dxl_send(&current_packet);
            }
            */
        }

        if (n) {
            dxl_write_serial(buffer, n);
        }
    }
#endif
}

// Pings a servo and returns true if it's alive
bool dxl_ping(ui8 id)
{
    struct dxl_packet request;
    request.id = id;
    request.instruction = DXL_CMD_PING;
    request.parameter_nb = 0;

    struct dxl_packet *reply = dxl_send_reply(&request);

    return (reply!=NULL) && reply->id==id;
}

// Write some data to a dynamixel servo
void dxl_write(ui8 id, ui8 addr, char *data, int size)
{
    int pos = 0;
    struct dxl_packet request;
    request.id = id;
    request.instruction = DXL_CMD_WRITE;
#ifdef DXL_VERSION_1
        request.parameters[pos++] = addr;
#endif
#ifdef DXL_VERSION_2
        request.parameters[pos++] = addr&0xff;
        request.parameters[pos++] = (addr>>8)&0xff;
#endif

    for (int i=0; i<size; i++) {
        request.parameters[pos++] = data[i];
    }
    
    request.parameter_nb = pos;

    dxl_send(&request);
}

// Write a single byte to a dynamixel servo
void dxl_write_byte(ui8 id, ui8 addr, ui8 value)
{
    dxl_write(id, addr, (char*)&value, 1);
}

void dxl_write_word(ui8 id, ui8 addr, int value)
{
    ui8 buffer[2];
    buffer[0] = (value)&0xff;
    buffer[1] = (value>>8)&0xff;
    dxl_write(id, addr, (char*)buffer, sizeof(buffer));
}

static volatile bool dxl_is_async = false;

static int dxl_order_to_value(ui8 id, float position)
{
    if (id < DXL_MAX_ID && id != 0) {
        struct dxl_config *config = &dxl_configs[id-1];
        position += config->zero;
        if (position < config->min) position = config->min;
        if (position > config->max) position = config->max;
    }

    return dxl_position_to_value(id, position);
}

void dxl_async(bool async)
{
    dxl_is_async = async;
}

void dxl_flush()
{
    bool hasDirty = true;

    while (hasDirty) {
        hasDirty = false;
        struct dxl_packet request;
        int n = 0;

#ifdef DXL_VERSION_1
#define DXL_OFFSET 2
#endif
#ifdef DXL_VERSION_2
#define DXL_OFFSET 4
#endif

        for (ui8 id=1; id<=DXL_MAX_ID && n<10; id++) {
            struct dxl_config *config = &dxl_configs[id-1];
            if (config->dirty) {
                hasDirty = true;
                config->dirty = false;
                request.parameters[3*n+0+DXL_OFFSET] = id;
                request.parameters[3*n+1+DXL_OFFSET] = config->position&0xff;
                request.parameters[3*n+2+DXL_OFFSET] = (config->position>>8)&0xff;
                n++;
            }
        }

        if (hasDirty) {
            request.id = DXL_BROADCAST;
            request.instruction = DXL_CMD_SYNC_WRITE;
            request.parameter_nb = 4+3*n;
#ifdef DXL_VERSION_1
            request.parameters[0] = DXL_GOAL_POSITION;
            request.parameters[1] = 2;
#endif
#ifdef DXL_VERSION_2
            request.parameters[0] = DXL_GOAL_POSITION;
            request.parameters[1] = 0;
            request.parameters[2] = 2;
            request.parameters[3] = 0;
#endif

            dxl_send(&request);
            delay(DXL_WRITE_DELAY);
        }
    }
}

void dxl_set_position(ui8 id, float position)
{
    int value = dxl_order_to_value(id, position);
    if (id < DXL_MAX_ID && id != 0) {
        struct dxl_config *config = &dxl_configs[id-1];
        if (config->position != value && dxl_is_async) {
            config->dirty = true;
        }
        config->position = value;
    }

    if (!dxl_is_async) {
        dxl_write_word(id, DXL_GOAL_POSITION, value);
    }
}

float dxl_get_target_position(ui8 id)
{
    if (id < DXL_MAX_ID && id != 0) {
        struct dxl_config *config = &dxl_configs[id-1];
        return dxl_value_to_position(id, config->position);
    }

    return 0.0;
}

void dxl_disable(ui8 id)
{   
    dxl_write_word(id, DXL_GOAL_TORQUE, 0);
    delay(DXL_WRITE_DELAY);
    dxl_write_byte(id, DXL_LED, 1);
    delay(DXL_WRITE_DELAY);
}

void dxl_enable(ui8 id, int torque)
{   
    dxl_write_word(id, DXL_GOAL_SPEED, 1023);
    delay(DXL_WRITE_DELAY);
    dxl_write_word(id, DXL_GOAL_TORQUE, torque);
    delay(DXL_WRITE_DELAY);
    dxl_write_byte(id, DXL_LED, 2);
}

int dxl_position_to_value(ui8 id, float position)
{
    return ((position/300.0)*1024)+512;
}

float dxl_value_to_position(ui8 id, int value)
{
    return ((value-512)/1024.0)*300.0;
}

float dxl_get_position(ui8 id, bool *success)
{
    int value = dxl_read_word(id, DXL_POSITION, success);
    return dxl_value_to_position(id, value);
}

bool dxl_read(ui8 id, ui8 addr, char *output, int size)
{
    struct dxl_packet request;
    request.id = id;
    request.instruction = DXL_CMD_READ;
#ifdef DXL_VERSION_1
        request.parameter_nb = 2;
        request.parameters[0] = addr;
        request.parameters[1] = size;
#endif
#ifdef DXL_VERSION_2
        request.parameter_nb = 4;
        request.parameters[0] = addr&0xff;
        request.parameters[1] = (addr>>8)&0xff;
        request.parameters[2] = size&0xff;
        request.parameters[3] = (size>>8)&0xff;
#endif

    struct dxl_packet *reply = dxl_send_reply(&request);

    if (reply != NULL) {
        int offset = 0;
#ifdef DXL_VERSION_2
        offset = 1;
#endif
        for (int i=0; i<reply->parameter_nb; i++) {
            output[i] = reply->parameters[i+offset];
        }
    }

    return (reply != NULL);
}

ui8 dxl_read_byte(ui8 id, ui8 addr, bool *success)
{
    bool dummy;
    ui8 value;
    success = (success != NULL) ? success : &dummy;
    *success = dxl_read(id, addr, (char*)&value, 1);
    return value;
}

int dxl_read_word(ui8 id, ui8 addr, bool *success)
{
    bool dummy;
    ui8 buffer[2];
    success = (success != NULL) ? success : &dummy;
    *success = dxl_read(id, addr, (char*)buffer, sizeof(buffer));

    return dxl_makeword(buffer[0], buffer[1]);
}

void dxl_set_zero(ui8 id, float zero)
{
    if (id < DXL_MAX_ID && id != 0) {
        dxl_configs[id-1].configured = true;
        dxl_configs[id-1].zero = zero;
    }
}

void dxl_set_min_max(ui8 id, float min, float max)
{
    if (id < DXL_MAX_ID && id != 0) {
        dxl_configs[id-1].configured = true;
        dxl_configs[id-1].min = min;
        dxl_configs[id-1].max = max;
    }
}

struct dxl_config *dxl_get_config(ui8 id)
{
    if (id < DXL_MAX_ID && id != 0) {
        return &dxl_configs[id-1];
    } else {
        return NULL;
    }
}

void dxl_disable_all()
{
    dxl_write_word(DXL_BROADCAST, DXL_GOAL_TORQUE, 0);
    delay(1);
    dxl_write_byte(DXL_BROADCAST, DXL_LED, 1);
}

void dxl_wakeup(int steps)
{
    dxl_write_byte(DXL_BROADCAST, DXL_LED, 2);

    for (int k=1; k<=steps; k++) {
        int timeToSleep = 50;
        for (int id=0; id<DXL_MAX_ID; id++) {
            struct dxl_config *config = dxl_get_config(id);
            if (config!=NULL && config->configured) {
                dxl_enable(id, (1023*k)/steps);
                delay(DXL_WRITE_DELAY);
                timeToSleep -= DXL_WRITE_DELAY;
            }
        }
        if (timeToSleep > 0) {
            delay(timeToSleep);
        }
    }
}

float dxl_average_voltage()
{
    bool success;
    ui8 value;
    float voltage = 0.0;
    int n = 0;

    for (int id=1; id<=DXL_MAX_ID; id++) {
        value = dxl_read_byte(id, DXL_VOLTAGE, &success);
        if (success) {
            voltage += (value/10.0);
            n++;
        }
    }

    return voltage/n;
}

int dxl_makeword(ui8 a, ui8 b)
{
    return a|(b<<8);
}

void dxl_compliance_slope(int slope)
{
    for (int id=1; id<DXL_MAX_ID; id++) {
        dxl_write_byte(id, DXL_COMPLIANCE_CW, slope);
        delay(DXL_WRITE_DELAY);
        dxl_write_byte(id, DXL_COMPLIANCE_CCW, slope);
        delay(DXL_WRITE_DELAY);
    }
}

void dxl_compliance_margin(int margin)
{
    for (int id=1; id<DXL_MAX_ID; id++) {
        dxl_write_byte(id, DXL_COMPLIANCE_MARGIN_CW, margin);
        delay(DXL_WRITE_DELAY);
        dxl_write_byte(id, DXL_COMPLIANCE_MARGIN_CCW, margin);
        delay(DXL_WRITE_DELAY);
    }
}

void dxl_configure_all()
{
    for (int i=0; i<5; i++) {
        delay(DXL_WRITE_DELAY);
        dxl_write_byte(DXL_BROADCAST, DXL_RETURN_DELAY, 0);
        delay(DXL_WRITE_DELAY);
        dxl_write_byte(DXL_BROADCAST, DXL_RETURN_LEVEL, 1);
    }
}

void dxl_configure(int id, int newId)
{
    dxl_write_byte(id, DXL_ID, newId);

    for (int i=0; i<5; i++) {
        delay(DXL_WRITE_DELAY);
        dxl_write_byte(newId, DXL_RETURN_DELAY, 0);
        delay(DXL_WRITE_DELAY);
        dxl_write_byte(newId, DXL_RETURN_LEVEL, 1);
    }
}
