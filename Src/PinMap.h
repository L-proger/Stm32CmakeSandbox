#pragma once

#include <stm32f7xx_hal.h>

#define LED_GPIO_Port GPIOE
#define LED_Pin LL_GPIO_PIN_0


#define LCD_CS_GPIO_Port GPIOD
#define LCD_CS_Pin LL_GPIO_PIN_0

#define LCD_DC_GPIO_Port GPIOD
#define LCD_DC_Pin LL_GPIO_PIN_1

#define LCD_RST_GPIO_Port GPIOD
#define LCD_RST_Pin LL_GPIO_PIN_2

#define LCD_SPI_INDEX 3
