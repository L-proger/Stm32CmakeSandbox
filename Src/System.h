#pragma once

#include <stm32f7xx_hal.h>
#include <cstdlib>
#include <stm32f7xx_ll_system.h>
#include <stm32f7xx_ll_pwr.h>
#include <stm32f7xx_ll_rcc.h>
#include <stm32f7xx_ll_utils.h>
#include "Stm32F7.h"
#include <cstdint>



inline void systemClockConfig(){
    auto hclk = getHclk(LL_RCC_SYS_CLKSOURCE_PLL, LL_RCC_PLLSOURCE_HSE, LL_RCC_SYSCLK_DIV_1, LL_RCC_PLLM_DIV_6, 216, LL_RCC_PLLP_DIV_2);
    auto flashLatency = calcFlashLatency(3.3f, hclk);

    LL_FLASH_SetLatency(flashLatency);
    while(LL_FLASH_GetLatency()!= flashLatency){}

    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    LL_PWR_EnableOverDriveMode();
    LL_RCC_HSE_Enable();

    // Wait till HSE is ready
    while(LL_RCC_HSE_IsReady() != 1){}

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_6, 216, LL_RCC_PLLP_DIV_2);
    LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_6, 216, LL_RCC_PLLQ_DIV_9);
    LL_RCC_PLL_Enable();

    // Wait till PLL is ready
    while(LL_RCC_PLL_IsReady() != 1){}

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    // Wait till System clock is ready
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL){}

    LL_SetSystemCoreClock(216000000);

    // Update the time base
    if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK){
        std::abort();
    }
}