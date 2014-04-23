// RC routing
#if defined(BOARD_opencm904) || defined(BOARD_cm900)
#define RC Serial2
#else
#define RC Serial3
#endif
