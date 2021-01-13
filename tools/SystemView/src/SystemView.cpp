/**
 * @file 	SystemView.cpp
 * @author 	Hrvoje Brezak (hrvoje@aconno.de)
 * @brief	Manages microcontroller side of trace analysis operation.
 *
 * @version	1.0.0
 * @date 	2020-10-22
 * 
 * @copyright aconno GmbH (c) 2020
 * 
 */

//--------------------------------- INCLUDES ----------------------------------

#include <SystemView.h>

//--------------------------- STRUCTS AND ENUMS -------------------------------

//-------------------------------- CONSTANTS ----------------------------------

//------------------------------ CONSTRUCTOR ----------------------------------

//--------------------------- EXPOSED FUNCTIONS -------------------------------

//----------------------- INTERFACE IMPLEMENTATIONS ---------------------------
/**
 * @brief Initializes trace recording if configured.
 * 
 */
void Tools::SystemView::init()
{
#ifdef TRACE_WITH_SD
    sysview_enable_configured_logs();

    // Enable tracing of softdevice events
    sysview_softdevice_enable(true, true);
#else
    SEGGER_SYSVIEW_Conf(); /* Configure and initialize SystemView */
#endif
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

//---------------------------- STATIC FUNCTIONS -------------------------------