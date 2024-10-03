/*
 * hal.c
 *
 *
 *
 *
 *
 *  Created on: 29.03.2023
 *      Author: Herzog Cyril
 */


#include <hal.h>


// spi1 is used for two devices in this application
static volatile bool_t spi_init_done = false;
static volatile bool_t rn4020_is_connect = false;


// scale between Min and Max Value (prefer 0 - 3300 => 0 - 3,3V)
hal_err_t read_voltage(uint16_t *pData){
	uint32_t temp = 0;
	uint16_t val  = 0;
	// polling 64 samples
	for (int j=0; j<8; j++){
		if (adc_read(&val) == SYSTEM_OK) {
			temp += (uint32_t)val;
		}else {
			*pData = 0;
			return HAL_DRV_ERROR;
		}
	}
	// average and scale
	temp = (((temp >> 3) * (uint32_t)(3300)) >> 12);
	*pData = (uint16_t)temp;
	return HAL_OK;
}


// digital potentiometer mcp4241 =>
/*
 * Command Bits:
 * 00 = Write Data
 * 01 = Increment
 * 10 = Decrement
 * 11 = Read Data
 *
 */
hal_err_t mcp4241_init(){

	if (spi_init_done == false) {
		spi_config_t spi_cfg;
		spi_cfg.clk_div = SPI_DIV2;
		spi_cfg.frame_format = SPI_MSB_FIRST;
		spi_cfg.mode = SPI_MODE_0;
		spi_init(SPI1, &spi_cfg);
	} else {
		print("HAL Info: SPI is already installed");
	}

	// chip select
	gpio_config_t gpio_cfg;
	gpio_cfg.pin         = GPIO_8;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_OUTPUT;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
	gpio_config(GPIOB, &gpio_cfg);

	mcp4241_cs_high();
	wait(100);


	uint8_t temp;
	// gain =>
	mcp4241_read(MCP4241_EEPROM_0, &temp); // read the error don't work with eeprom

	if (mcp4241_write(MCP4241_VOLATILE_WIPER_0, temp) != HAL_OK) {
		print("HAL: Error write gain");
		return HAL_ERROR;
	}

	// offset =>
	mcp4241_read(MCP4241_EEPROM_1, &temp); // read the error don't work with eeprom

	if (mcp4241_write(MCP4241_VOLATILE_WIPER_1, temp) != HAL_OK) {
		print("HAL: Error write offset");
		return HAL_ERROR;
	}

	return HAL_OK;
}


// increment by 8-Bit Command
// [7:4] Adr [3:2] cmd [1:0] data (always zero)

hal_err_t mcp4241_incr(mcp4241_adr_t adr) {
	uint8_t rx_data[1];
	uint8_t tx_data[1];

	tx_data[0] = ((adr << 4) | MCP4241_CMD_INCR << 2);

	mcp4241_cs_low();
	if(spi_transaction(SPI1, tx_data, rx_data, 1) != SYSTEM_OK) {
		return HAL_DRV_ERROR;
	}
	mcp4241_cs_high();

	// Bit 2 is cmd error bit => by error goes to low
	// all other bits have value high
	if (rx_data[0] != 0xFF) {
		return HAL_MCP4241_ADR_ERROR;
	}
	return HAL_OK;
}

// decrement by 8-Bit Command
// [7:4] Adr [3:2] cmd [1:0] data (always zero)
hal_err_t mcp4241_decr(mcp4241_adr_t adr) {
	uint8_t rx_data[1];
	uint8_t tx_data[1];

	tx_data[0] = ((adr << 4) | MCP4241_CMD_DECR << 2);

	mcp4241_cs_low();
	if(spi_transaction(SPI1, tx_data, rx_data, 1) != SYSTEM_OK) {
		return HAL_DRV_ERROR;
	}
	mcp4241_cs_high();

	// Bit 2 is cmd error bit => by error goes to low
	// all other bits have value high
	if (rx_data[0] != 0xFF) {
		return HAL_MCP4241_ADR_ERROR;
	}
	return HAL_OK;
}

// write by 16-Bit Command
// [15:12] Adr [11:10] cmd [9:0] data (i'm use only 7 bit of transfer data)
hal_err_t mcp4241_write(mcp4241_adr_t adr, uint8_t val) {
	uint8_t rx_data[2];
	uint8_t tx_data[2];

	tx_data[0] = ((adr << 4) | MCP4241_CMD_WRITE << 2);
	tx_data[1] = val;

	mcp4241_cs_low();
	if(spi_transaction(SPI1,tx_data,rx_data, 2) != SYSTEM_OK) {
		return HAL_DRV_ERROR;
	}
	mcp4241_cs_high();

	// Bit 9 is cmd error bit => by error goes to low
	// all other bits have value high
	if ((rx_data[0] & rx_data[1]) != 0xFF) {
		return HAL_MCP4241_ADR_ERROR;
	}

	return HAL_OK;
}


