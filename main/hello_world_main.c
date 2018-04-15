/* Hello World Example

 This example code is in the Public Domain (or CC0 licensed, at your option.)

 Unless required by applicable law or agreed to in writing, this
 software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "bmp180Jonas.h"
#include "esp_wifi.h"
#include "jonas_proj.h"
#include "esp_event_loop.h"
#include "tcpip_adapter.h"
#include "esp_event.h"
#include "lwip/err.h"
#include "apps/sntp/sntp.h"
#include "apps/sntp/sntp_opts.h"
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include "mqtt.h"
#include "esp_int_wdt.h"
#include "esp_task_wdt.h"
#include "lcdSerialDriver.h"
#include "esp_wifi.h"
#include "esp32/ulp.h"
#include "rom/ets_sys.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

#define u16_t uint16_t

//GLOBALS........
mqtt_client *mqttClientJonas; //my client
extern int public_heap; // project variables
extern double public_tempC;
time_t now;
unsigned int esp32ID = 0; //for my chipid
EventGroupHandle_t  group_task_watchdog;
wifi_config_t sta_config = { .sta =
		{ .ssid = "vodafone-doing",
		.password =	"wallyb26",
		.bssid_set = false
		}
	};


enum task_shift {
	blink_task_shift, bmp180_shift, post_variables_shift
};
//END GLOBALS

void getMacID(void) {
	uint8_t chipid[6];
	esp_efuse_read_mac(chipid);
	for (int var = 0; var < sizeof(chipid); ++var) {
		printf("%02x   ", chipid[var]);
	}
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n", chipid[0], chipid[1], chipid[2],
			chipid[3], chipid[4], chipid[5]);

	int varShift = 0;
	for (int var = 5; var > 2 && var < 6; var--, varShift++) {
		esp32ID = esp32ID | (chipid[var] << (8 * varShift));
	}
	printf("sizeof esp32id %d-   ", sizeof(esp32ID));
	printf("hex value:%08x, decimal value:%d    \n", esp32ID, esp32ID);

}
void connected_cb(void *self, void *params) {
	//mqtt_client *client = (mqtt_client *) self;

	flash_rate = connectedMQTT;
	lcdPosition(1, 0);
	char message[] = "MqttON  ";
	lcdPrint(message, sizeof(message));

}
void disconnected_cb(void *self, void *params) {
	flash_rate = connectingMQTT;
	lcdPosition(1, 0);
	char message[] = "MqttOFF ";
	lcdPrint(message, sizeof(message));
}

void subscribe_cb(void *self, void *params) {
	printf("[APP] Subscribe ok, test publish msg\n");
	mqtt_client *client = (mqtt_client *) self;
	mqtt_publish(client, "/tester", "abcde", 5, 0, 0);
}

void publish_cb(void *self, void *params) {

}

void data_cb(void *self, void *params) {

	mqtt_client *client = (mqtt_client *) self;
	mqtt_event_data_t *event_data = (mqtt_event_data_t *) params;

	if (event_data->data_offset == 0) {
//9
		char *topic = malloc(event_data->topic_length + 1);
		memcpy(topic, event_data->topic, event_data->topic_length);
		topic[event_data->topic_length] = 0;
		printf("[APP] Publish topic: %s\n", topic);
		free(topic);
	}

	char *data = malloc(event_data->data_length + 1);
	memcpy(data, event_data->data, event_data->data_length);
	//data[event_data->data_length] = 0;
	printf("[APP]  data[%d/%d bytes]\n",
			event_data->data_length + event_data->data_offset,
			event_data->data_total_length);
	// data);
	printf("[APP] recieved data;%s\n", data);

	free(data);
	getMacID();
}
//MQTT SETTINGS..........................
mqtt_settings settings = {
		//.host = "test.mosquitto.org",
		.host = "192.168.1.71",
#if defined(CONFIG_MQTT_SECURITY_ON)
		.port = 8883, // encrypted
#else
		//.port = 1883, // unencrypted
		.port = 10002, // unencrypted
#endif
		.client_id = "homeMqtt", .username = "user",
		.password = "pass", //"mqtt_client_id" or "homeMqtt"
		.clean_session = 0, .keepalive = 120, .lwt_topic = "/lwt", .lwt_msg =
				"offline_GONE_GONE_GONE", .lwt_retain = 0, .connected_cb =
				connected_cb, .disconnected_cb = disconnected_cb,
		 .subscribe_cb = subscribe_cb, .auto_reconnect = 1,
		.publish_cb = publish_cb, .data_cb = data_cb };
//END MQTT SETTINGS..........................

esp_err_t event_handler(void *ctx, system_event_t *event) {
	if (event->event_id == SYSTEM_EVENT_STA_GOT_IP) {
		printf("Our IP address is " "%d.%d.%d.%d" "\n",
				//IP2STR(&event->event_info.got_ip));
		IP2STR(&event->event_info.got_ip.ip_info.ip));
		printf("We have now connected to a station and can do things...\n");
		Flag_wifi_connected = 1;
		lcdPosition(1, 9);
		char message[] = "WifiON ";
		//Now connect to MQTT
		lcdPrint(message, sizeof(message));
		lcdPosition(1, 0);
		int len = 0;
		char message2[] = "MqttOFF ";
		lcdPrint(message2, sizeof(message2));
		flash_rate = connectingMQTT;
		mqttClientJonas = mqtt_start(&settings);

	}
	return ESP_OK;
}
//set up sntp ####################################
static void initialize_sntp(void) {
	ESP_LOGI(TAG, "Initializing SNTP");
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
}
// end set up sntp##################################

void watchdog_task(void *pvParameter) {
	/* Get flags from all tasks to say alive then kick the dog
	 * task watchdog timeout is 5s- set in menuconfig near bottom of esp specific section
	 */
	//esp_task_wdt_feed();
	while (1) {
		//esp_task_wdt_feed();
	    /*SET_PERI_REG_BITS(SENS_SAR_MEAS_WAIT2_REG, SENS_FORCE_XPD_SAR, 3, SENS_FORCE_XPD_SAR_S);
	    SET_PERI_REG_BITS(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_CLK_DIV, 10, SENS_TSENS_CLK_DIV_S);
	    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP);
	    CLEAR_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_DUMP_OUT);
	    SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP_FORCE);
	    SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_POWER_UP);
	    ets_delay_us(1000);
	    SET_PERI_REG_MASK(SENS_SAR_TSENS_CTRL_REG, SENS_TSENS_DUMP_OUT);
	    ets_delay_us(5);
	    int res = GET_PERI_REG_BITS2(SENS_SAR_SLAVE_ADDR3_REG, SENS_TSENS_OUT, SENS_TSENS_OUT_S);
	    printf("res=%d\t", res);
	    printf("res=%#06x\n", res);
*/



		vTaskDelay(3 * 1000 / portTICK_PERIOD_MS); //check every 10 secs

	}
}
void blink_task(void *pvParameter) {
	/* Configure the IOMUX register for pad BLINK_GPIO (some pads are
	 muxed to GPIO on reset already, but some default to other
	 functions and need to be switched to GPIO. Consult the
	 Technical Reference for a list of pads and their default
	 functions.)
	 */
	gpio_pad_select_gpio(BLINK_GPIO);
	/* Set the GPIO as a push/pull output */
	gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
	while (1) {
		/* Blink off (output low) */
		gpio_set_level(BLINK_GPIO, 0);
		vTaskDelay(250 / portTICK_PERIOD_MS);
		/* Blink on (output high) */
		gpio_set_level(BLINK_GPIO, 1);
		vTaskDelay(flash_rate / portTICK_PERIOD_MS);

	}
}

