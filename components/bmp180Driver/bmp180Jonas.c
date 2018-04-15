//#include <driver/i2c.h>
//#include <esp_log.h>
//#include <freertos/FreeRTOS.h>
//#include <freertos/task.h>

#include "../bmp180Driver/include/bmp180Jonas.h"

int public_heap = 0; // project variables
double public_tempC = 0;

void init_bmp280(void) {
	uint8_t reset_read = 0;
	reset_read = send_command(BMP280_RST_REG, 0xB6); //0xb6 is reset command

	send_command(BMP280_CONFIG_REG,
			(BMP280_STANDBY_TIME_125_MS << 5) | (BMP280_FILTER_COEFF_16 << 2));
	send_command(BMP280_CTRL_MEAS_REG,
			BMP280_NORMAL_MODE | (BMP280_OVERSAMP_16X << 5)
					| (BMP280_OVERSAMP_16X << 2));

	dig_T1 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG);
//short AC2 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T1_MSB_REG);
	dig_T2 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T2_LSB_REG);
//unsigned short AC4 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T2_MSB_REG);
	dig_T3 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T3_LSB_REG);
//unsigned short AC6 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T3_MSB_REG);
	dig_P1 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P1_LSB_REG);
//short B2 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P1_MSB_REG);
	dig_P2 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P2_LSB_REG);
//short MC = readRegister16(BMP280_PRESSURE_CALIB_DIG_P2_MSB_REG);
	dig_P3 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P3_LSB_REG);
//unsigned short AC4 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P3_MSB_REG);
	dig_P4 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P4_LSB_REG);
//unsigned short AC6 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P4_MSB_REG);
	dig_P5 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P5_LSB_REG);
//unsigned short AC5 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P5_MSB_REG);
	dig_P6 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P6_LSB_REG);
//unsigned short MD = readRegister16(BMP280_PRESSURE_CALIB_DIG_P6_MSB_REG);
	dig_P7 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P7_LSB_REG);
//unsigned short AC5 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P7_MSB_REG);
	dig_P8 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P8_LSB_REG);
//unsigned short AC4 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P8_MSB_REG);
	dig_P9 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P9_LSB_REG);
//unsigned short AC6 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P9_MSB_REG);

	ESP_LOGD(tag,
			"Params: T1: %d, T2: %d, T3: %d, P1:%d, P2: %d, P3: %d, P4: %d, P5: %d, P6: %d, P7: %d, P8: %d, P9: %d\n\n",
			dig_T1, dig_T2, dig_T3, dig_P1, dig_P2, dig_P3, dig_P4, dig_P5,
			dig_P6, dig_P7, dig_P8, dig_P9);

}

uint8_t send_command(uint8_t reg, uint8_t command) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (BMP180_ADDRESS << 1) | I2C_MASTER_WRITE,
			1 /* expect ack */);
	i2c_master_write_byte(cmd, reg, 1);
	i2c_master_write_byte(cmd, command, 1);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	uint8_t byte;
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (BMP180_ADDRESS << 1) | I2C_MASTER_READ,
			1 /* expect ack */);
	i2c_master_read_byte(cmd, &byte, 1);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	short ret = (char) (byte);
	return ret;
}

uint8_t readRegister8(uint8_t reg) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (BMP180_ADDRESS << 1) | I2C_MASTER_WRITE,
			1 /* expect ack */);
	i2c_master_write_byte(cmd, reg, 1);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	uint8_t byte;
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (BMP180_ADDRESS << 1) | I2C_MASTER_READ,
			1 /* expect ack */);
	i2c_master_read_byte(cmd, &byte, 1);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	return byte;
}

short readRegister16(uint8_t reg) {
	uint8_t msb;
	uint8_t lsb;
	lsb = readRegister8(reg);
	msb = readRegister8(reg + 1);
	printf("lsb and msb from i2c lsb;%d msb;%d", lsb, msb);
	//short ret = (short) ((msb << 8) | lsb);
	short ret = ((msb << 8) | lsb);
	return ret;
}

