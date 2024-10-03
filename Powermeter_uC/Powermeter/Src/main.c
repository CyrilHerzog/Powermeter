/*
 * Programm Powermeter Print (STM32 L051)
 *
 * Message from RN4020 =>
 *		string with variable length with "\n"
 *
 * Defined command from smartphone-application =>
 * 		fixed length of 4 bytes
 * 		[BLE_APP_TRANSFER_CMD][instruction][data][\n]
 *
 * Transfer data for smartphone-application =>
 * 		[BLE_APP_TRANSFER_CMD][ADC_L][ADC_H][DPS_L][DPS_H][\n]
 *
 *
 * Herzog Cyril
 * 26.03.23
 *
 *
 */


#include <system.h>
#include <hal.h>
#include <fifo.h>


#define BLE_RX_BUFFER_SIZE			1024
#define BLE_TX_BUFFER_SIZE 			4096
#define BLE_APP_TRANSFER_CMD		192

// instruction-code's
#define INST_GAIN_INC				0b10000001
#define INST_GAIN_DEC				0b10000010
#define INST_GAIN_WR				0b10000011
#define INST_OFFSET_INC				0b10001001
#define INST_OFFSET_DEC				0b10001010
#define INST_OFFSET_WR				0b10001011



union bit32_t {
	uint8_t abyValue[4];
	uint32_t udiValue;
};

union bit16_t {
	uint8_t abyValue[2];
	uint16_t uiValue;
};


fifo_t ble_rx_fifo;
fifo_t ble_tx_fifo;

uint8_t ble_rx_buffer[BLE_RX_BUFFER_SIZE];
uint8_t ble_tx_buffer[BLE_TX_BUFFER_SIZE];



void USART1_IRQHandler(void){

	uint8_t rx_data;

	if (usart_receive(USART1, &rx_data) != SYSTEM_OK) {
		print("System: Error Receive from BluetoothLe Module");
	}

	if (ble_rx_fifo.state != FIFO_FULL) {
		fifo_write(&ble_rx_fifo, rx_data);
		//print("BLE Receive FIFO has add an Element");

	} else {
		print("BLE Receive FIFO is full");
	}
}


void __Task_1(void){


	union bit16_t value_16bit;

	fifo_write(&ble_tx_fifo, BLE_APP_TRANSFER_CMD);

	//static uint16_t inc = 0;


	// read force sensor
	if (read_voltage(&value_16bit.uiValue) != HAL_OK) {
		print("HAL: Error read ADC voltage from force sensor");
	}
	//value_16bit.uiValue = inc;

	fifo_write(&ble_tx_fifo, value_16bit.abyValue[0]);
	fifo_write(&ble_tx_fifo, value_16bit.abyValue[1]);
/*
	if (inc == 200) { only for testing
			inc = 0;
		}else{
			inc++;
		}
*/
	// read gyro sensor
	uint8_t axis_dps;
	if (read_axis_dps(LSM6DSO32_AXIS_Z_L, &axis_dps) != HAL_OK) {
		print("HAL: Error read dps axis z low on gyroscope");
	}

	fifo_write(&ble_tx_fifo, axis_dps);

	if (read_axis_dps(LSM6DSO32_AXIS_Z_H, &axis_dps) != HAL_OK) {
		print("HAL: Error read dps axis z high on gyroscope");
	}

	fifo_write(&ble_tx_fifo, axis_dps);

	fifo_write(&ble_tx_fifo, 10); // "\n"

}


uint8_t rx_data[BLE_RX_BUFFER_SIZE];
uint8_t rx_data_index = 0;
uint8_t offset	= 0;
uint8_t gain    = 0;
bool_t app_cmd_receive_flag = false;



void __Task_2(void){



	wait(42);








	if (ble_rx_fifo.state != FIFO_EMPTY) {
		rx_data[rx_data_index] = fifo_read(&ble_rx_fifo);
		rx_data_index++;
		if(rx_data[rx_data_index - 1] == 10) { // "\n"
			// Length = 4, Byte_0 = app_cmd_wr => Data is from Smartphone
			if ((rx_data_index == 5) && (rx_data[0] == BLE_APP_TRANSFER_CMD)) {
				app_cmd_receive_flag = true;
				print("Receive Command from Smartphone");
			} else {
				// Message is directly from RN4020
				print("Message from RN4020 - OS");
				usart_send(USART2, rx_data, rx_data_index); // print Message
			}
			rx_data_index = 0;
		}
	}

	if (rx_data_index >= BLE_RX_BUFFER_SIZE) {
		rx_data_index = 0;
	}

	// Handle received instructions from smartphone-application
	if (app_cmd_receive_flag == true) {
		switch (rx_data[1]) {   // instruction
		    case INST_GAIN_INC:
		    	if (mcp4241_incr(MCP4241_VOLATILE_WIPER_0) != HAL_OK) {
		    		print("HAL: Error increment gain");
		    	}
		        break;
		    case INST_GAIN_DEC:
		    	if (mcp4241_decr(MCP4241_VOLATILE_WIPER_0) != HAL_OK) {
		    		print("HAL: Error decrement gain");
		    	}
		        break;
		    case INST_GAIN_WR:
		        if (mcp4241_write(MCP4241_VOLATILE_WIPER_0, rx_data[2]) != HAL_OK) {
		        	print("HAL: Error write gain");
		        }
		        break;
		    case INST_OFFSET_INC:
		    	if (mcp4241_incr(MCP4241_VOLATILE_WIPER_1) != HAL_OK) {
		    		print("HAL: Error increment offset");
		    	}
		        break;
		    case INST_OFFSET_DEC:
		    	if (mcp4241_decr(MCP4241_VOLATILE_WIPER_1) != HAL_OK) {
		    		print("HAL: Error decrement offset");
		    	}
		        break;
		    case INST_OFFSET_WR:
		        if (mcp4241_write(MCP4241_VOLATILE_WIPER_1, rx_data[2]) != HAL_OK) {
		        	print("HAL: Error write offset");
		        }
		        break;
		    default:
		        print("APP Transfer: No valid Instruction");
		        break;
		}

		app_cmd_receive_flag = false;
	}


	// read the actually value and save into internal eeprom

	// => gain
	if (mcp4241_read(MCP4241_VOLATILE_WIPER_0, &gain) != HAL_OK) {
		print("HAL: Error read gain");
	}

	if (mcp4241_write(MCP4241_EEPROM_0, gain) != HAL_OK) {
		print("HAL: Error write gain into eeprom");
	}

	// => offset
	if (mcp4241_read(MCP4241_VOLATILE_WIPER_1, &offset) != HAL_OK) {
		print("HAL: Error read offset");
	}

	if (mcp4241_write(MCP4241_EEPROM_1, offset) != HAL_OK) {
		print("HAL: Error write offset into eeprom");
	}


	// automatic connection
	rn4020_connect();

	// transfer data

	uint8_t tx_data[24];
	if (rn4020_get_connect_state() == true) {
		for (uint8_t i = 0; i < 24; i++) {
			tx_data[i] = fifo_read(&ble_tx_fifo);
		}
		usart_send(USART1, tx_data, 24);
	}



}


void __Run_Up(void) {

	if (init_system() != SYSTEM_OK) {
		print("SYSTEM: Error initialize");
	}

	if (hal_init() != HAL_OK) {
		print("HAL: Error initialize");
	}

	fifo_init(&ble_rx_fifo, ble_rx_buffer, BLE_RX_BUFFER_SIZE);
	fifo_init(&ble_tx_fifo, ble_tx_buffer, BLE_TX_BUFFER_SIZE);
}


int main(void)
{
	__OS();
}