void post_variables_task(void *pvParameter) {
	while (1) {
		if (Flag_wifi_connected) {
			printf("########################################\n");
			time(&now);
			// INFO("[jonas]  test publish msg\n");
			int heap, timestamp;
			heap = 456;
			public_heap = system_get_free_heap_size();

			if (public_heap)
				heap = public_heap;
			float volts;
			volts = 12.7;
			if (public_tempC)
				volts = public_tempC;
			char message[100];
			int len = 3;

			timestamp = 1477606545;
			if (now > 12000)
				timestamp = now;

			len = sprintf(message,
					"{\"HEAP\":%d,\"Volts\":%f,\"timestamp\":%ld}", heap, volts,
					(long) now); //json
			//json must have escaped " ; e.g \"

			//must publish to "/mcu/14056893/heap,volts,stamp/" pahoListenPostSQLLAN.jar in the toshiba lappy home/
			//directory is only subscribed to that- currently
			printf("message=%s; message length=%d\n", message, len);
			mqtt_publish(mqttClientJonas, "/mcu/14056893/heap,volts,stamp/",
					message, len, 0, 0);

			lcdPosition(0, 0);
			len = sprintf(message, "Temp C %.2f", volts);
			lcdPrint(message, len);
		}
		vTaskDelay(1 * 62 * 1000 / portTICK_PERIOD_MS);
	}
}
void vTimerCallback(TimerHandle_t xTimer) {

	ESP_LOGI(TAG,"the freertos timer in MAIN has been kicked \n                          ========\n");
}

void app_main() {
	nvs_flash_init();
	//esp_task_wdt_init();
	//esp_int_wdt_init(); // initiate watchdog- set in menuconfig 300ms timeout Task w'dog is 5s
	// set up wifi ##############################################
	flash_rate = connecting;
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());
	// end set up wifi####################################

	initialize_sntp();

	//MY timer lest start line
	TimerHandle_t MainTestTimer = xTimerCreate("Timer", 3 * 1000 / portTICK_PERIOD_MS, pdTRUE,
	/* The ID is used to store a count of the
	 number of times the timer has expired, which
	 is initialised to 0. */
	(void *) 0,
	/* Each timer calls the same callback when
	 it expires. */
	vTimerCallback);
	if (MainTestTimer == NULL) {
		printf(" The timer was not created.");
	}
	//End of test timer setup
	//xTimerStart(MainTestTimer,0);// start that timer

	serialLCDInit();
	lcdPosition(0, 0);
	clearLCD();

	xTaskCreate(&blink_task, "blink_task", 512, NULL, 7, NULL);
	xTaskCreate(&watchdog_task, "watchdog_task", 2048, NULL, 8, NULL);
	xTaskCreate(&task_bmp180JONAS, "bmp180_task", 2048, NULL, 1, NULL);
	xTaskCreate(&post_variables_task, "post_variables_task", 2048, NULL, 2,
			NULL);
}