structTempCPressUnComp readRegister24with20bits(uint8_t reg) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (BMP180_ADDRESS << 1) | I2C_MASTER_WRITE,
			1 /* expect ack */);
	i2c_master_write_byte(cmd, reg, 1);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	uint8_t array[6] = { 0, 0, 0, 0, 0, 0 };
	uint8_t txlsb = 0;
	uint8_t pxlsb = 0;
	structTempCPressUnComp structTempPressUncomp2;
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (BMP180_ADDRESS << 1) | I2C_MASTER_READ,
			1 /* expect ack */);
	i2c_master_read_byte(cmd, &array[0], 0); //press msb
	i2c_master_read_byte(cmd, &array[1], 0); //press lsb
	i2c_master_read_byte(cmd, &pxlsb, 0); //press xlsb
	i2c_master_read_byte(cmd, &array[3], 0); //temp msb
	i2c_master_read_byte(cmd, &array[4], 0); //temp lsb
	i2c_master_read_byte(cmd, &txlsb, 0); //temp xlsb
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	//printf( "xlsb before shift %04X\n", array[5]);
	txlsb = (txlsb >> 0x04);
	//printf( "xlsb after shift %04X\n", array[5]);
	pxlsb = (pxlsb >> 0x04);
	//printf( "tempmsb;%4X templsb;%X tempxlsb;%X######################################\n", array[3], array[4], txlsb);
	int tempTemp = 0;
	int tempPress = 0;
	tempTemp = ((array[3] << 12) | (array[4] << 4) | (txlsb >> 4));
	tempPress = ((array[0] << 12) | (array[1] << 4) | (pxlsb >> 4));

	structTempPressUncomp2.tempCUncomp = tempTemp;
	structTempPressUncomp2.pressUncomp = tempPress;
	return structTempPressUncomp2;
}
void call_calib(void) {
	dig_T1 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG);
	//short AC2 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T1_MSB_REG);
	dig_T2 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T2_LSB_REG);
	//unsigned short AC4 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T2_MSB_REG);
	dig_T3 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T3_LSB_REG);
	//unsigned short AC6 = readRegister16(BMP280_TEMPERATURE_CALIB_DIG_T3_MSB_REG);
	dig_P1 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P1_LSB_REG);
	//short B2 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P1_MSB_REG);
	dig_P2 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P2_LSB_REG);
	//short MC = readRegister16(BMP280_PRESSURE_CALIB_DIG_P2_MSB_REG);
	dig_P3 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P3_LSB_REG);
	//unsigned short AC4 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P3_MSB_REG);
	dig_P4 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P4_LSB_REG);
	//unsigned short AC6 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P4_MSB_REG);
	dig_P5 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P5_LSB_REG);
	//unsigned short AC5 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P5_MSB_REG);
	dig_P6 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P6_LSB_REG);
	//unsigned short MD = readRegister16(BMP280_PRESSURE_CALIB_DIG_P6_MSB_REG);
	dig_P7 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P7_LSB_REG);
	//unsigned short AC5 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P7_MSB_REG);
	dig_P8 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P8_LSB_REG);
	//unsigned short AC4 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P8_MSB_REG);
	dig_P9 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P9_LSB_REG);
	//unsigned short AC6 = readRegister16(BMP280_PRESSURE_CALIB_DIG_P9_MSB_REG);

	ESP_LOGD(tag,
			"Params: T1: %d, T2: %d, T3: %d, P1:%d, P2: %d, P3: %d, P4: %d, P5: %d, P6: %d, P7: %d, P8: %d, P9: %d\n\n",
			dig_T1, dig_T2, dig_T3, dig_P1, dig_P2, dig_P3, dig_P4, dig_P5,
			dig_P6, dig_P7, dig_P8, dig_P9);
	int dig_t1lsb = readRegister8(BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG);
	int dig_t1msb = readRegister8(BMP280_TEMPERATURE_CALIB_DIG_T1_MSB_REG);
	int dig_t2lsb = readRegister8(BMP280_TEMPERATURE_CALIB_DIG_T2_LSB_REG);
	int dig_t2msb = readRegister8(BMP280_TEMPERATURE_CALIB_DIG_T2_MSB_REG);
	ESP_LOGD(tag, "Params: T1lsb: %d, T1msb: %d, T2lsb: %d, T2msb:%d\n",
			dig_t1lsb, dig_t1msb, dig_t2lsb, dig_t2msb);

}

// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t t_fine;
double bmp280_compensate_T_double(BMP280_S32_t adc_T) {
	double var1, var2, T;
	var1 = (((double) adc_T) / 16384.0 - ((double) dig_T1) / 1024.0)
			* ((double) dig_T2);
	var2 = ((((double) adc_T) / 131072.0 - ((double) dig_T1) / 8192.0)
			* (((double) adc_T) / 131072.0 - ((double) dig_T1) / 8192.0))
			* ((double) dig_T3);
	t_fine = (BMP280_S32_t) (var1 + var2);
	T = (var1 + var2) / 5120.0;
	return T;
}

// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
double bmp280_compensate_P_double(BMP280_S32_t adc_P) {
	double var1, var2, p;
	var1 = ((double) t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double) dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double) dig_P5) * 2.0;
	var2 = (var2 / 4.0) + (((double) dig_P4) * 65536.0);
	var1 = (((double) dig_P3) * var1 * var1 / 524288.0
			+ ((double) dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0) * ((double) dig_P1);
	if (var1 == 0.0) {
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576.0 - (double) adc_P;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double) dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double) dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double) dig_P7)) / 16.0;
	return p;
}

double centigrade_to_fahrenheit(double centigrade) {
	return centigrade * 9 / 5 + 32.0;
}

double pascals_to_inHg(double pressure) {
	return pressure * 0.00029530;
}

void x() {

	structTempCPressUnComp temp1 = readRegister24with20bits(
			BMP280_PRESSURE_MSB_REG);
	int UT = temp1.tempCUncomp;
	int UP = temp1.pressUncomp;
	//ESP_LOGD(tag, "uNCOMPENSATED tEMP: UT: %d, AND PREESURE UP: %d\n\n",UT, UP);

	/*read control registers
	 * =====================================================================================================================================*/
	/*int CONFIG = readRegister8(BMP280_CONFIG_REG);
	 int CONTROL_MEASURE = readRegister8(BMP280_CTRL_MEAS_REG);
	 int STATUS = readRegister8(BMP280_STAT_REG);
	 int RESET = readRegister8(BMP280_RST_REG);
	 int ID = readRegister8(BMP280_CHIP_ID_REG);
	 int TMP8BIT = readRegister8(BMP280_TEMPERATURE_MSB_REG);

	 ESP_LOGD(tag, "CONTROL REGISTERS CONFIG: %04X, CONTROL MEASURE: %04X, STATUS: %04X, RESET: %04X, ID: %04X,\n\n TMP8BIT: %04X\n\n",
	 CONFIG, CONTROL_MEASURE, STATUS, RESET, ID, TMP8BIT);

	 uint8_t TMP8BITmsb = readRegister8(BMP280_TEMPERATURE_MSB_REG);
	 uint8_t TMP8BITlsb = readRegister8(BMP280_TEMPERATURE_LSB_REG);
	 uint8_t TMP8BITxlsb = readRegister8(BMP280_TEMPERATURE_XLSB_REG);

	 ESP_LOGD(tag, "Tmsb: %04X, Tlsb: %04X, Txlsb: %04X\n\n ",
	 TMP8BITmsb, TMP8BITlsb, TMP8BITxlsb);*/

	double floatTempC, floatPressureHPa;
	floatTempC = bmp280_compensate_T_double(UT);
	floatPressureHPa = bmp280_compensate_P_double(UP);
	//SANITY CHECK if chip id does not return 0x55 then output 66.6 degress and pick it up at database as bad number and not use
	if (readRegister8(BMP280_CHIP_ID_REG) != 0x58) {
		floatTempC = 66.6; //66.6 bad number- sort it at database
		init_bmp280();
	}
	ESP_LOGD(tag, "compensated temp; %f\t", floatTempC);
	ESP_LOGD(tag, "compensated pressure Pa; %f\n\n", floatPressureHPa / 100);
	//printf("id register read should be 0x58; %x\n\n", readRegister8(BMP280_CHIP_ID_REG));
	ESP_LOGD(tag, "=====================================================\n");

	// provide temperature  to project global variable
	public_tempC = floatTempC;

}

void task_bmp180JONAS(void *ignore) {
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = SDA_IOpin;
	conf.scl_io_num = SCL_IOpin;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;
	i2c_param_config(I2C_NUM_0, &conf);

	i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
	esp_log_level_set("bmp180", ESP_LOG_INFO);
	init_bmp280();
	while (1) {
		x();
		vTaskDelay(funcTimeMiliSecs / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

