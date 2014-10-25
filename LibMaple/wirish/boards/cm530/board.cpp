/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2011 LeafLabs, LLC.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/**
 * @file   maple.cpp
 * @author Marti Bolivar <mbolivar@leaflabs.com>
 * @brief  Maple PIN_MAP and boardInit().
 */
/*
 *  CM904.cpp ported from CM900.cpp
 *
 *  Created on: 2013. 5. 22.
 *      Author: in2storm
 */
/*!
 * ported to CM530 by Fabien R <theedge456@free.fr>
 */

#include <board/board.h>         // For this board's header file

#include <wirish/wirish_types.h> // For stm32_pin_info and its contents
                                 // (these go into PIN_MAP).

#include "boards_private.h"      // For PMAP_ROW(), which makes


void boardInit(void) {

	afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY); //[ROBOTIS] 2013-07-17

	//[ROBOTIS][CHANGE] add here if you want to initialize something
    gpio_set_mode(GPIOC, 13, GPIO_OUTPUT_PP);
    gpio_write_bit(GPIOB, 13,1); //LED0
}

extern const stm32_pin_info PIN_MAP[BOARD_NR_GPIO_PINS] = {
    {GPIOC,   NULL, NULL,  13, 0,   ADCx}, /* PC13 */
    {GPIOC,   NULL, NULL,  14, 0,   ADCx}, /* PC14 */
    {GPIOC,   NULL, NULL,  15, 0,   ADCx}, /* PC15 */
    {GPIOC,   NULL, ADC1,   0, 0,   0},    /* PC0 */
    {GPIOC,   NULL, ADC1,   1, 0,   1},    /* PC1 */
    {GPIOC,   NULL, ADC1,   2, 0,   2},    /* PC2 */
    {GPIOC,   NULL, ADC1,   4, 0,   9},    /* PC4 */
    {GPIOC,   TIMER3, NULL, 6, 1,ADCx},    /* PC6 */
    {GPIOC,   TIMER3, NULL, 7, 2,ADCx},    /* PC7 */
    {GPIOC,   TIMER3, NULL, 8, 3,ADCx},    /* PC8 */

    {GPIOC,   TIMER3, NULL, 9, 4,ADCx},    /* PC9 */
    {GPIOC,   NULL, NULL, 10, 0,ADCx},    /* PC10 */
    {GPIOC,   NULL, NULL, 11, 0,ADCx},    /* PC11 */
    {GPIOC,   NULL, NULL, 12, 0,ADCx},    /* PC12 */
    {GPIOD,   NULL, NULL, 2, 0,ADCx},    /* PD2 */
    {GPIOA,   TIMER2, ADC1, 0, 1,   3},    /* PA0 */
    {GPIOA,   TIMER2, ADC1, 1, 2,   4},    /* PA1 */
    {GPIOA,   TIMER2, ADC1, 2, 3,   5},    /* PA2 */
    {GPIOA,   TIMER2, ADC1, 3, 4,   6},    /* PA3 */
    {GPIOA,   NULL, ADC1, 5, 0,     7},    /* PA5 */
    {GPIOA,   NULL, ADC1, 6, 0,     8},    /* PA6 */

    {GPIOA,   TIMER1, NULL, 8,1, ADCx},    /* PA8 */    
    {GPIOA,   TIMER1, NULL, 11,4, ADCx},   /* PA11 */
    {GPIOA,   TIMER1, NULL, 12,1, ADCx},   /* PA12 */
    {GPIOA,   NULL, NULL, 14,0,   ADCx},   /* PA14 */
    {GPIOA,   TIMER2, NULL, 15,1, ADCx},   /* PA15 */
    {GPIOB,   TIMER2, NULL, 3,  2,  ADCx},    /* PB3 */
    {GPIOB,   TIMER3, NULL, 4,  1,  ADCx},    /* PB4 */
    {GPIOB,   TIMER3, NULL, 5,  2,  ADCx},    /* PB5 */
    {GPIOB,   TIMER4, NULL, 6,  1,  ADCx},    /* PB6 */
    {GPIOB,   TIMER4, NULL, 7,  2,  ADCx},    /* PB7 */

    {GPIOB,   TIMER4, NULL, 8,  3,  ADCx},    /* PB8 */    
    {GPIOB,   TIMER4, NULL, 9,  4,  ADCx},    /* PB9 */    
    {GPIOB,   TIMER2, NULL, 10, 3,  ADCx},    /* PB10 */
    {GPIOB,   TIMER2, NULL, 11, 4,  ADCx},    /* PB11 */
    {GPIOB,   TIMER1, NULL, 12, 1,  ADCx},    /* PB12 */
    {GPIOB,   TIMER1, NULL, 13, 4,  ADCx},    /* PB13 */
    {GPIOB,   TIMER1, NULL, 14, 2,  ADCx},    /* PB14 */
    {GPIOB,   TIMER1, NULL, 15, 3,  ADCx},    /* PB15 */

/*
 * Hidden pin map
 * the below pins are used carefully, need to check schematic of CM530
 * */

};
// a verifier
extern const uint8 boardPWMPins[] __FLASH__ = {
    4, 5, 6, 7,
    15, 16, 17, 18, 19, 20
};

// a verifier
extern const uint8 boardADCPins[] __FLASH__ = {
    3, 4, 5, 6,
    15, 16, 17, 18, 19, 20
};

