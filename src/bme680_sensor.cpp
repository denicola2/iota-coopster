/**
 * @file bme680_sensor.ino
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Initialization and data aquisition for Bosch BME680
 * @version 0.1
 * @date 2021-09-11
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "main.h"
#include <Arduino.h>

extern coop_data_s g_coop_data;

/** BME680 */
Adafruit_BME680 bme;

// Might need adjustments
#define SEALEVELPRESSURE_HPA (1005.0) // Ref: Albany, NY

// Integrating Air Quality Index Calculation example
// Ref: https://github.com/robmarkcole/bme680-mqtt/blob/master/bme680-air-quality-RC.py
int humidity_score, gas_score;
float gas_reference = 2500;
float hum_reference = 40;
int getgasreference_count = 0;
int gas_lower_limit = 10000;  // Bad air quality limit
int gas_upper_limit = 300000; // Good air quality limit
float hum_weighting = 0.25;	  // so hum effect is 25% of the total air quality score
float gas_weighting = 0.75;	  // so gas effect is 75% of the total air quality score

void GetGasReference()
{
	// Now run the sensor for a burn-in period, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
	// Serial.println("Getting a new gas reference value");
	int readings = 10;
	for (int i = 1; i <= readings; i++)
	{ // read gas for 10 x 0.150mS = 1.5secs
		gas_reference += bme.readGas();
	}
	gas_reference = gas_reference / readings;
	// Serial.println("Gas Reference = "+String(gas_reference,3));
}

String CalculateIAQ(int score)
{
	String IAQ_text = "Unknown";
	score = (100 - score) * 5;
	if (score >= 301)
		IAQ_text = "Hazardous";
	else if (score >= 201 && score <= 300)
		IAQ_text = "Very Unhealthy";
	else if (score >= 176 && score <= 200)
		IAQ_text = "Unhealthy";
	else if (score >= 151 && score <= 175)
		IAQ_text = "Unhealthy for Sensitive Groups";
	else if (score >= 51 && score <= 150)
		IAQ_text = "Moderate";
	else if (score >= 00 && score <= 50)
		IAQ_text = "Good";
	MYLOG("BME", "IAQ Score [%d] = %s", score, IAQ_text.c_str());
	return IAQ_text;
}

int GetHumidityScore()
{ // Calculate humidity contribution to IAQ index
	float current_humidity = bme.readHumidity();
	float hum_offset = current_humidity - hum_reference;
	if(hum_offset > 0) 
	{
		humidity_score = (100 - hum_reference - hum_offset) / (100 - hum_reference) * (hum_weighting * 100);
	}
	else
	{
		humidity_score = (hum_reference + hum_offset) / hum_reference * (hum_weighting * 100);
	}

	MYLOG("BME", "Humidity score: %d", humidity_score);
	return humidity_score;
}

int GetGasScore()
{
	GetGasReference();

	// Calculate gas contribution to IAQ index
	float current_gas = bme.gas_resistance;
	float gas_offset = gas_reference - current_gas;
	if(gas_offset > 0)
	{
		gas_score = (current_gas / gas_reference) * (100 - (hum_weighting * 100));
	}
	else
	{
		gas_score = 100 - (hum_weighting * 100);
	}

	MYLOG("BME", "Gas score: %d", gas_score);
	return gas_score;
}

bool init_bme680(void)
{
	Wire.begin();

	if (!bme.begin(0x76))
	{
		MYLOG("APP", "Could not find a valid BME680 sensor, check wiring!");
		return false;
	}

	// Set up oversampling and filter initialization
	bme.setTemperatureOversampling(BME680_OS_8X);
	bme.setHumidityOversampling(BME680_OS_2X);
	bme.setPressureOversampling(BME680_OS_4X);
	bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
	bme.setGasHeater(320, 150); // 320*C for 150 ms

	//GetGasReference();
	return true;
}

void bme680_get()
{
	bme.performReading();

	int16_t temp_int = (int16_t)(bme.temperature * 10.0);
	uint16_t humid_int = (uint16_t)(bme.humidity * 2);
	uint16_t press_int = (uint16_t)(bme.pressure / 10);
	uint16_t gasres_int = (uint16_t)(bme.gas_resistance / 10);

	g_coop_data.humid_1 = (uint8_t)(humid_int);
	g_coop_data.temp_1 = (uint8_t)(temp_int >> 8);
	g_coop_data.temp_2 = (uint8_t)(temp_int);
	g_coop_data.press_1 = (uint8_t)(press_int >> 8);
	g_coop_data.press_2 = (uint8_t)(press_int);
	g_coop_data.gas_1 = (uint8_t)(gasres_int >> 8);
	g_coop_data.gas_2 = (uint8_t)(gasres_int);

	MYLOG("BME", "RH= %.2f T= %.2f", (float)(humid_int / 2.0), (float)(temp_int / 10.0));
	MYLOG("BME", "P= %d R= %d", press_int * 10, gasres_int * 10);

	humidity_score = GetHumidityScore();
	gas_score = GetGasScore();

	// Combine results for the final IAQ index value (0-100% where 100% is good quality air)
	float air_quality_score = humidity_score + gas_score;
	CalculateIAQ(air_quality_score);

	// Report the IAQ index in score representation instead of %
	air_quality_score = (100 - air_quality_score) * 5;
	g_coop_data.iaq_index_1 = (uint8_t)((int)air_quality_score >> 8);
	g_coop_data.iaq_index_2 = (uint8_t)(air_quality_score);
}