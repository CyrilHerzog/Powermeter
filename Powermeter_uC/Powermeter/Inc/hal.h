/*
 * hal.h
 *
 *  Created on: 29.03.2023
 *      Author: Herzog Cyril
 */

#ifndef HAL_H_
#define HAL_H_

#include <system.h>
#include <adc.h>
#include <gpio.h>
#include <usart.h>
#include <spi.h>


//ToDo
typedef enum {
	HAL_OK                = 0,
	HAL_ERROR             = 1,
	HAL_DRV_ERROR         = 2,  // SPI, I2C, UART ....
	HAL_MCP4241_ADR_ERROR = 10,
}hal_err_t;

// Initialize all Driver's for used IC's (SPIx, UARTx, GPIOx) =>
hal_err_t hal_init(void);

// Internal ADC (Read Force Sensor) =>
hal_err_t read_voltage(uint16_t *ptr);

// Bluetooth Low Energy Module RN4020 =>
#define rn4020_cmd_mode()     	gpio_set_val(GPIOB, GPIO_4, false)
#define rn4020_mldp_mode()    	gpio_set_val(GPIOB, GPIO_4, true)
#define rn4020_wake_sw_0()	  	gpio_set_val(GPIOB, GPIO_5, false)
#define rn4020_wake_sw_1()    	gpio_set_val(GPIOB, GPIO_5, true)
#define rn4020_wake_hw_0()	  	gpio_set_val(GPIOB, GPIO_6, false)
#define rn4020_wake_hw_1()	  	gpio_set_val(GPIOB, GPIO_6, true)
#define rn4020_state_connect()	gpio_get_val(GPIOB, GPIO_9);


/* RN4020
 *
 * POWER STATES
 * POWER ON   -> IDLE ~2mA             by WAKE_SW_1, WAKE_HW_0
 * POWER ON   -> DEEP SLEEP  ~5uA      by WAKE_SW_0, WAKE_HW_0
 * IDLE       -> DORMANT ~900nA        by WAKE_SW_0, WAKE_HW_0, COMMAND "o"
 * DORMANT    -> IDLE                  by WAKE_SW_1, WAKE_HW_1 (THEN SET WAKE_HW_0!) => push
 * IDLE       -> DEEP SLEEP            by WAKE_SW_0, WAKE_HW_0
 * DORMANT    -> DEEP SLEEP            by WAKE_SW_0, WAKE_HW_1 (SW STAY 0, THEN SET HW_0!) = push
 * DEEP SLEEP -> ACTIVE RF ~17-20mA    by WAKE_SW_1, WAKE_HW_0
 * ACTIVE RF  -> DEEP SLEEP            by WAKE_SW_0, WAKE_HW_0
 * IDLE       -> ACTIVE RF             by START RF ACTIVITY
 * ACTIVE RF  -> IDLE                  by END RF ACTIVITY
 *
 *
 *
 */

void rn4020_connect();

bool_t rn4020_get_connect_state();


// Digitalpotentiometer mcp4241 =>
#define mcp4241_cs_low()     gpio_set_val(GPIOB, GPIO_8, false)
#define mcp4241_cs_high()    gpio_set_val(GPIOB, GPIO_8, true)

#define MCP4241_CMD_WRITE   (uint8_t) 0x00
#define MCP4241_CMD_INCR    (uint8_t) 0x01
#define MCP4241_CMD_DECR    (uint8_t) 0x02
#define MCP4241_CMD_READ    (uint8_t) 0x03

typedef enum {
	MCP4241_VOLATILE_WIPER_0     = 0x00,
	MCP4241_VOLATILE_WIPER_1     = 0x01,
	MCP4241_NON_VOLATILE_VIPER_0 = 0x02,
	MCP4241_NON_VOLATILE_VIPER_1 = 0x03,
	MCP4241_VOLATILE_TCON_REG    = 0x04,
	MCP4241_STATUS_REG           = 0x05,
	MCP4241_EEPROM_0			 = 0x06,
	MCP4241_EEPROM_1			 = 0x07,
	MCP4241_EEPROM_2			 = 0x08,
	MCP4241_EEPROM_3			 = 0x09,
	MCP4241_EEPROM_4			 = 0x0A,
	MCP4241_EEPROM_5			 = 0x0B,
	MCP4241_EEPROM_6			 = 0x0C,
	MCP4241_EEPROM_7     	     = 0x0D,
	MCP4241_EEPROM_8			 = 0x0E,
	MCP4241_EEPROM_9	         = 0x0F,
}mcp4241_adr_t;


// increment
hal_err_t mcp4241_incr(mcp4241_adr_t adr);

// decrement
hal_err_t mcp4241_decr(mcp4241_adr_t adr);

// write
hal_err_t mcp4241_write(mcp4241_adr_t adr, uint8_t val);

// read
hal_err_t mcp4241_read(mcp4241_adr_t adr, uint8_t *pVal);

// Gyrosensor LSM6DSO32
// ToDo => extend features (config struct, accelometer, .....)

#define lsm6dso32_cs_low()     gpio_set_val(GPIOB, GPIO_7, false)
#define lsm6dso32_cs_high()    gpio_set_val(GPIOB, GPIO_7, true)

#define LSM6DSO32_RW_CMD	   0x80
#define LSM6DSO32_CTRL2_A	   0x10
#define LSM6DSO32_CTRL2_G	   0x11

typedef enum {
	LSM6DSO32_AXIS_X_L = 0x22,
	LSM6DSO32_AXIS_X_H = 0x23,
	LSM6DSO32_AXIS_Y_L = 0x24,
	LSM6DSO32_AXIS_Y_H = 0x25,
	LSM6DSO32_AXIS_Z_L = 0x26,
	LSM6DSO32_AXIS_Z_H = 0x27
}lsm6dso32_axis_t;


// read axis degree
hal_err_t read_axis_dps(lsm6dso32_axis_t axis, uint8_t *pVal);



#endif /* HAL_H_ */
