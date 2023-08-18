#pragma once

#include "WS2812B.h"
#include <stm32f7xx_hal.h>
#include "Color.h"
#include <stm32f7xx_ll_tim.h>
#include <stm32f7xx_ll_dma.h>

class LedCircle {
public:
    using LedDataBuffer = LFramework::Driver::WS2812B::LedStripDmaBuffer<std::uint32_t, 135, 24>;
  
    LedCircle(TIM_TypeDef* timer) : _timer(timer) {
      
    }

    void setLedColor(int ledIndex, const Color& color) {
        _buffer.setLedColor(ledIndex, color.r, color.g, color.b);
    }

    void sendLedData() {

        LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_2, (uint32_t)&_buffer.dmaBuffer);
        LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_2, LedDataBuffer::DmaBufferSize);

        /* Clear flags, enable interrupts */
        LL_DMA_ClearFlag_TC2(DMA1);
        LL_DMA_ClearFlag_HT2(DMA1);
        LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_2);
        //LL_DMA_EnableIT_HT(DMA1, LL_DMA_STREAM_2);

        /* Enable DMA, TIM channel and TIM counter */
        LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_2);
        LL_TIM_CC_EnableChannel(_timer, LL_TIM_CHANNEL_CH1);
        LL_TIM_EnableCounter(_timer);

        //HAL_TIM_PWM_Start_DMA(_timer,TIM_CHANNEL_1,(uint32_t*)&_buffer.dmaBuffer, LedDataBuffer::DmaBufferSize);
        while(!_pwmComplete);
        _pwmComplete = false;
    }

    void onPwmComplete() {
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_2);
        LL_DMA_DisableIT_TC(DMA1, LL_DMA_STREAM_2);
        LL_TIM_CC_DisableChannel(_timer, LL_TIM_CHANNEL_CH1);

        _pwmComplete = true;
    }
private:
    TIM_TypeDef* _timer;
    bool _pwmComplete = false;
    LedDataBuffer _buffer;
};