extern const uint8 boardUsedPins[] __FLASH__ = {
    BOARD_LED_PIN, BOARD_LED_AUX_PIN
};

/*!
 * pin map (cf http://support.robotis.com/en/software/embeded_c/cm530/programming/hardware_port_map_cm530.htm)
 * Fin number |Pin name| NET NAME	|Function description
 * 1 	        Vbat 	  VCC_33L 	  3.3V power supply
 * 2 	        PC13 	  LED0 	  LED POWER
 * 3 	        PC14 	  LED_TX 	  LED TX
 * 4 	        PC15 	  LED_RX 	  LED RX
 * 5 	        OSC_IN 	  XTAL1 	  X-TAL INPUT
 * 6 	        OSC_OUT 	  XTAL2 	  X-TAL OUTPUT
 * 7 	        NRST 	  RESET 	  RESET
 * 8 	        PC0 	  SIG_ADC0 	  ADC0 INPUT
 * 9 	        PC1 	  ADC_SELECT0 	  ANALOG MUX SELECT0
 * 10 	        PC2 	  ADC_SELECT1 	  ANALOG MUX SELECT1
 * 11 	        PC3 	  VDD_VOLT 	  input voltage  ADC
 * 12 	        VSSA 	  VSSA 	  ADC reference voltage  3.3V
 * 13 	        VDDA 	  VDDA 	  ADC reference voltage  0V
 * 14 	        PA0 	  SIG_MOT1+ 	  #1 external port  OUPUT
 * 15 	        PA1 	  SIG_MOT1- 	  #1 external port  OUPUT
 * 16 	        PA2 	  SIG_MOT2+ 	  #2 external port  OUPUT
 * 17 	        PA3 	  SIG_MOT2- 	  #2 external port  OUPUT
 * 18 	        VSS4 	  GND 	  0V reference voltage
 * 19 	        VDD4 	  VCC_33L 	  3.3V reference voltage
 * 20 	        PA4 	   NC 	  RESERVED
 * 21 	        PA5 	  SIG_ADC1 	  ADC1 INPUT
 * 22 	        PA6 	  SIG_BUZZER 	  Buzzer OUTPUT
 * 23 	        PA7 	  NC 	  RESERVED
 * 24 	        PC4 	  SIG_MIC 	  MIC Signal INPUT
 * 25 	        PC5 	  NC 	  RESERVED
 * 26 	        PB0 	  NC 	  RESERVED
 * 27 	        PB1 	  NC 	  RESERVED
 * 28 	        PB2 	  BOOT1 	  RESERVED
 * 29 	        PB10 	  PC_TXD 	  PC transmit
 * 30 	        PB11 	  PC_RXD 	  PC receive
 * 31 	        VSS1 	  GND 	  0V reference voltage
 * 32 	        VDD1 	  VCC_33L 	  3.3V reference voltage
 * 33 	        PB12 	  LED3 	  LED AUX
 * 34 	        PB13 	   LED4 	  LED MANAGE
 * 35 	        PB14 	  LED5 	  LED PROGRAM
 * 36 	        PB15 	  LED6 	  LED PLAY
 * 37 	        PC6 	  SIG_MOT3+ 	  #3 external port  OUPUT
 * 38 	        PC7 	  SIG_MOT3- 	  #3 external port  OUPUT
 * 39 	        PC8 	  SIG_MOT4+ 	  #4 external port  OUPUT
 * 40 	        PC9 	  SIG_MOT4- 	  #4 external port  OUPUT
 * 41 	        PA8 	  SIG_MOT5+ 	  #5 external port  OUPUT
 * 42 	        PA9 	   NC 	  RESERVED
 * 43 	        PA10 	   NC 	  RESERVED
 * 44 	        PA11 	  SIG_MOT5- 	  #5 external port OUPUT
 * 45 	        PA12 	  ZIGBEE_RESET 	  Zigbee ENABLE
 * 46 	        PA13 	  USB_SLEEP 	  USB connections
 * 47 	        VSS2 	  GND 	  0V reference voltage
 * 48 	        VDD2 	  VCC_33L 	  3.3V reference voltage
 * 49 	        PA14 	  SW_RT 	  R Button
 * 50 	        PA15 	  SW_LF 	  L Button
 * 51 	        PC10 	  SW_DN 	  D Button
 * 52 	        PC11 	  SW_UP 	  U Button
 * 53 	        PC12 	  ZIGBEE_TXD 	  Zigbee transmit
 * 54 	        PD2 	  ZIGBEE_RXD 	  Zigbee receive
 * 55 	        PB3 	  SW_START 	  START Button
 * 56 	        PB4 	  ENABLE_TX 	  Dynamixel transmit  ENABLE
 * 57 	        PB5 	  ENABLE_RX 	  Dynamixel receive ENABLE
 * 58 	        PB6 	  DXL_TXD 	  Dynamixel transmit
 * 59 	        PB7 	  DXL_RXD 	  Dynamixel receive
 * 60 	        BOOT0 	  BOOT0 	  RESERVED
 * 61 	        PB8 	  SIG_MOT6+ 	  #6 external port OUPUT
 * 62 	        PB9 	  SIG_MOT6- 	  #6 external port OUPUT
 * 63 	        VSS3 	  GND 	  0V reference voltage
 * 64 	        VDD3 	  VCC_33L 	  3.3V reference voltage
 */
