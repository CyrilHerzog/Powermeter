/*
 * gpio.h
 *
 *  Created on: 04.04.2023
 *      Author: Herzog Cyril
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <system.h>

/* update F-Serie <=> L-Serie
#define AHBx_IOPAx		(uint32_t) 0x020000
#define AHBx_IOPBx		(uint32_t) 0x040000
#define AHBx_IOPCx		(uint32_t) 0x080000
#define AHBx_IOPDx		(uint32_t) 0x100000
#define AHBx_IOPHx		(uint32_t) 0x400000
*/

#define IOPx_IOPAx      (uint32_t) 0x000001
#define IOPx_IOPBx      (uint32_t) 0x000002
#define IOPx_IOPCx      (uint32_t) 0x000004
#define IOPx_IOPDx      (uint32_t) 0x000008
#define IOPx_IOPEx      (uint32_t) 0x000010
#define IOPx_IOPHx      (uint32_t) 0x000080

typedef enum{
	GPIO_0  = 0x00,
	GPIO_1  = 0x01,
	GPIO_2  = 0x02,
	GPIO_3  = 0x03,
	GPIO_4  = 0x04,
	GPIO_5  = 0x05,
	GPIO_6  = 0x06,
	GPIO_7  = 0x07,
	GPIO_8  = 0x08,
	GPIO_9  = 0x09,
	GPIO_10 = 0x0A,
	GPIO_11 = 0x0B,
	GPIO_12 = 0x0C,
	GPIO_13 = 0x0D,
	GPIO_14 = 0x0E,
	GPIO_15 = 0x0F
}gpio_pin_t;


typedef enum{
	GPIO_PULL_MODE_NO_PULL  = 0,
	GPIO_PULL_MODE_PULL_UP  = 1,
	GPIO_PULL_MODE_PULLDOWN = 2
}gpio_pull_mode_t;

typedef enum{
	GPIO_OUTPUT_MODE_PUSH_PULL = 0,
	GPIO_OUTPUT_MODE_OPEN_DRAIN = 1
}gpio_output_mode_t;

#define GPIO_AF0				(uint32_t) 0x00
#define GPIO_AF1				(uint32_t) 0x01
#define GPIO_AF2				(uint32_t) 0x02
#define GPIO_AF3				(uint32_t) 0x03
#define GPIO_AF4				(uint32_t) 0x04
#define GPIO_AF5				(uint32_t) 0x05
#define GPIO_AF6				(uint32_t) 0x06
#define GPIO_AF7				(uint32_t) 0x07

typedef enum{
	GPIO_FUNC_MODE_INPUT  = 0,
	GPIO_FUNC_MODE_OUTPUT = 1,
	GPIO_FUNC_MODE_ANALOG = 3,
	GPIO_FUNC_MODE_AF0    = 2 | (GPIO_AF0 << 2),
	GPIO_FUNC_MODE_AF1    = 2 | (GPIO_AF1 << 2),
	GPIO_FUNC_MODE_AF2    = 2 | (GPIO_AF2 << 2),
	GPIO_FUNC_MODE_AF3    = 2 | (GPIO_AF3 << 2),
	GPIO_FUNC_MODE_AF4    = 2 | (GPIO_AF4 << 2),
	GPIO_FUNC_MODE_AF5    = 2 | (GPIO_AF5 << 2),
	GPIO_FUNC_MODE_AF6    = 2 | (GPIO_AF6 << 2),
	GPIO_FUNC_MODE_AF7    = 2 | (GPIO_AF7 << 2),
}gpio_func_mode_t;

typedef enum{
	GPIO_SPEED_MODE_LOW_SPEED       = 0,
	GPIO_SPEED_MODE_MEDIUM_SPEED    = 1,
	GPIO_SPEED_MODE_HIGH_SPEED      = 2,
	GPIO_SPEED_MODE_VERY_HIGH_SPEED = 3
}gpio_speed_mode_t;

typedef struct{
	gpio_pin_t	       pin;
	gpio_func_mode_t   func_mode;
	gpio_pull_mode_t   pull_mode;
	gpio_speed_mode_t  speed_mode;
	gpio_output_mode_t output_mode;
}gpio_config_t;

extern gpio_reg_t *GPIOA;
extern gpio_reg_t *GPIOB;
extern gpio_reg_t *GPIOC;
extern gpio_reg_t *GPIOD;
extern gpio_reg_t *GPIOH;

void gpiox_reset();

void gpio_config(gpio_reg_t *pPort, gpio_config_t *pConfig);

void gpio_set_val(gpio_reg_t *pPort, gpio_pin_t pin, bool_t val);

bool_t gpio_get_val(gpio_reg_t *pPort, gpio_pin_t pin);


#endif /* GPIO_H_ */
