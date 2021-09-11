#include <core/system.h>
#include <core/engine.h>
#include "gpio.h"

extern "C" void SystemClock_Config(void);

extern "C" void SysTick_Handler(void)
{
	core::Engine::instance().tick();
}

void systemInit()
{
	SystemClock_Config();
	MX_GPIO_Init();
	LL_SYSTICK_EnableIT();
	//MX_IWDG_Init();
}
