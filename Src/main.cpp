#include <FreeRTOS.h>
#include <task.h>
#include <stm32f7xx_hal.h>
#include <main.h>

#include <cstring>
//#include <usart.h>
//#include <tim.h>
#include <type_traits>
#include <cstdint>
#include <cmath>

#include "WS2812B.h"
#include "Color.h"
#include "cmsis_os.h"
#include "Stm32F7.h"

TIM_HandleTypeDef htim5 = {};
DMA_HandleTypeDef hdma_tim5_ch1 = {};
TIM_HandleTypeDef htim2 = {};
void Error_Handler() {for(;;){}}

void systemClockConfig(void){
    RCC_OscInitTypeDef RCC_OscInitStruct {};
    // Configure the main internal regulator output voltage
    __HAL_RCC_PWR_CLK_ENABLE();
    //TODO: voltage scale calculator ?
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // Initializes the RCC Oscillators
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 6;
    RCC_OscInitStruct.PLL.PLLN = 216;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
        Error_Handler();
    }

    //Activate the Over-Drive mode
    if (HAL_PWREx_EnableOverDrive() != HAL_OK){
        Error_Handler();
    }

    RCC_ClkInitTypeDef RCC_ClkInitStruct {};
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    auto hclk = getHclk(&RCC_OscInitStruct, &RCC_ClkInitStruct);
    auto flashLatency = calcFlashLatency(3.3f,hclk );
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, flashLatency) != HAL_OK) {
        Error_Handler();
    }
}

void MX_GPIO_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //GPIO Ports Clock Enable
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    //Configure GPIO pin Output Level
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

    //Configure GPIO pin Output Level
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

    //Configure GPIO pin : PD8 
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    //Configure GPIO pin : PD14
    GPIO_InitStruct.Pin = GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    //Configure GPIO pin : PtPin
    GPIO_InitStruct.Pin = LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);
}

void MX_DMA_Init(void){
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Stream2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
}


extern "C" void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle){
    if(tim_baseHandle->Instance==TIM2){
        __HAL_RCC_TIM2_CLK_DISABLE();
    }
    else if(tim_baseHandle->Instance==TIM5){
        __HAL_RCC_TIM5_CLK_DISABLE();
        HAL_DMA_DeInit(tim_baseHandle->hdma[TIM_DMA_ID_CC1]);
    }
}

extern "C" void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle){
    if(tim_baseHandle->Instance==TIM2) {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
    else if(tim_baseHandle->Instance==TIM5){
        __HAL_RCC_TIM5_CLK_ENABLE();
        hdma_tim5_ch1.Instance = DMA1_Stream2;
        hdma_tim5_ch1.Init.Channel = DMA_CHANNEL_6;
        hdma_tim5_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_tim5_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_tim5_ch1.Init.MemInc = DMA_MINC_ENABLE;
        hdma_tim5_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        hdma_tim5_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        hdma_tim5_ch1.Init.Mode = DMA_NORMAL;
        hdma_tim5_ch1.Init.Priority = DMA_PRIORITY_LOW;
        hdma_tim5_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_tim5_ch1) != HAL_OK){
            Error_Handler();
        }
        __HAL_LINKDMA(tim_baseHandle,hdma[TIM_DMA_ID_CC1],hdma_tim5_ch1);
    }
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle){
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(timHandle->Instance==TIM2)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**TIM2 GPIO Configuration
        PA5     ------> TIM2_CH1
        */
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if(timHandle->Instance==TIM5)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**TIM5 GPIO Configuration
        PA0/WKUP     ------> TIM5_CH1
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void MX_TIM2_Init(void){
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 107;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 20000;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK){
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK){
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK){
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK){
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK){
        Error_Handler();
    }
    HAL_TIM_MspPostInit(&htim2);
}

void MX_TIM5_Init(void){
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim5.Instance = TIM5;
    htim5.Init.Prescaler = 0;
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 134;
    htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim5) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK){
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim5) != HAL_OK){
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK){
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_1) != HAL_OK){
        Error_Handler();
    }
    HAL_TIM_MspPostInit(&htim5);
}

extern "C" void StartDefaultTask(void  * argument);



osThreadId_t defaultTaskHandle;
osThreadAttr_t defaultTask_attributes;

void MX_FREERTOS_Init(void) {
    defaultTask_attributes = {};
    defaultTask_attributes.name = "defaultTask",
    defaultTask_attributes.stack_size = 4096 * 4,
    defaultTask_attributes.priority = (osPriority_t) osPriorityNormal,
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);
}


extern "C" void HAL_MspInit(void){
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    //System interrupt init
    HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
}


extern "C" int main() {
    HAL_Init();

    systemClockConfig();

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_TIM5_Init();
    MX_TIM2_Init();

    auto s0 = osKernelInitialize();
    MX_FREERTOS_Init();
    auto s1 = osKernelStart();
    return 0;
}


extern "C" {
  volatile int dmaTransferCount = 0;
}

using LedDataBuffer = LFramework::Driver::WS2812B::LedStripDmaBuffer<std::uint32_t, 135, 24>;
LedDataBuffer ledDataBuffer;


void sendLedData() {
  htim5.Instance->CCR1 = 0;
  HAL_TIM_PWM_Start_DMA(&htim5,TIM_CHANNEL_1,(uint32_t*)&ledDataBuffer.dmaBuffer, LedDataBuffer::DmaBufferSize);
  while(dmaTransferCount == 0);
  dmaTransferCount = 0;
}

float gaussian(float x, float squeeze = 1.0f) {
  return (1.0f / (0.4f * sqrtf(2*3.14159265358979f))) * powf(2.718f, (-(x*x))/((4.0f/squeeze) * (0.2f * 0.3f)) );
}

extern"C" void StartDefaultTask(void  * argument){
  
    Color c1 = Color{182, 0, 210};
    Color c2 = Color::green(0x10);

    int frameId = 0;
    while(true){

        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

        for(int i = 0; i < LedDataBuffer::LedCount; ++i){

            float distance = frameId - i;
            if(distance > 11) {
                distance = -(24 - distance);
            }

            if(distance < -11) {
                distance = (24 + distance);
            }
            distance = distance / 12.0f;

            float gauss = gaussian(distance, 6.0f);
            float k = distance < 0.0f ? -distance : distance;

            Color c = Color::lerp(c1, c2, k * 6) * gauss;
            ledDataBuffer.setLedColor(i, c.r, c.g, c.b );
        }
        sendLedData();

        vTaskDelay(20);
        frameId = (frameId + 1) % LedDataBuffer::LedCount;
    }

    for(;;){
        vTaskDelay(1000);
    }
}

extern "C"  void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){
    HAL_TIM_PWM_Stop_DMA(&htim5,TIM_CHANNEL_1);
    dmaTransferCount++;
}


extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask,  char *pcTaskName){
  //Debug::Log() << "Stack overflow in task " << (const char*)pcTaskName;
  for(;;);
}
extern "C" void vApplicationMallocFailedHook(void){
  //Debug::Log() << "Malloc failed";
  for(;;);
}


extern "C" {
  int _fstat(int fd, struct stat *st) {
    (void) fd, (void) st;
    return -1;
  }

  int _close(int fd) {
    (void) fd;
    return 0;
  }

  int _isatty(int fd) {
    (void) fd;
    return 1;
  }

  int _lseek(int fd, int offset, int whence) {
    (void) fd;
    (void) offset;
    (void) whence;
    return -1;
  }

  int _read(int fd, char *ptr, int len) {
    if (fd < 3) {
      return len;
    } else {
      return -1;
    }
  }

  int _write(int file, char *data, int len)
  {
    return 0;
  }
}
