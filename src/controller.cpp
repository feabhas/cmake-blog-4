
// -----------------------------------------------------------------------------
// controller.cpp
//
// DISCLAIMER:
// Feabhas is furnishing this item "as is". Feabhas does not provide any
// warranty of the item whatsoever, whether express, implied, or statutory,
// including, but not limited to, any warranty of merchantability or fitness
// for a particular purpose or any warranty that the contents of the item will
// be error-free.
// In no respect shall Feabhas incur any liability for any damages, including,
// but limited to, direct, indirect, special, or consequential damages arising
// out of, resulting from, or any way connected to the use of the item, whether
// or not based upon warranty, contract, tort, or otherwise; whether or not
// injury was sustained by persons or property or otherwise; and whether or not
// loss was sustained from, or arose out of, the results of, the item, or any
// services that may be provided by Feabhas.
// -----------------------------------------------------------------------------

#include <cstdint>
#include <cassert>
#include <iostream>
#include "controller.h"
#include "devices.h"

#ifdef RTOS
#include "FreeRTOS.h"
#include "task.h"
#endif

namespace controller
{

void init()
{
    devices::init();
}



#ifdef RTOS

void task(void *params)
{
    (void)(params);
    unsigned int value = 0;
    while(true)
    {
        devices::s7seg_set(value);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        value = (value != 15) ? value + 1 : 0;
    }
}

void start()
{
    std::cout << "Using RTOS" << std::endl;
    BaseType_t status = xTaskCreate(task,
                           "Controller",
                           1024 / sizeof(StackType_t),
                           nullptr,
                           0,
                           nullptr);

    assert(status == pdPASS);
    vTaskStartScheduler();
}

#else

void start()
{
    std::cout << "Using bare metal" << std::endl;
    unsigned int value = 0;
    while(true)
    {
        devices::s7seg_set(value);
        devices::sleep(1000);
        value = (value != 15) ? value + 1 : 0;
    }
}

#endif
}
