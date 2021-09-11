#ifndef ESP32_TCPCLIENT_H_
#define ESP32_TCPCLIENT_H_

#include <core/engine.h>
#include <core/machine.h>
#include <core/task.h>
#include <core/event.h>
#include <core/strand.h>
#include <esp32/esp-event.h>
#include <esp32/driver.h>

MACHINE(esp32, TcpClient)

	M_TASK(timeout, {SM_POST(Event::Timeout);})

	M_EVENT(cmdTimeout)
	M_EVENT(rebooted)
	M_EVENT(msgServer, Driver::messageServer)	// Message From Server

	M_STRAND(data2Server,1024)

	ESP_EVENT(turnOffATE)
	ESP_EVENT(setWifiMode)
	ESP_EVENT(checkWifiState)
	ESP_EVENT(connectRouter)
	ESP_EVENT(queryDeviceId)
	ESP_EVENT(connectToServerAsClient)
	ESP_EVENT(RequestSendDataToServer)
	ESP_EVENT(SendingDataToServer)

public:
	void init() override;
	void setIpV4(uint8_t ip, uint8_t len);
	void sendData2Server(uint8_t *data, uint16_t len);
	struct data2Server
	{
		uint16_t len;
		uint8_t* data;
	};

	M_EVENT(send2Server, data2Server)


private:
	enum class Mode{TCPClient, TCPServer};
	enum class Event{Timeout, Ok, Error, Rebooted, WifiConnected, WifiNotConnected,
					RequestSend2Server};

private:
	void turnOffATE_();
	void setWifiMode_();
	void checkWifiState_();
	void connectRouter_();
	void getIp_();
	void connectServer_();
	void requestSend_(uint16_t len);
	void writeData_(uint8_t *data, uint16_t len);

private:
	STATE_DEF(StartUp)
	/*TCP-Client*/
	STATE_DEF(TurnOffATE)
	STATE_DEF(SetWifiMode)
	STATE_DEF(CheckWifiState)
	STATE_DEF(ConnectWifi)
	STATE_DEF(GetIp)
	STATE_DEF(Connecting)
	STATE_DEF(Connected)
	STATE_DEF(WritingData)

private:
	Mode mode_ = Mode::TCPClient;
	uint8_t myIpV4_[15] = {0};
	char* name_ = "ThanhNeymar1234\r\n";
	char* dataSend2Sever_ = nullptr;
	uint16_t length_ = 0;
MACHINE_END

#endif /* ESP32_TCPCLIENT_H_ */
