#include <FreeRTOS.h>
#include <task.h>
#include <stm32f7xx_hal.h>
#include <main.h>

#include <cstring>
#include <usart.h>
#include <tim.h>
#include <type_traits>
#include <cstdint>
#include <cmath>

#include "WS2812B.h"
#include "Color.h"

extern TIM_HandleTypeDef htim5;


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

extern"C" void StartDefaultTask(void const * argument){
	
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

extern "C"  void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  	HAL_TIM_PWM_Stop_DMA(&htim5,TIM_CHANNEL_1);
    dmaTransferCount++;
}


extern "C" void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName){
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
