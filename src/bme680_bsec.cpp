/*
 * Functions based oin the Bosch BSEC library v1.6.1480
 */

#include "main.h"
#include "bsec.h"
#include <Arduino.h>

// Helper functions declarations
static void checkIaqSensorStatus(void);

// Create an object of the class Bsec
Bsec iaqSensor;

bool init_bme680(void)
{
    Wire.begin();

    iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);
    MYLOG("BME", "BSEC library version %d.%d.%d.%d",
          iaqSensor.version.major, iaqSensor.version.minor, iaqSensor.version.major_bugfix, iaqSensor.version.minor_bugfix);
    checkIaqSensorStatus();

    bsec_virtual_sensor_t sensorList[10] = {
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STATIC_IAQ,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    };

    iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
    checkIaqSensorStatus();

    return true;
}

void bme680_get()
{
    String output;
    output = "Timestamp [ms], raw temperature [C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [C], relative humidity [%], Static IAQ, CO2 equivalent, breath VOC equivalent";
    MYLOG("BME", "%s", output.c_str());

    unsigned long time_trigger = millis();
    if (iaqSensor.run())
    { // If new data is available
        output = String(time_trigger);
        output += ", " + String(iaqSensor.rawTemperature);
        output += ", " + String(iaqSensor.pressure);
        output += ", " + String(iaqSensor.rawHumidity);
        output += ", " + String(iaqSensor.gasResistance);
        output += ", " + String(iaqSensor.iaq);
        output += ", " + String(iaqSensor.iaqAccuracy);
        output += ", " + String(iaqSensor.temperature);
        output += ", " + String(iaqSensor.humidity);
        output += ", " + String(iaqSensor.staticIaq);
        output += ", " + String(iaqSensor.co2Equivalent);
        output += ", " + String(iaqSensor.breathVocEquivalent);
        MYLOG("BME", "%s", output.c_str());

        // Fill the uplink packet
        int16_t temp_int = (int16_t)(iaqSensor.temperature * 10.0);
        uint16_t humid_int = (uint16_t)(iaqSensor.humidity * 2);
        uint16_t press_int = (uint16_t)(iaqSensor.pressure / 10);
        uint16_t gasres_int = (uint16_t)(iaqSensor.gasResistance / 10);
        uint16_t air_quality_score = (uint16_t)(iaqSensor.iaq);

        g_coop_data.humid_1 = (uint8_t)(humid_int);
        g_coop_data.temp_1 = (uint8_t)(temp_int >> 8);
        g_coop_data.temp_2 = (uint8_t)(temp_int);
        g_coop_data.press_1 = (uint8_t)(press_int >> 8);
        g_coop_data.press_2 = (uint8_t)(press_int);
        g_coop_data.gas_1 = (uint8_t)(gasres_int >> 8);
        g_coop_data.gas_2 = (uint8_t)(gasres_int);
        g_coop_data.iaq_index_1 = (uint8_t)((int)air_quality_score >> 8);
        g_coop_data.iaq_index_2 = (uint8_t)(air_quality_score);
    }
    else
    {
        checkIaqSensorStatus();
    }
}

// Helper function definitions
static void checkIaqSensorStatus(void)
{
    String output;
    if (iaqSensor.status != BSEC_OK)
    {
        if (iaqSensor.status < BSEC_OK)
        {
            output = "BSEC error code : " + String(iaqSensor.status);
        }
        else
        {
            output = "BSEC warning code : " + String(iaqSensor.status);
        }
        MYLOG("BME", "%s", output.c_str());
    }

    if (iaqSensor.bme680Status != BME680_OK)
    {
        if (iaqSensor.bme680Status < BME680_OK)
        {
            output = "BME680 error code : " + String(iaqSensor.bme680Status);
        }
        else
        {
            output = "BME680 warning code : " + String(iaqSensor.bme680Status);
        }
        MYLOG("BME", "%s", output.c_str());
    }
}
