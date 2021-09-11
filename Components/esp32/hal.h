#ifndef ESP32_HAL_H_
#define ESP32_HAL_H_

#include<core/base.h>

HAL_DEF(esp32)
	static void init();
	static bool txReady();
	static void write(uint8_t c);
HAL_END

#endif /* ESP32_HAL_H_ */
