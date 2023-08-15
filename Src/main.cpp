#include <FreeRTOS.h>
#include <task.h>
#include <stm32f7xx_hal.h>
#include <main.h>


#include <cstring>
#include <usart.h>
#include <tim.h>
#include <type_traits>


static constexpr int T0H_ns = 350;
static constexpr int T1H_ns = 900;
static constexpr int T0L_ns = 900;
static constexpr int T1L_ns = 350;


static int nsToDwtClocks(uint32_t nanoseconds) {
	auto clocksPerUs = HAL_RCC_GetHCLKFreq() / 1000000;
  	return (nanoseconds * clocksPerUs) / 1000 + 1;
}


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

#define DATA_HI() \
	GPIOB->BSRR = GPIO_PIN_9;

#define DATA_LO() \
	GPIOB->BSRR = (uint32_t)GPIO_PIN_9 << 16;




#define CODE0() \
	DATA_HI() \
	delayClocks(t0hClocks); \
	DATA_LO() \
	delayClocks(t0lClocks); 

#define CODE1() \
	DATA_HI() \
	delayClocks(t1hClocks); \
	DATA_LO() \
	delayClocks(t1lClocks); 


#define RET_CODE() \
	DATA_LO() \
	delayUs(50);


extern"C" void StartDefaultTask(void const * argument){
	
	volatile auto freq = HAL_RCC_GetHCLKFreq();

	delayInit();

	DATA_LO();

	GPIO_InitTypeDef dinPinConfig;
	dinPinConfig.Pin = GPIO_PIN_9;
	dinPinConfig.Mode = GPIO_MODE_OUTPUT_PP;
	dinPinConfig.Pull = GPIO_NOPULL;
	dinPinConfig.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &dinPinConfig);


	uint32_t offset = 40;
	volatile auto t0hClocks = nsToDwtClocks(T0H_ns) - offset - 10;
	volatile auto t0lClocks = nsToDwtClocks(T0L_ns) - offset - 20;
	volatile auto t1hClocks = nsToDwtClocks(T1H_ns) - offset - 20;
	volatile auto t1lClocks = nsToDwtClocks(T1L_ns) - offset- 10;


	RET_CODE();

	for(int i = 0; i < 8; ++i){
		CODE0();
	}
	for(int i = 0; i < 8; ++i){
		CODE1();
	}

	for(int i = 0; i < 8; ++i){
		CODE0();
	}
	RET_CODE();


	for(;;){
		//Threading::ThisThread::sleepForMs(100);
		//vTaskDelay(1000);
		//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1);
		delayUs(1000000);
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
