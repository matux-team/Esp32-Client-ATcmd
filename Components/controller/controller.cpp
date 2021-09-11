#include <controller/controller.h>
#include <console/controller.h>
#include <esp32/tcpClient.h>

void device::Controller::init()
{
	COMPONENT_REG(console, Controller);
	COMPONENT_REG(esp32, TcpClient);
}

U_TEXT_HANDLER(device::Controller, data2Server)
{
	LOG_PRINTF("Send Data To Server");
	esp32::TcpClient::instance().sendData2Server(data, length);
}
