#include <FreeRTOS.h>
#include <task.h>
#include <stm32f7xx_hal.h>
#include <PinMap.h>
#include <cstdint>
#include "System.h"
#include "LedCircle.h"
#include <stm32f7xx_ll_tim.h>
#include <stm32f7xx_ll_dma.h>
#include <stm32f7xx_ll_bus.h>
#include <stm32f7xx_ll_gpio.h>
#include <stm32f7xx_ll_usart.h>
#include <stm32f7xx_ll_spi.h>
#include <thread>
#include <iostream>
#include "Usb.h"
#include <LFramework/IO/Terminal/TerminalAnsi.h>
#include <LFramework/Debug.h>
#include "ILI9488.h"

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


void initLcdGpio() {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LCD_CS_Pin | LCD_DC_Pin | LCD_RST_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);
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

void initUart(void)
{
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    LL_USART_InitTypeDef USART_InitStruct = {0};
    USART_InitStruct.BaudRate = 2000000;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &USART_InitStruct);
    LL_USART_ConfigAsyncMode(USART1);
    LL_USART_Enable(USART1);
}


void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  LL_SPI_InitTypeDef SPI_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  /**SPI3 GPIO Configuration
  PC10   ------> SPI3_SCK
  PC11   ------> SPI3_MISO
  PC12   ------> SPI3_MOSI
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI3, &SPI_InitStruct);
  LL_SPI_SetStandard(SPI3, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_EnableNSSPulseMgt(SPI3);
  /* USER CODE BEGIN SPI3_Init 2 */


    LL_SPI_Enable(SPI3);
  /* USER CODE END SPI3_Init 2 */

}


extern "C" void initPendSVInterrupt(){
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
}

extern "C" {
    PCD_HandleTypeDef hpcd_USB_OTG_FS;

    //void MX_USB_OTG_FS_PCD_Init(void){
    //    hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
    //    hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
    //    hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
    //    hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
    //    hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
    //    hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
    //    hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
    //    hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
    //    hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
    //    hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
    //    if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK){
    //        std::abort();
    //    }
    //}

    void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle){
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        if(pcdHandle->Instance==USB_OTG_FS){
            LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL);

            __HAL_RCC_GPIOA_CLK_ENABLE();
            /**USB_OTG_FS GPIO Configuration
            PA11     ------> USB_OTG_FS_DM
            PA12     ------> USB_OTG_FS_DP
            */
            GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            // USB_OTG_FS clock enable
            __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

            // USB_OTG_FS interrupt Init
            HAL_NVIC_SetPriority(OTG_FS_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
        }
    }
}




void displayApp() {

    ILI9488 ili;


    ili.setInversionEnabled(true);

    ili.fillScreen(0x3f << 12);

 


    while(true){
        LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

        ili.fillScreen(0x3f << 12);
         ili.fillScreen(0x3f << 6);

          ili.fillScreen(0x3f << 0);


       // vTaskDelay(1000);
    }
}


LedCircle* led = nullptr;
void ledCircleApp() {
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


extern"C" void mainThread(void * argument){
    initBoardLed();
    initLedPwmTimer();
    initUart();

    initLcdGpio();

    MX_SPI3_Init();



    LFramework::Terminal::out << LFramework::Terminal::Ansi::Cursor::MoveHome() << LFramework::Terminal::Ansi::Viewport::ClearScreen();
    LFramework::Debug::Log() << "Hello";

    Usb* usb = new Usb();
    LFramework::Debug::Log() << "USB Started";


    //std::cout << "cout" << std::endl;
    displayApp();

    //ledCircleApp();
}

extern "C" int main() {
    initPendSVInterrupt();
    systemClockConfig();

   
    TaskHandle_t mainThreadHandle;
    if (xTaskCreate ((TaskFunction_t)mainThread, "mainThread", (configSTACK_DEPTH_TYPE)4096, 0, tskIDLE_PRIORITY + 1, &mainThreadHandle) != pdPASS) {
        for(;;);
    }

    vTaskStartScheduler();

    return 0;
}

extern "C"  void pwmDmaDone(){
    led->onPwmComplete();
}