/*
 * bmp180h.h
 *
 *  Created on: 14/04/2017
 *      Author: jonas
 */

#ifndef BMP180Jonas_H_
#define BMP180Jonas_H_
#include "sdkconfig.h"
#include"driver/i2c.h"
#include </home/jonas/esp32/esp-idf/components/log/include/esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SDA_IOpin	18
#define SCL_IOpin	19
//#define BMP180_ADDRESS 0x77 //SDO pin held high
#define BMP180_ADDRESS 0x76  // SDO pin held low
#define funcTimeMiliSecs 5000 // how long this function will wait

enum {
	BMP085_REGISTER_CAL_AC1 = 0xAA,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_AC2 = 0xAC,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_AC3 = 0xAE,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_AC4 = 0xB0,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_AC5 = 0xB2,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_AC6 = 0xB4,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_B1 = 0xB6,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_B2 = 0xB8,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_MB = 0xBA,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_MC = 0xBC,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CAL_MD = 0xBE,  // R   Calibration data (16 bits)
	BMP085_REGISTER_CHIPID = 0xD0,
	BMP085_REGISTER_VERSION = 0xD1,
	BMP085_REGISTER_SOFTRESET = 0xE0,
	BMP085_REGISTER_CONTROL = 0xF4,
	BMP085_REGISTER_TEMPDATA = 0xF6,
	BMP085_REGISTER_PRESSUREDATA = 0xF6,
	BMP085_REGISTER_READTEMPCMD = 0x2E,
	BMP085_REGISTER_READPRESSURECMD = 0x34 // 0011 0100
};
typedef enum {
	BMP085_MODE_ULTRALOWPOWER = 0,
	BMP085_MODE_STANDARD = 1,
	BMP085_MODE_HIGHRES = 2,
	BMP085_MODE_ULTRAHIGHRES = 3
} bmp085_mode_t;

/************************************************/
/**\name	POWER MODE DEFINITION       */
/***********************************************/
/* Sensor Specific constants */
#define BMP280_SLEEP_MODE                    (0x00)
#define BMP280_FORCED_MODE                   (0x01)
#define BMP280_NORMAL_MODE                   (0x03)
#define BMP280_SOFT_RESET_CODE               (0xB6)
/************************************************/
/************************************************/
/**\name	CALIBRATION PARAMETERS DEFINITION       */
/***********************************************/
/*calibration parameters */
#define BMP280_TEMPERATURE_CALIB_DIG_T1_LSB_REG             (0x88)
#define BMP280_TEMPERATURE_CALIB_DIG_T1_MSB_REG             (0x89)
#define BMP280_TEMPERATURE_CALIB_DIG_T2_LSB_REG             (0x8A)
#define BMP280_TEMPERATURE_CALIB_DIG_T2_MSB_REG             (0x8B)
#define BMP280_TEMPERATURE_CALIB_DIG_T3_LSB_REG             (0x8C)
#define BMP280_TEMPERATURE_CALIB_DIG_T3_MSB_REG             (0x8D)
#define BMP280_PRESSURE_CALIB_DIG_P1_LSB_REG                (0x8E)
#define BMP280_PRESSURE_CALIB_DIG_P1_MSB_REG                (0x8F)
#define BMP280_PRESSURE_CALIB_DIG_P2_LSB_REG                (0x90)
#define BMP280_PRESSURE_CALIB_DIG_P2_MSB_REG                (0x91)
#define BMP280_PRESSURE_CALIB_DIG_P3_LSB_REG                (0x92)
#define BMP280_PRESSURE_CALIB_DIG_P3_MSB_REG                (0x93)
#define BMP280_PRESSURE_CALIB_DIG_P4_LSB_REG                (0x94)
#define BMP280_PRESSURE_CALIB_DIG_P4_MSB_REG                (0x95)
#define BMP280_PRESSURE_CALIB_DIG_P5_LSB_REG                (0x96)
#define BMP280_PRESSURE_CALIB_DIG_P5_MSB_REG                (0x97)
#define BMP280_PRESSURE_CALIB_DIG_P6_LSB_REG                (0x98)
#define BMP280_PRESSURE_CALIB_DIG_P6_MSB_REG                (0x99)
#define BMP280_PRESSURE_CALIB_DIG_P7_LSB_REG                (0x9A)
#define BMP280_PRESSURE_CALIB_DIG_P7_MSB_REG                (0x9B)
#define BMP280_PRESSURE_CALIB_DIG_P8_LSB_REG                (0x9C)
#define BMP280_PRESSURE_CALIB_DIG_P8_MSB_REG                (0x9D)
#define BMP280_PRESSURE_CALIB_DIG_P9_LSB_REG                (0x9E)
#define BMP280_PRESSURE_CALIB_DIG_P9_MSB_REG                (0x9F)
/************************************************/
/************************************************/
/**\name	REGISTER ADDRESS DEFINITION       */
/***********************************************/
#define BMP280_CHIP_ID_REG                   (0xD0)  /*Chip ID Register should ret. 0x55 */
#define BMP280_RST_REG                       (0xE0) /*Softreset Register */
#define BMP280_STAT_REG                      (0xF3)  /*Status Register */
#define BMP280_CTRL_MEAS_REG                 (0xF4)  /*Ctrl Measure Register */
#define BMP280_CONFIG_REG                    (0xF5)  /*Configuration Register */
#define BMP280_PRESSURE_MSB_REG              (0xF7)  /*Pressure MSB Register */
#define BMP280_PRESSURE_LSB_REG              (0xF8)  /*Pressure LSB Register */
#define BMP280_PRESSURE_XLSB_REG             (0xF9)  /*Pressure XLSB Register */
#define BMP280_TEMPERATURE_MSB_REG           (0xFA)  /*Temperature MSB Reg */
#define BMP280_TEMPERATURE_LSB_REG           (0xFB)  /*Temperature LSB Reg */
#define BMP280_TEMPERATURE_XLSB_REG          (0xFC)  /*Temperature XLSB Reg */
/************************************************/
/************************************************/
/**\name	POWER MODE DEFINITION       */
/***********************************************/
/* Sensor Specific constants */
#define BMP280_SLEEP_MODE                    (0x00)
#define BMP280_FORCED_MODE                   (0x01)
#define BMP280_NORMAL_MODE                   (0x03)
#define BMP280_SOFT_RESET_CODE               (0xB6)
/************************************************/
/**\name	STANDBY TIME DEFINITION       */
/***********************************************/
#define BMP280_STANDBY_TIME_1_MS              (0x00)
#define BMP280_STANDBY_TIME_63_MS             (0x01)
#define BMP280_STANDBY_TIME_125_MS            (0x02)
#define BMP280_STANDBY_TIME_250_MS            (0x03)
#define BMP280_STANDBY_TIME_500_MS            (0x04)
#define BMP280_STANDBY_TIME_1000_MS           (0x05)
#define BMP280_STANDBY_TIME_2000_MS           (0x06)
#define BMP280_STANDBY_TIME_4000_MS           (0x07)
/************************************************/
/**\name	OVERSAMPLING DEFINITION       */
/***********************************************/
#define BMP280_OVERSAMP_SKIPPED          (0x00)
#define BMP280_OVERSAMP_1X               (0x01)
#define BMP280_OVERSAMP_2X               (0x02)
#define BMP280_OVERSAMP_4X               (0x03)
#define BMP280_OVERSAMP_8X               (0x04)
#define BMP280_OVERSAMP_16X              (0x05)
/************************************************/
/**\name	WORKING MODE DEFINITION       */
/***********************************************/
#define BMP280_ULTRA_LOW_POWER_MODE          (0x00)
#define BMP280_LOW_POWER_MODE	             (0x01)
#define BMP280_STANDARD_RESOLUTION_MODE      (0x02)
#define BMP280_HIGH_RESOLUTION_MODE          (0x03)
#define BMP280_ULTRA_HIGH_RESOLUTION_MODE    (0x04)