// read by 16-Bit Command
// [15:12] Adr [11:10] cmd [9:0] data (i'm use only 7 bit of received data)
hal_err_t mcp4241_read(mcp4241_adr_t adr, uint8_t *pVal) {
	uint8_t rx_data[2];
	uint8_t tx_data[2];

	tx_data[0] = ((adr << 4) | MCP4241_CMD_READ << 2);

	mcp4241_cs_low();
	if(spi_transaction(SPI1,tx_data,rx_data, 2) != SYSTEM_OK) {
		return HAL_DRV_ERROR;
	}
	mcp4241_cs_high();

	// Bit 9 is cmd error bit => by error goes to low
	// all other bits have value high
	if (rx_data[0] != 0xFE) {
		*pVal = 0x00;
		return HAL_MCP4241_ADR_ERROR;
	}

	*pVal = rx_data[1];
	return HAL_OK;
}



// commando print for rn4020 => same as system_print
void send_cmd(const char *msg){

	uint8_t i = 0;
	uint8_t TxBuf[63];

	// copy message
	while (*msg != 0){
		TxBuf[i] = *msg++;
		i++;
	}
	// for console
	TxBuf[i] = '\r';  // no diagonal printing
	i++;
	TxBuf[i] = '\n';
	i++;

	usart_send(USART1, TxBuf, i);
}


hal_err_t rn4020_init(){

	usart_config_t usart_cfg;
	usart_cfg.clk_div      = 139; // (16000000 / 115200)
	usart_cfg.format       = USART_LSB_FIRST;
	usart_cfg.mode         = USART_MODE_TX;
	usart_cfg.parity       = USART_NO_PARITY;
	usart_cfg.word_len     = USART_WORD_LEN_8BIT;
	usart_cfg.stp_bit      = USART_STOP_1BIT;
	usart_cfg.flow_control = USART_HW_FLOW_CONTROL_DISABLE;
	usart_cfg.interrupt    = USART_INTERRUPT_DISABLE;
	usart_init(USART1, &usart_cfg);

	// cmd mode
	gpio_config_t gpio_cfg;
	gpio_cfg.pin         = GPIO_4;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_OUTPUT;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_LOW_SPEED;
	gpio_config(GPIOB, &gpio_cfg);

	// wake_sw
	gpio_cfg.pin         = GPIO_5;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_OUTPUT;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_LOW_SPEED;
	gpio_config(GPIOB, &gpio_cfg);

	// wake_hw
	gpio_cfg.pin         = GPIO_6;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_OUTPUT;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_LOW_SPEED;
	gpio_config(GPIOB, &gpio_cfg);

	// state connect
	gpio_cfg.pin         = GPIO_9;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_INPUT;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_PULL_UP;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_LOW_SPEED;
	gpio_config(GPIOB, &gpio_cfg);

	// cs
	gpio_cfg.pin         = GPIO_10;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_INPUT;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_PULL_UP;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_LOW_SPEED;
	gpio_config(GPIOB, &gpio_cfg);

	// mosi
	gpio_cfg.pin         = GPIO_11;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_INPUT;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_PULL_UP;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_LOW_SPEED;
	gpio_config(GPIOB, &gpio_cfg);



	rn4020_wake_sw_1();
	rn4020_wake_hw_1();

	rn4020_cmd_mode();
	//rn4020_mldp_mode();

	// factory reset
	if (usart_send_string(USART1, "SF,1\r\n") != SYSTEM_OK) {
		print("HAL: Error Data Transfer to BluetoothLE module");
		return HAL_DRV_ERROR;
	}

	wait(500);

	if (usart_send_string(USART1, "SS,00000001\r\n") != SYSTEM_OK) {
		print("HAL: Error Data Transfer to BluetoothLE module");
		return HAL_DRV_ERROR;
	}
	wait(500);


	// Auto Advertise    => 0x20000000
	// Enable MLDP       => 0x10000000
	// UART Flow Control => 0x02000000
	// Auto Enter MLDP   => 0x00000800 => not used
	if (usart_send_string(USART1, "SR,32000000\r\n") != SYSTEM_OK) {
		print("HAL: Error Data Transfer to BluetoothLE module");
		return HAL_DRV_ERROR;
	}
	wait(500);

	if (usart_send_string(USART1, "SN,P4\r\n") != SYSTEM_OK) {
			print("HAL: Error Data Transfer to BluetoothLE module");
			return HAL_DRV_ERROR;
		}
	wait(500);

	// Anpassung Baudrate => 115200
	if (usart_send_string(USART1, "SB,4\r\n") != SYSTEM_OK) {
		print("HAL: Error Data Transfer to BluetoothLE module");
		return HAL_DRV_ERROR;
	}
	wait(500);


	// reboot
	if (usart_send_string(USART1, "R,1\r\n") != SYSTEM_OK) {
		print("HAL: Error Data Transfer to BluetoothLE module");
		return HAL_DRV_ERROR;
	}
	wait(2000);




	// ble - mode with flow control


	usart_cfg.clk_div      = 139; // => 16000000 / 115200
	usart_cfg.format       = USART_LSB_FIRST;
	usart_cfg.mode         = USART_MODE_TX_RX;
	usart_cfg.parity       = USART_NO_PARITY;
	usart_cfg.word_len     = USART_WORD_LEN_8BIT;
	usart_cfg.stp_bit      = USART_STOP_1BIT;
	usart_cfg.flow_control = USART_HW_FLOW_CONTROL_ENABLE;
	usart_cfg.interrupt    = USART_RX_INTERRUPT_ENABLE;
	usart_init(USART1, &usart_cfg);


//	rn4020_mldp_mode(); => not used, switch in mldp mode with command ("I")

	return HAL_OK;
}


