#pragma once


#define GPIOA       0U
#define GPIOB       0U
#define GPIOC       0U

#define GPIO_PIN_1   1U
#define GPIO_PIN_2   3U
#define GPIO_PIN_3   3U
#define GPIO_PIN_4   3U
#define GPIO_PIN_5   3U
#define GPIO_PIN_7   3U
#define GPIO_PIN_10 13U
#define GPIO_PIN_11 13U
#define GPIO_PIN_13 13U

#define SPI0 0U

#define GPIO_MODE_AF_PP     0U
#define GPIO_OSPEED_50MHZ   0U 

void spi_i2s_data_transmit(uint, uint8);

void gpio_init(uint, uint, uint, uint);

void spi_i2s_deinit(uint);

struct spi_parameter_struct
{

};
