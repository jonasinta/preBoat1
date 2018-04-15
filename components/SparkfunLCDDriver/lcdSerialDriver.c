/* Uart Example

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */
#include "../SparkfunLCDDriver/include/lcdSerialDriver.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "soc/uart_struct.h"
static const char *TAG = "uart_example";

/**
 * Drive Sparkfun serial lcd module
 * connect rx of lcd to uart tx pin for selected uart module.
 * uart_evt_test() is an example that read and write data on UART0, and handler some of the special events.
 * - port: UART1 default can change in make menuconfig
 * - tx buffer: on
 * - flow control: off
 * - event queue: on
 * - pin assignment: txd(i/o4) default for uart1 can change in make menuconfig
 *
 *
 */

#define BUF_SIZE (1024)
enum jonBaudRate {
	fast = 38400, standard = 9600,
};

// wbp: goto with row & column
void lcdPosition(int row, int col) {
	const char temp[] = { 0xfe, (col + (row * 64) + 128) };
	uart_write_bytes(UART_NUM_1, temp, sizeof(temp));   //command flag
	vTaskDelay(5 / portTICK_RATE_MS);

	/*const char temp2[] = { "Hullo Stef n Toby" };
	 uart_write_bytes(UART_NUM_1, temp2, sizeof(temp2));    //position
	 vTaskDelay(400 / portTICK_RATE_MS);*/
}
void lcdPrint(const char * textIn, int length) {
	uart_write_bytes(UART_NUM_1, textIn, length);
}
void clearLCD(void) {
	const char temp2[] = { 0xfe, 0x01 };
	uart_write_bytes(UART_NUM_1, temp2, sizeof(temp2));    //position

	vTaskDelay(1 / portTICK_RATE_MS);
}
void configLCD(void) {
	const char temp2[] = { 0x7c, 0x04, 0x7c, 0x06 }; //16 characters wide, 2 lines
	uart_write_bytes(UART_NUM_1, temp2, sizeof(temp2));    //send
	vTaskDelay(100 / portTICK_RATE_MS);
	void clearLCD(void);
}
void speedLCD(int speed) {
	//result = a > b ? x : y;
	char speedCode = (speed == standard) ? 0x0d : 0x10; //set baud 9600 is 0x0d or ctrl+M, 38400 0x10 or ctrl+P
	const char temp2[] = { 0x7c, speedCode }; //speed to 38400 baud or 9600
	uart_write_bytes(UART_NUM_1, temp2, sizeof(temp2));    //send
	vTaskDelay(100 / portTICK_RATE_MS);

	vTaskDelay(100 / portTICK_RATE_MS);
}

void serialLCDInit() {
	// Initialise Uart for lcd operations...................................

	uart_config_t uart_config = { .baud_rate = 9600, .data_bits =
			UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits =
			UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.rx_flow_ctrl_thresh = 122, };
	//Configure UART1 parameters
	uart_param_config(UART_NUM_1, &uart_config);
	//Set UART1 pins(TX: IO4, RX: I05, RTS: IO18, CTS: IO19)
	uart_set_pin(UART_NUM_1, CONFIG_uart_TXD, UART_PIN_NO_CHANGE,
			UART_PIN_NO_CHANGE,
			UART_PIN_NO_CHANGE);

	//Install UART driver( We don't need an event queue here)
	//In this example we don't even use a buffer for sending data.
	uart_driver_install(UART_NUM_1, BUF_SIZE, BUF_SIZE, 0, NULL, 0);
	vTaskDelay(750 / portTICK_RATE_MS);
	configLCD();
	lcdPosition(0, 0);

}