void rn4020_connect() {
	static bool_t statePre = false;

	bool_t state;

	state = gpio_get_val(GPIOB, GPIO_9);

	if (state == false) {
		rn4020_is_connect = false;
	}
	// positive flag detect
	if ((state == true) && (statePre == false)) {
		wait(100);
		usart_send_string(USART1, "I\n");
		wait(1000);
		rn4020_is_connect = true;
	}

	statePre = state;
}

bool_t rn4020_get_connect_state() {
	return rn4020_is_connect;
}


hal_err_t lsm6dso32_init() {

	if (spi_init_done == false) {
		spi_config_t spi_cfg;
		spi_cfg.clk_div = SPI_DIV2;
		spi_cfg.frame_format = SPI_MSB_FIRST;
		spi_cfg.mode = SPI_MODE_0;
		spi_init(SPI1, &spi_cfg);
	} else {
		print("HAL Info: SPI is already installed");
	}


	// chip select
	gpio_config_t gpio_cfg;
	gpio_cfg.pin         = GPIO_7;
	gpio_cfg.func_mode   = GPIO_FUNC_MODE_OUTPUT;
	gpio_cfg.output_mode = GPIO_OUTPUT_MODE_PUSH_PULL;
	gpio_cfg.pull_mode   = GPIO_PULL_MODE_NO_PULL;
	gpio_cfg.speed_mode  = GPIO_SPEED_MODE_HIGH_SPEED;
	gpio_config(GPIOB, &gpio_cfg);

	lsm6dso32_cs_high();
	wait(100);

	uint8_t tx_data[2];
	uint8_t rx_data[2];

	tx_data[0] = LSM6DSO32_CTRL2_G;
	tx_data[1] = 0b01101100; // 416Hz rate + 2000dps

	lsm6dso32_cs_low();

	if(spi_transaction(SPI1,tx_data,rx_data, 2) != SYSTEM_OK) {
		return HAL_DRV_ERROR;
	}

	lsm6dso32_cs_high();

	return HAL_OK;
}

hal_err_t read_axis_dps(lsm6dso32_axis_t axis, uint8_t *pVal) {
	uint8_t tx_data[2];
	uint8_t rx_data[2];

	tx_data[0] = (LSM6DSO32_RW_CMD | axis);

	lsm6dso32_cs_low();

	if(spi_transaction(SPI1,tx_data,rx_data, 2) != SYSTEM_OK) {
		return HAL_DRV_ERROR;
	}

	lsm6dso32_cs_high();

	*pVal = rx_data[1];

	return HAL_OK;
}



hal_err_t hal_init(void){

	// initialization and calibration the analog digital converter
	adc_config_t adc_cfg;
	adc_cfg.pin_num     = GPIO_0;
	adc_cfg.resolution  = ADC_RES_12_BIT;
	adc_cfg.sample_time = ADC_CYCLE_1_5;

	if (adc_init(&adc_cfg) != SYSTEM_OK) {
		print("HAL: Error Initialization analog digital converter");
		return HAL_DRV_ERROR;
	}

	wait(500); // ADC must run for some time before calibration can start
	//adc_cal();

	if (adc_cal() != SYSTEM_OK) {
		print("HAL: Error to calibration the analog digital converter");
		return HAL_DRV_ERROR;
	}

	// initialization the rn4020 bluetoothLE module
	if (rn4020_init() != HAL_OK) {
		print("HAL: Error Initialization BluetoothLE Module");
		return HAL_DRV_ERROR;
	}

	// initialization the digital potentiometer mcp4241
	if (mcp4241_init() != HAL_OK) {
		print("HAL: Error Initialization Digital Potentiometer");
		return HAL_DRV_ERROR;
	}

	// initialization the gyro-accelometer lsm6dso32
	if (lsm6dso32_init() != HAL_OK) {
		print("HAL: Error Initialization Gyroscope");
		return HAL_DRV_ERROR;
	}

	print("HAL: Initialization success");

	return HAL_OK;

}
