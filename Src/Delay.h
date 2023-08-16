#pragma once

#include <cstdint>
#include <stm32f7xx_hal.h>

namespace LFramework {

    namespace Delay {

        void delayUs(volatile uint32_t microseconds)
        {
        /* Go to number of cycles for system */
        microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

        DWT->CYCCNT = 0;
        /* Delay till end */
        while (DWT->CYCCNT < microseconds);
        }


        void delayNs(volatile uint32_t nanoseconds)
        {
        /* Go to number of cycles for system */

        auto clocksPerUs = HAL_RCC_GetHCLKFreq() / 1000000;
        nanoseconds = (nanoseconds * clocksPerUs) / 1000 + 1;

        DWT->CYCCNT = 0;
        /* Delay till end */
        while (DWT->CYCCNT < nanoseconds);
        }

        __STATIC_INLINE void delayClocks(volatile uint32_t clocks)
        {
        DWT->CYCCNT = 0;
        while (DWT->CYCCNT < clocks);
        }


        __STATIC_INLINE void delayInit() {
            CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
            DWT->LAR = 0xC5ACCE55;
            DWT->CYCCNT = 0;
            DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        }
    }
}