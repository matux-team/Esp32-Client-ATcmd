#include <esp32/driver.h>
#include <esp32/hal.h>
#include "usart.h"

#define ESP_PORT			USART6

void esp32::HAL::init()
{
	//PC6: TX	PC7: RX
	MX_USART6_UART_Init();
	LL_USART_EnableIT_RXNE(ESP_PORT);
	LL_USART_EnableIT_ERROR(ESP_PORT);
	LL_USART_DisableIT_TC(ESP_PORT);
	LL_USART_DisableIT_TXE(ESP_PORT);
}

bool esp32::HAL::txReady()
{
	return (LL_USART_IsActiveFlag_TXE(ESP_PORT));
}

void esp32::HAL::write(uint8_t c)
{
	LL_USART_TransmitData8(ESP_PORT, c);
}

extern "C" void USART6_IRQHandler(void)
{
	static esp32::Driver& driver = esp32::Driver::instance();
	if(LL_USART_IsActiveFlag_RXNE(ESP_PORT) && LL_USART_IsEnabledIT_RXNE(ESP_PORT))
	{
		uint8_t c = LL_USART_ReceiveData8(ESP_PORT);
		driver.receiveEvent(c);
	}
}


