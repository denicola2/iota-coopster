
#include <Arduino.h>
#include <Wire.h>

/** Include the WisBlock-API */
#include <WisBlock-API.h>
#include "Seeed_Arduino_GroveAI.h"

#include "main.h"
#include "ai.h"

GroveAI ai(Wire);

static bool ai_init_done = false;
object_detection_t g_ai_data;
extern coop_data_s g_coop_data;

bool init_ai(void)
{
    bool result = true;
    MYLOG("AI", "AI Detection Setup Begin");
    if (ai.begin(ALGO_OBJECT_DETECTION, MODEL_EXT_INDEX_1)) // Object detection and pre-trained model 1
    {
        MYLOG("AI", "Version: %d", ai.version());
        MYLOG("AI", "ID: %d", ai.id());
        MYLOG("AI", "Algo: %d", ai.algo());
        MYLOG("AI", "Model: %d", ai.model());
        MYLOG("AI", "Confidence: %d", ai.confidence());
        ai_init_done = true;
    }
    else
    {
        MYLOG("AI", "Algo begin failed.");
        result = false;
    }

    return result;
}

ai_error_e get_ai_data()
{
    ai_error_e result = AI_ERROR_NONE;
    uint32_t tick = millis();
    int retry = 1000;
    int confidence_sum = 0;
    
    g_coop_data.ai_conf_avg_1 = 0;
    g_coop_data.ai_count_1 = 0;

    if (!ai_init_done)
    {
        MYLOG("AI", "Init did not complete, skipping AI data fetch.");
        result = AI_ERROR_INIT;
    }
    else
    {
        if (ai.invoke()) // begin invoke
        {
            while (retry--) // wait for invoking finished
            {
                CMD_STATE_T ret = ai.state();
                if (ret == CMD_STATE_IDLE)
                {
                    break;
                }
                delay(20);
            }

            if (!retry)
            {
                MYLOG("AI", "Timed out while waiting for AI state.");
                result = AI_ERROR_TIMEOUT;
            }
            else
            {
                uint8_t len = ai.get_result_len(); // receive how many people detect
                if (len)
                {
                    int time1 = millis() - tick;
                    MYLOG("AI", "Time consuming: %d", time1);
                    MYLOG("AI", "Number of chickens: %d", len);
                    // object_detection_t data; // get data

                    for (int i = 0; i < len; i++)
                    {
                        MYLOG("AI", "result:detected");
                        MYLOG("AI", "Detecting and calculating: %d", i + 1);
                        ai.get_result(i, (uint8_t *)&g_ai_data, sizeof(object_detection_t)); // get result

                        MYLOG("AI", "confidence: %d", g_ai_data.confidence);
                        confidence_sum += g_ai_data.confidence;
                    }
                }
                else
                {
                    MYLOG("AI", "No identification");
                }
            }
        }
        else
        {
            delay(1000);
            MYLOG("AI", "Invoke Failed.");
            result = AI_ERROR_INVOKE;
        }
    }

    // Fill coop data for uplink
    if (result == AI_ERROR_NONE)
    {
        g_coop_data.ai_count_1 = ai.get_result_len();

        // A rough average in confidence accross all targets detected
        if(g_coop_data.ai_count_1)
            g_coop_data.ai_conf_avg_1 = confidence_sum / g_coop_data.ai_count_1;
    }
    g_coop_data.ai_status_1 = result;

    return result;
}
