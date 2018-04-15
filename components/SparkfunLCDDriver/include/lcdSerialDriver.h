/*
 * lcdSerialDriver.h
 *
 *  Created on: 9/07/2017
 *      Author: jonas
 */

#ifndef COMPONENTS_SPARKFUNLCDDRIVER_LCDSERIALDRIVER_H_
#define COMPONENTS_SPARKFUNLCDDRIVER_LCDSERIALDRIVER_H_
#include "sdkconfig.h"

void lcdPosition(int, int);
void serialLCDInit(void);
void clearLCD(void);
void lcdPrint(const char *, int);

#endif /* COMPONENTS_SPARKFUNLCDDRIVER_LCDSERIALDRIVER_H_ */