#define BMP280_ULTRALOWPOWER_OVERSAMP_PRESSURE          BMP280_OVERSAMP_1X
#define BMP280_ULTRALOWPOWER_OVERSAMP_TEMPERATURE       BMP280_OVERSAMP_1X

#define BMP280_LOWPOWER_OVERSAMP_PRESSURE	         BMP280_OVERSAMP_2X
#define BMP280_LOWPOWER_OVERSAMP_TEMPERATURE	         BMP280_OVERSAMP_1X

#define BMP280_STANDARDRESOLUTION_OVERSAMP_PRESSURE     BMP280_OVERSAMP_4X
#define BMP280_STANDARDRESOLUTION_OVERSAMP_TEMPERATURE  BMP280_OVERSAMP_1X

#define BMP280_HIGHRESOLUTION_OVERSAMP_PRESSURE         BMP280_OVERSAMP_8X
#define BMP280_HIGHRESOLUTION_OVERSAMP_TEMPERATURE      BMP280_OVERSAMP_1X

#define BMP280_ULTRAHIGHRESOLUTION_OVERSAMP_PRESSURE       BMP280_OVERSAMP_16X
#define BMP280_ULTRAHIGHRESOLUTION_OVERSAMP_TEMPERATURE    BMP280_OVERSAMP_2X
/************************************************/
/**\name	FILTER DEFINITION       */
/***********************************************/
#define BMP280_FILTER_COEFF_OFF               (0x00)
#define BMP280_FILTER_COEFF_2                 (0x01)
#define BMP280_FILTER_COEFF_4                 (0x02)
#define BMP280_FILTER_COEFF_8                 (0x03)
#define BMP280_FILTER_COEFF_16                (0x04)
/************************************************/
/**\name	DELAY TIME DEFINITION       */
/***********************************************/
#define T_INIT_MAX					(20)
/* 20/16 = 1.25 ms */
#define T_MEASURE_PER_OSRS_MAX				(37)
/* 37/16 = 2.3125 ms*/
#define T_SETUP_PRESSURE_MAX				(10)
/* 10/16 = 0.625 ms */
/************************************************/

#define BMP280_S32_t int
#define BMP280_U32_t unsigned int
static char tag[] = "bmp180";
unsigned short dig_T1, dig_P1; //global calibration v
short dig_T2, dig_T3, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8,
		dig_P9; //global calibration v
typedef struct {
	int tempCUncomp;
	int pressUncomp;
} structTempCPressUnComp;

structTempCPressUnComp structTempPressUncomp1;

void init_bmp280(void);
uint8_t send_command(uint8_t, uint8_t);
uint8_t readRegister8(uint8_t);
short readRegister16(uint8_t);
structTempCPressUnComp readRegister24with20bits(uint8_t);
//static uint32_t readUncompensatedTemp(void);
//static uint32_t readUncompensatedPressure(uint32_t);
double bmp280_compensate_T_double(BMP280_S32_t);
double centigrade_to_fahrenheit(double);
double pascals_to_inHg(double);
void x(void);
void task_bmp180JONAS(void *);

#endif /* BMP180Jonas_H_ */
