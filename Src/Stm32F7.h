

#include <stm32f7xx.h>

#ifdef STM32F746xx

inline std::uint32_t calcFlashLatency(float voltage, std::uint32_t hclk) {
    auto hclkMhz = hclk / 1000000U; //Convert to MHz
    if(voltage <= 2.1f){
        return hclkMhz / 20;
    }else if(voltage <= 2.4f){
        return hclkMhz / 22;
    }else if(voltage <= 2.7f){
        return hclkMhz / 24;
    }else{
        return hclkMhz / 30;
    }
}


inline uint32_t getHclk(RCC_OscInitTypeDef* oscInit, RCC_ClkInitTypeDef* clkInit) {
    if(clkInit->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK){
        auto pllInputValue = oscInit->PLL.PLLSource == RCC_PLLSOURCE_HSE ? HSE_VALUE : HSI_VALUE;

        auto ahbPrescalerValue = clkInit->AHBCLKDivider == RCC_CFGR_HPRE_DIV1 ? 1 :  (2 << ((clkInit->AHBCLKDivider - RCC_CFGR_HPRE_DIV2) / 16));

        return (pllInputValue / oscInit->PLL.PLLM * oscInit->PLL.PLLN / oscInit->PLL.PLLP) / ahbPrescalerValue;

    }else if(clkInit->SYSCLKSource == RCC_SYSCLKSOURCE_HSE){
        return HSE_VALUE;
    }else if(clkInit->SYSCLKSource == RCC_SYSCLKSOURCE_HSI){
        return HSI_VALUE;
    }else{
        return 0;
    }
}

#endif