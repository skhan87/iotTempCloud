#include "AL_Log.h"
#include "AL_Port.h"
#include "AL_RTOS.h"
#include "aconnoConfig.h"

/**
 * @brief Function for application main entry
 * 
 */
void main()
{
    Port::init();

    RTOS::init();

    // should never get here
    CHECK_ERROR(Error::Unknown);
}
