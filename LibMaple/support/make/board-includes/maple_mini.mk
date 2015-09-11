MCU := STM32F103CB
PRODUCT_ID := 0003
ERROR_LED_PORT := GPIOB
ERROR_LED_PIN  := 1
MCU_SERIES := stm32f1
MCU_F1_LINE := performance
ifeq ($(BOOTLOADER),maple)
LD_MEM_DIR := sram_20k_flash_128k
endif
ifeq ($(BOOTLOADER),robotis)
LD_MEM_DIR := sram_20k_flash_128k_robotis
endif
