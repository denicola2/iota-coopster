#include "Seeed_Arduino_GroveAI.h"

typedef enum
{
    AI_ERROR_NONE = 0x00,
    AI_ERROR_INIT,
    AI_ERROR_TIMEOUT,
    AI_ERROR_INVOKE
} ai_error_e;

bool init_ai(void);
ai_error_e get_ai_data();