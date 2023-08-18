#include <FreeRTOS.h>
#include <task.h>
#include <stm32f7xx_hal.h>
#include <PinMap.h>
#include <cstdint>
#include "cmsis_os.h"
#include "System.h"
#include "LedCircle.h"
#include <stm32f7xx_ll_tim.h>
#include <stm32f7xx_ll_dma.h>
#include <stm32f7xx_ll_bus.h>
#include <stm32f7xx_ll_gpio.h>

void initBoardLed(void){
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    //GPIO Ports Clock Enable
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);

    GPIO_InitStruct.Pin = LED_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
}

void initLedPwmTimer(void){
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    // DMA1_Stream2_IRQn interrupt configuration 
    NVIC_SetPriority(DMA1_Stream2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
    NVIC_EnableIRQ(DMA1_Stream2_IRQn);

    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_2, LL_DMA_CHANNEL_6);
    LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_STREAM_2, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetStreamPriorityLevel(DMA1, LL_DMA_STREAM_2, LL_DMA_PRIORITY_LOW);
    LL_DMA_SetMode(DMA1, LL_DMA_STREAM_2, LL_DMA_MODE_NORMAL);
    LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_STREAM_2, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_STREAM_2, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1, LL_DMA_STREAM_2, LL_DMA_PDATAALIGN_WORD);
    LL_DMA_SetMemorySize(DMA1, LL_DMA_STREAM_2, LL_DMA_MDATAALIGN_WORD);
    LL_DMA_DisableFifoMode(DMA1, LL_DMA_STREAM_2);


    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_2, (uint32_t)&TIM5->CCR1);

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 0;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 134;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM5, &TIM_InitStruct);

    LL_TIM_EnableARRPreload(TIM5);
    LL_TIM_SetClockSource(TIM5, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_OC_EnablePreload(TIM5, LL_TIM_CHANNEL_CH1);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = 0;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(TIM5, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM5, LL_TIM_CHANNEL_CH1);
    LL_TIM_SetTriggerOutput(TIM5, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM5);


    LL_TIM_EnableDMAReq_CC1(TIM5);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

extern "C" void initPendSVInterrupt(){
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
}

LedCircle* led = nullptr;

extern"C" void mainThread(void * argument){
    initBoardLed();
    initLedPwmTimer();

    led = new LedCircle(TIM5);

    Color c1 = Color{18, 0, 21};
    Color c2 = Color::green(0x1);
    int frameId = 0;
    while(true){

        LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        for(int i = 0; i < LedCircle::LedDataBuffer::LedCount; ++i){
            auto c = i==frameId ? c1 : c2;
            led->setLedColor(i, c);
        }
        led->sendLedData();

        vTaskDelay(20);
        frameId = (frameId + 1) % LedCircle::LedDataBuffer::LedCount;
    }
}

extern "C" int main() {
    initPendSVInterrupt();
    systemClockConfig();
    if(osKernelInitialize() != osOK){
        std::abort();
    }

    osThreadId_t defaultTaskHandle;
    osThreadAttr_t defaultTaskAttributes;
    defaultTaskAttributes = {};
    defaultTaskAttributes.name = "defaultTask",
    defaultTaskAttributes.stack_size = 4096 * 4,
    defaultTaskAttributes.priority = (osPriority_t) osPriorityNormal,
    defaultTaskHandle = osThreadNew(mainThread, NULL, &defaultTaskAttributes);

    if(osKernelStart() != osOK){
        std::abort();
    }
    return 0;
}

extern "C"  void pwmDmaDone(){
    led->onPwmComplete();
}