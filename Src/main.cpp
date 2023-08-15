#include <FreeRTOS.h>
#include <task.h>
#include <stm32f7xx_hal.h>
#include <main.h>


#include <cstring>
#include <usart.h>
#include <tim.h>
#include <type_traits>


extern"C" void StartDefaultTask(void const * argument){
	
	for(;;){
		//Threading::ThisThread::sleepForMs(100);
		vTaskDelay(1000);
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	}

}


extern "C" void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName){
	//Debug::Log() << "Stack overflow in task " << (const char*)pcTaskName;
	for(;;);
}
extern "C" void vApplicationMallocFailedHook(void){
	//Debug::Log() << "Malloc failed";
	for(;;);
}
