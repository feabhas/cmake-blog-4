
// -----------------------------------------------------------------------------
// devices.cpp
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

#include "devices.h"

namespace devices
{

// constexpr uint32_t GPIOD_BASE {0x40020C00u};

static volatile uint32_t *const AHB1_enable {reinterpret_cast<uint32_t*>(0x40023830u)};
static volatile uint32_t *const GPIOD_mode_r {reinterpret_cast<uint32_t*>(GPIOD_BASE)};
static volatile uint32_t *const GPIOD_out_r {reinterpret_cast<uint32_t*>(GPIOD_BASE + 0x14u)};
static volatile const uint32_t *const GPIOD_in_r {reinterpret_cast<uint32_t*>(GPIOD_BASE + 0x10u)};

constexpr int led_start {8};
constexpr int led_end {11};

void init()
{
    uint32_t value {};
    value = *AHB1_enable;
    value |= (0x1u << 3); 
    *AHB1_enable = value;

    value = *GPIOD_mode_r;
    value &= ~(0b11111111u << led_start*2); 
    *GPIOD_mode_r = value;
    
    value = *GPIOD_mode_r;
    value |= 0b01010101u << led_start*2; 
    *GPIOD_mode_r = value;
}

inline void gpio_set(volatile uint32_t *const port, uint32_t pattern) 
{
    uint32_t value = *port;
    value |= pattern; 
    *port = value;
}

inline void gpio_clear(volatile uint32_t *const port, uint32_t pattern) 
{
    uint32_t value = *port;
    value &= ~pattern; 
    *port = value;
}

void s7seg_set(uint32_t value)
{
    gpio_clear(GPIOD_out_r, 0xFu << led_start);
    gpio_set(GPIOD_out_r, value << led_start);
}

}

#ifdef RTOS

#include "FreeRTOS.h"
#include "task.h"

#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

// FreeRTOS hook callbacks

extern "C" {

void vApplicationTickHook(void)
{
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
#ifdef DEBUG
    printf("assert failed %s:%ld\n", pcFileName, ulLine);
#endif
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char * pcTaskName )
{
    (void) xTask;
#ifdef DEBUG
    printf("stack overflow in task %s\n", pcTaskName);
#endif
    exit(1);
}

void vApplicationMallocFailedHook( void )
{
#ifdef DEBUG
    printf("malloc failed\n");
#endif
    exit(1);   
}

}

#else   

#include "cmsis_device.h"

namespace devices
{

static constexpr uint32_t TIMER_FREQUENCY_HZ {1000u};

static uint32_t delay_count;

void sleep(uint32_t t)
{
    ::SysTick_Config(SystemCoreClock / TIMER_FREQUENCY_HZ);
    delay_count = t;

    // Busy wait until the SysTick decrements the counter to zero.
    while (delay_count != 0)
    {
        __WFE();
    }
}

}
// -----------------------------------------------------------------------------
// CMSIS ISR Handler.  This function uses weak linkage
// to override the default ISR handler (provided by CMSIS)
//
extern "C"
void SysTick_Handler(void)
{
    // Decrement to zero the counter used by the delay routine.
    if (devices::delay_count != 0)
    {
        --devices::delay_count;
    }
}

#endif

