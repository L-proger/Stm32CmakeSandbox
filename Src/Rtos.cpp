#include <FreeRTOS.h>
#include <task.h>

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName){
	//Debug::Log() << "Stack overflow in task " << (const char*)pcTaskName;
	for(;;);
}
extern "C" void vApplicationMallocFailedHook(void){
	//Debug::Log() << "Malloc failed";
	for(;;);
}
