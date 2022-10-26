/**
 * @file environment.ino
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Example how to use the SX126x-API for an environment sensor
 * @version 0.1
 * @date 2021-09-11
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "main.h"
#include "ai.h"

/** Declare BME680 functions (bme680_sensor.ino) */
bool init_bme680(void);
void bme680_get();

/** Define the version of your SW */
#define SW_VERSION_1 1 // major version increase on API change / not backwards compatible
#define SW_VERSION_2 0 // minor version increase on API change / backward compatible
#define SW_VERSION_3 0 // patch version increase on bugfix, no affect on API

/** Optional hardcoded LoRaWAN credentials provided in a separate file */
extern uint8_t node_device_eui[];
extern uint8_t node_app_eui[];
extern uint8_t node_app_key[];
extern uint8_t node_nws_key[];
extern uint8_t node_apps_key[];

/** Application function definitions */
void setup_app(void);
bool init_app(void);
void app_event_handler(void);
void ble_data_handler(void) __attribute__((weak));
void lora_data_handler(void);

/** Application stuff */

/** Packet buffer for sending */
uint8_t collected_data[64] = {0};

/** Set the device name, max length is 10 characters */
char g_ble_dev_name[10] = "IoTA-COOP";

/** Flag showing if TX cycle is ongoing */
bool lora_busy = false;

/** Send Fail counter **/
uint8_t send_fail = 0;

/** Data collected for payload **/
coop_data_s g_coop_data;

/**
 * @brief Application specific setup functions
 *
 */
void setup_app(void)
{
	Serial.begin(115200);
	time_t serial_timeout = millis();
	// On nRF52840 the USB serial is not available immediately
	while (!Serial)
	{
		if ((millis() - serial_timeout) < 5000)
		{
			delay(100);
			digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
		}
		else
		{
			break;
		}
	}
	digitalWrite(LED_GREEN, LOW);

	MYLOG("APP", "Setup IoT-Anywhere Coopster");

	// Enable BLE
	g_enable_ble = true;

	// Set firmware version
	api_set_version(SW_VERSION_1, SW_VERSION_2, SW_VERSION_3);
}

/**
 * @brief Application specific initializations
 *
 * @return true Initialization success
 * @return false Initialization failure
 */
bool init_app(void)
{
	bool init_result = true;

	MYLOG("APP", "Initialize Grove AI");
	init_result = init_ai();
	MYLOG("APP", "Result %s", init_result ? "success" : "failed");

	MYLOG("APP", "Initialize BME680");
	init_result = init_bme680();
	MYLOG("APP", "Result %s", init_result ? "success" : "failed");

	return init_result;
}

/**
 * @brief Application specific event handler
 *        Requires as minimum the handling of STATUS event
 *        Here you handle as well your application specific events
 */
void app_event_handler(void)
{
	// Timer triggered event
	if ((g_task_event_type & STATUS) == STATUS)
	{
		g_task_event_type &= N_STATUS;
		MYLOG("APP", "Timer wakeup");

		// If BLE is enabled, restart Advertising
		if (g_enable_ble)
		{
			restart_advertising(15);
		}

		if (lora_busy)
		{
			MYLOG("APP", "LoRaWAN TX cycle not finished, skip this event");
		}
		else
		{
			// Get battery level
			batt_s batt_level;
			batt_level.batt16 = read_batt() / 10;
			g_coop_data.batt_1 = batt_level.batt8[1];
			g_coop_data.batt_2 = batt_level.batt8[0];

			// Get BME680 sensor data
			bme680_get();

			// Get GroveAI sensor data
			get_ai_data();

			lmh_error_status result = send_lora_packet((uint8_t *)&g_coop_data, COOP_DATA_LEN);
			switch (result)
			{
			case LMH_SUCCESS:
				MYLOG("APP", "Packet enqueued");
				// Set a flag that TX cycle is running
				lora_busy = true;
				break;
			case LMH_BUSY:
				MYLOG("APP", "LoRa transceiver is busy");
				break;
			case LMH_ERROR:
				MYLOG("APP", "Packet error, too big to send with current DR");
				break;
			}
		}
	}
}

#ifdef NRF52_SERIES
/**
 * @brief Handle BLE UART data
 *
 */
void ble_data_handler(void)
{
	if (g_enable_ble)
	{
		/**************************************************************/
		/**************************************************************/
		/// \todo BLE UART data arrived
		/// \todo or forward them to the AT command interpreter
		/// \todo parse them here
		/**************************************************************/
		/**************************************************************/
		if ((g_task_event_type & BLE_DATA) == BLE_DATA)
		{
			MYLOG("AT", "RECEIVED BLE");
			// BLE UART data arrived
			// in this example we forward it to the AT command interpreter
			g_task_event_type &= N_BLE_DATA;

			while (g_ble_uart.available() > 0)
			{
				at_serial_input(uint8_t(g_ble_uart.read()));
				delay(5);
			}
			at_serial_input(uint8_t('\n'));
		}
	}
}
#endif

/**
 * @brief Handle received LoRa Data
 *
 */
void lora_data_handler(void)
{
	// LoRa data handling
	if ((g_task_event_type & LORA_DATA) == LORA_DATA)
	{
		/**************************************************************/
		/**************************************************************/
		/// \todo LoRa data arrived
		/// \todo parse them here
		/**************************************************************/
		/**************************************************************/
		g_task_event_type &= N_LORA_DATA;
		MYLOG("APP", "Received package over LoRa");
		char log_buff[g_rx_data_len * 3] = {0};
		uint8_t log_idx = 0;
		for (int idx = 0; idx < g_rx_data_len; idx++)
		{
			sprintf(&log_buff[log_idx], "%02X ", g_rx_lora_data[idx]);
			log_idx += 3;
		}
		lora_busy = false;
		MYLOG("APP", "%s", log_buff);
	}

	// LoRa TX finished handling
	if ((g_task_event_type & LORA_TX_FIN) == LORA_TX_FIN)
	{
		g_task_event_type &= N_LORA_TX_FIN;

		MYLOG("APP", "LPWAN TX cycle %s", g_rx_fin_result ? "finished ACK" : "failed NAK");

		if (!g_rx_fin_result)
		{
			// Increase fail send counter
			send_fail++;

			if (send_fail == 10)
			{
				// Too many failed sendings, reset node and try to rejoin
				delay(100);
				api_reset();
			}
		}

		// Clear the LoRa TX flag
		lora_busy = false;
	}

	// LoRa Join finished handling
	if ((g_task_event_type & LORA_JOIN_FIN) == LORA_JOIN_FIN)
	{
		g_task_event_type &= N_LORA_JOIN_FIN;
		if (g_join_result)
		{
			MYLOG("APP", "Successfully joined network");
		}
		else
		{
			MYLOG("APP", "Join network failed");
			/// \todo here join could be restarted.
			// lmh_join();
		}
	}
}