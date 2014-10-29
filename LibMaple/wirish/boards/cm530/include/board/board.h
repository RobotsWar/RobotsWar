/*
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


/*
 * CM530.h
 *
 *  Created on: 2013. 05. 22.
 *      Author: ROBOTIS[sm6787@robotis.com]
 *      ported from maple.h(leaflabs.com)
 *      @brief  Private include file for cm-904 in boards.h
 */
#ifndef CM530_H_
#define CM530_H_

//#include "gpio.h"


#define CYCLES_PER_MICROSECOND  72
#define SYSTICK_RELOAD_VAL      71999 /* takes a cycle to reload */
/*
 * [ROBOTIS][CHANGE]2013-04-22 CM-900 Do not have built-in button.
 * 2013-08-03 CM-9.04 has built-in button -> inserted it again.
 * */
#define BOARD_LED_PIN           0//PC13
#define BOARD_LED_AUX_PIN        35//PB12
#define BOARD_BUTTON_PIN        26//PB3
//#define BOOT1_PIN				28// added

/* Total number of GPIO pins that are broken out to headers and
 * intended for general use. */
#define BOARD_NR_GPIO_PINS      39

/* Number of pins capable of PWM output */
#define BOARD_NR_PWM_PINS       15

/* Number of pins capable of ADC conversion */
#define BOARD_NR_ADC_PINS       10

/* Number of pins already connected to external hardware.  For Maple,
 * these are just BOARD_LED_PIN and BOARD_BUTTON_PIN. */
#define BOARD_NR_USED_PINS       2

/* Number of USARTs/UARTs whose pins are broken out to headers */
#define BOARD_NR_USARTS         3

/* Default USART pin numbers (not considering AFIO remap) */
// Not used
#define BOARD_USART2_TX_PIN     4	//D2 (PA2)
#define BOARD_USART2_RX_PIN     5	//D3 (PA3)

/* Number of SPI ports */
#define BOARD_NR_SPI            2

/* Default SPI pin numbers (not considering AFIO remap) */
// Not used
#define BOARD_SPI1_NSS_PIN      0 //D10 (PA4)
#define BOARD_SPI1_MOSI_PIN     7 //D11 PA7
#define BOARD_SPI1_MISO_PIN     6 //D12 PA6
#define BOARD_SPI1_SCK_PIN      1 //D13 PA5
#define BOARD_SPI2_NSS_PIN      18 //D26 PB12
#define BOARD_SPI2_MOSI_PIN     21 //D29 PB15
#define BOARD_SPI2_MISO_PIN     20 //D28 PB14
#define BOARD_SPI2_SCK_PIN      19 //D27 PB13
#define BOARD_SPI3_NSS_PIN      18 //D26 PB12
#define BOARD_SPI3_MOSI_PIN     21 //D29 PB15
#define BOARD_SPI3_MISO_PIN     20 //D28 PB14
#define BOARD_SPI3_SCK_PIN      19 //D27 PB13

#define BOARD_USB_DISC_DEV      GPIOC
#define BOARD_USB_DISC_BIT      13

void boardInit(void);

// DXL
// Ok
#define BOARD_DYNAMIXEL_DIR_TX     27 //PB4
#define BOARD_DYNAMIXEL_DIR_RX     28 //PB5
#define BOARD_DYNAMIXEL_TX      29 //PB6
#define BOARD_DYNAMIXEL_RX      30 //PB7
#define BOARD_USART1_TX_PIN     BOARD_DYNAMIXEL_TX
#define BOARD_USART1_RX_PIN     BOARD_DYNAMIXEL_RX

// PC USB
// Ok
#define BOARD_PC_TX_PIN     33	//PB10
#define BOARD_PC_RX_PIN     34	//PB11
#define BOARD_USART3_TX_PIN   BOARD_PC_TX_PIN
#define BOARD_USART3_RX_PIN   BOARD_PC_RX_PIN

// Zigbee
// Ok
#define BOARD_ZIGBEE_RESET     23	//PA12
#define BOARD_ZIGBEE_TX_PIN     13	//PC12
#define BOARD_ZIGBEE_RX_PIN     14	//PD2
// Zigbee is on UART5
#define BOARD_UART5_TX_PIN     BOARD_ZIGBEE_TX_PIN
#define BOARD_UART5_RX_PIN     BOARD_ZIGBEE_RX_PIN

// Not tested
#define BOARD_JTMS_SWDIO_PIN      26
#define BOARD_JTCK_SWCLK_PIN      27
#define BOARD_JTDI_PIN            15
#define BOARD_JTDO_PIN            16
#define BOARD_NJTRST_PIN          17

#endif /* CM530_H_ */
