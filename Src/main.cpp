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
extern TIM_HandleTypeDef htim5;




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

static constexpr int RetCommandLength = 160;
static constexpr int LedCount = 24;
static constexpr int LedChannelBits = 8;
static constexpr int LedColorBits = LedChannelBits * 3;

static constexpr int T0H_ns = 350;
static constexpr int T1H_ns = 900;
static constexpr int T0L_ns = 900;
static constexpr int T1L_ns = 350;

static constexpr int T0_ns = T0H_ns + T0L_ns;
static constexpr int T1_ns = T1H_ns + T1L_ns;

static constexpr int PwmPeriodLength = 135;

static constexpr int T0_pwm = PwmPeriodLength * 350 / T0_ns;
static constexpr int T1_pwm = PwmPeriodLength * 900 / T0_ns;

static constexpr int DmaBufferSize = RetCommandLength + LedColorBits * LedCount;
uint32_t dmaBuffer[DmaBufferSize];

extern "C" {
	volatile int dmaTransferCount = 0;
}


void sendLedData() {
	htim5.Instance->CCR1 = 0;
	HAL_TIM_PWM_Start_DMA(&htim5,TIM_CHANNEL_1,(uint32_t*)&dmaBuffer, DmaBufferSize);
	while(dmaTransferCount == 0);
	dmaTransferCount = 0;
}


struct Color {
	std::uint8_t r;
	std::uint8_t g;
	std::uint8_t b;

	static Color red(std::uint8_t value = 0xff) {
		return {value, 0, 0};
	}

	static Color green(std::uint8_t value = 0xff) {
		return {0, value, 0};
	}

	static Color blue(std::uint8_t value = 0xff) {
		return {0, 0, value};
	}

	Color operator *( float k) const{
		return {
			(std::uint16_t)(r * k),
			(std::uint16_t)(g * k),
			(std::uint16_t)(b * k)
		};
	}

	static Color lerp(const Color& a, const Color& b, float k){
		return {
			(std::uint16_t)(a.r * (1.0f - k) + b.r * k),
			(std::uint16_t)(a.g * (1.0f - k) + b.g * k),
			(std::uint16_t)(a.b * (1.0f - k) + b.b * k)
		};
	} 
};


struct PixelBits {
	std::uint32_t g[8];
	std::uint32_t r[8];
	std::uint32_t b[8];
};


struct StripPixels {
	PixelBits pixels[LedCount];
};

float gaussian(float x, float squeeze = 1.0f) {
	return (1.0f / (0.4f * sqrtf(2*3.14159265358979f))) * powf(2.718f, (-(x*x))/((4.0f/squeeze) * (0.2f * 0.3f)) );
}


void setLedColor(int pixelId, Color color) {
	int pixelOffset = RetCommandLength + pixelId * LedColorBits;

	for(int i = 0; i < 8; ++i){
		dmaBuffer[pixelOffset + i] = (color.g & 0x80) == 0 ? T0_pwm : T1_pwm;
		color.g <<= 1;
	}

	for(int i = 0; i < 8; ++i){
		dmaBuffer[pixelOffset + 8 + i] = (color.r & 0x80) == 0 ? T0_pwm : T1_pwm;
		color.r <<= 1;
	}

	for(int i = 0; i < 8; ++i){
		dmaBuffer[pixelOffset + 16 + i] = (color.b & 0x80) == 0 ? T0_pwm : T1_pwm;
		color.b <<= 1;
	}
}


extern"C" void StartDefaultTask(void const * argument){
	
	volatile auto freq = HAL_RCC_GetHCLKFreq();

	delayInit();

	for(int i = 0; i < RetCommandLength; ++i){
		dmaBuffer[i] = 0;
	}



	StripPixels* pixels = reinterpret_cast<StripPixels*>(&dmaBuffer[RetCommandLength]);


	Color c1 = Color::blue(0x10);
	Color c2 = Color::green(0x10);

	int frameId = 0;
	while(true){

		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

		for(int i = 0; i < LedCount; ++i){

			float distance = frameId - i;
			if(distance > 11) {
				distance = -(24 - distance);
			}

			if(distance < -11) {
				distance = (24 + distance);
			}
			distance = distance / 12.0f;

			float gauss = gaussian(distance, 0.5f);
			float k = distance < 0.0f ? -distance : distance;
			setLedColor(i,  Color::lerp(c1, c2, k) * gauss );
		}
		sendLedData();
		

		vTaskDelay(20);
		frameId = (frameId + 1) % LedCount;


	}

	for(;;){
		//Threading::ThisThread::sleepForMs(100);
		//vTaskDelay(1000);
		//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1);
		delayUs(100000);
		dmaTransferCount = 0;
		
		
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
