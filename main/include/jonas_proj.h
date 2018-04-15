/*
 * jonas_proj.h
 *
 *  Created on: 21/04/2017
 *      Author: jonas
 */

#ifndef JONAS_PROJ_H_
#define JONAS_PROJ_H_

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define CONFIG_BLINK_GPIO 2
#define BLINK_GPIO CONFIG_BLINK_GPIO
#define TZNZ "NZST-12:00:00NZDT-13:00:00,M10.1.0,M3.3.0"
static const char *TAG = "main";


#define mqtt_port 1002
#define mqtt_host "192.168.1.71"
char Flag_wifi_connected = 0;
enum {
    connecting = 100,
    connectingMQTT = 500,
    connectedMQTT = 30,
};
int flash_rate= 0;


#endif /* MAIN_JONAS_PROJ_H_ */
