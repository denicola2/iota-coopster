/**
 * @file main.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Includes and defines
 * @version 0.1
 * @date 2022-01-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <Arduino.h>
/** Add you required includes after Arduino.h */
#include <Wire.h>

/** Include the BME680 library */
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BME680.h> // Click to install library: http://librarymanager/All#Adafruit_BME680

/** Include the WisBlock-API */
#include <WisBlock-API.h> // Click to install library: http://librarymanager/All#WisBlock-API

// Debug output set to 0 to disable app debug output
#ifndef MY_DEBUG
#define MY_DEBUG 1
#endif

#if MY_DEBUG > 0
#define MYLOG(tag, ...)                     \
	do                                      \
	{                                       \
		if (tag)                            \
			PRINTF("[%s] ", tag);           \
		PRINTF(__VA_ARGS__);                \
		PRINTF("\r\n");                       \
		if (g_ble_uart_is_connected)        \
		{                                   \
			g_ble_uart.printf(__VA_ARGS__); \
			g_ble_uart.printf("\r\n");        \
		}                                   \
	} while (0)
#else
#define MYLOG(...)
#endif

/** Battery level uinion */
typedef union 
{
	uint16_t batt16 = 0;
	uint8_t batt8[2];
} batt_s;

struct coop_data_s
{
	uint8_t humid_1 = 0;		//0
	uint8_t temp_1 = 0;			//1
	uint8_t temp_2 = 0;			//2
	uint8_t press_1 = 0;		//3
	uint8_t press_2 = 0;		//4
	uint8_t gas_1 = 0;			//5
	uint8_t gas_2 = 0;   	   	//6
	uint8_t batt_1 = 0;			//7
	uint8_t batt_2 = 0;     	//8
    uint8_t ai_count_1 = 0;		//9
    uint8_t ai_status_1 = 0;	//10
	uint8_t ai_conf_avg_1 = 0;	//11
	uint8_t iaq_index_1 = 0;    //12
	uint8_t iaq_index_2 = 0;    //13
};
extern coop_data_s g_coop_data;
#define COOP_DATA_LEN sizeof(g_coop_data)
