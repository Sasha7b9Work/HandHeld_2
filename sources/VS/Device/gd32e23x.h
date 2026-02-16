#pragma once


#define GPIOA             0
#define GPIOB             1
#define GPIO_MODE_INPUT   0
#define GPIO_PUPD_PULLUP  0
#define GPIO_PUPD_NONE    0
#define GPIO_PIN_2        0
#define GPIO_PIN_13       0
#define EXTI4_15_IRQn     0
#define EXTI_SOURCE_GPIOB 0
#define EXTI_SOURCE_PIN13 0
#define EXTI_13           0
#define EXTI_INTERRUPT    0
#define EXTI_TRIG_RISING  0


void gpio_mode_set(uint, uint, uint, uint);
void nvic_irq_enable(uint, uint);
void syscfg_exti_line_config(uint, uint);
void exti_init(uint, uint, uint);
void exti_interrupt_flag_clear(uint);
void nvic_irq_disable(uint);
