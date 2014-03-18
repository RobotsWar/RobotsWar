// WiFly routing
#if defined(BOARD_opencm904) || defined(BOARD_cm900)
#define WiFly Serial2
#else
#define WiFly Serial3
#endif
