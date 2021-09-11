#include <esp32/tcpClient.h>
#include <esp32/driver.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <console/controller.h>
#include <console/driver.h>

void esp32::TcpClient::init()
{
	Driver::instance().init();

	Driver::instance().cmdTimeoutSignal.connect(&cmdTimeoutEvent);
	Driver::instance().rebootedSignal.connect(&rebootedEvent);
	Driver::instance().messageServerSignal.connect(&msgServerEvent);

	SM_START(StartUp);
}

void esp32::TcpClient::turnOffATE_()
{
	static const char* cmd = "ATE0";
	turnOffATEEspEvent.post((uint8_t*)cmd, strlen(cmd));
}

void esp32::TcpClient::setWifiMode_()
{
	static const char* cmd = "AT+CWMODE=3";
	setWifiModeEspEvent.post((uint8_t*)cmd, strlen(cmd));
}

void esp32::TcpClient::checkWifiState_()
{
	static const char* cmd = "AT+CWSTATE?";
	checkWifiStateEspEvent.post((uint8_t*)cmd, strlen(cmd));
}

void esp32::TcpClient::connectRouter_()
{
	static const char* cmd = "AT+CWJAP=\"PhucTien\",\"27022004@\"";			//Add your SSID, Password of your Router
	connectRouterEspEvent.post((uint8_t*)cmd, strlen(cmd));
}

void esp32::TcpClient::getIp_()
{
	static const char* cmd = "AT+CIFSR";
	queryDeviceIdEspEvent.post((uint8_t*)cmd, strlen(cmd));
}

void esp32::TcpClient::connectServer_()
{
	static const char* cmd = "AT+CIPSTART=\"TCP\",\"192.168.55.115\",8080";	// Add your Local Ip Sever, port
	connectToServerAsClientEspEvent.post((uint8_t*)cmd, strlen(cmd));
}

void esp32::TcpClient::requestSend_(uint16_t len)
{
	char cmd[20];
	auto l = sprintf(cmd,"AT+CIPSEND=%d", len);
	RequestSendDataToServerEspEvent.post((uint8_t*)cmd, l);
}

void esp32::TcpClient::writeData_(uint8_t *data, uint16_t len)
{
	SendingDataToServerEspEvent.post(data, len + 2);
}

void esp32::TcpClient::sendData2Server(uint8_t* data, uint16_t len)
{
	data2Server mess;
	mess.data = data; mess.len = len;
	data2ServerStrand.post<data2Server>(&send2ServerEvent,mess);
}

ESP_EVENT_HANDLER(esp32::TcpClient, turnOffATE)
{
	if(ok)
	{
		SM_POST(Event::Ok);
	}
	else
	{
		SM_POST(Event::Error);
	}
}

ESP_EVENT_HANDLER(esp32::TcpClient, setWifiMode)
{
	if(ok)
	{
		SM_POST(Event::Ok);
	}
	else
	{
		SM_POST(Event::Error);
	}
}

ESP_EVENT_HANDLER(esp32::TcpClient, checkWifiState)
{
	if(ok)
	{
		for(int i = 7; i< length; i++)		// FIXME: 7 is ok ?
		{
			if(data[i] == ',')
			{
				if(data[i-1] == '2'){SM_POST(Event::WifiConnected);}
				else{SM_POST(Event::WifiNotConnected);}
				LOG_PRINTF("%d", data[i-1]-0x30);
				i = length;
			}
		}
	}
	else
	{
		SM_POST(Event::Error);
	}
}

ESP_EVENT_HANDLER(esp32::TcpClient, connectRouter)
{
	if(strstr((char*)data,"WIFI CONNECTED")!= NULL)
	{
		SM_POST(Event::Ok);	// Connect Wifi Ok
	}
	else
	{
		SM_POST(Event::Error);
	}
}

ESP_EVENT_HANDLER(esp32::TcpClient, queryDeviceId)
{
	if(ok)
	{
		SM_POST(Event::Ok);	// Get Device ID Ok
		for(int i = 0; i < 15; i++)
		{
			myIpV4_[i] = data[i];				// FIXME: Debug
			LOG_PRINTF("My IP: %s", myIpV4_);
		}
	}
	else {SM_POST(Event::Error);}
}

ESP_EVENT_HANDLER(esp32::TcpClient, connectToServerAsClient)
{
//	if(strstr((char*)data,"CONNECT")!= NULL)
	if(ok)
	{
		SM_POST(Event::Ok);	// Send AT+CIPSTART OK
	}
	else
	{
		SM_POST(Event::Error);
	}
}

ESP_EVENT_HANDLER(esp32::TcpClient, RequestSendDataToServer)
{
	if(ok)
	{
		SM_POST(Event::Ok);	// Send "AT+CIPSEND=n" OK
	}
	else
	{
		SM_POST(Event::Error);
	}
}

ESP_EVENT_HANDLER(esp32::TcpClient, SendingDataToServer)
{
	if(strstr((char*)data,"SEND OK")!= NULL)
	{
		SM_POST(Event::Ok);	// Send Data to Server OK
	}
	else
	{
		SM_POST(Event::Error);
	}
}


M_EVENT_HANDLER(esp32::TcpClient, msgServer, Driver::messageServer)
{
	uint16_t len;
	uint8_t a,b;		// bien a dung de tim dau ':' , b de tim dau ','
	for(int i = 0; i < event.len; i++)
	{
		if(event.data[i] == ','){b = i;}
		if(event.data[i] == ':')
		{
			a = i;
			i = event.len;
		}
	}
	for(int i = b + 1; i < a; i++)
	{
		len += (event.data[i]-0x30)*pow(10,a - i - 1);
	}

	LOG_PRINTF("%d",len);
	console::Driver::instance().sendPacket(console::Controller::MessageReceived, len, event.data+a+1);
}

M_EVENT_HANDLER(esp32::TcpClient, send2Server, data2Server)
{
	dataSend2Sever_ = (char*)event.data;
	length_ = event.len;
	SM_POST(Event::RequestSend2Server);
}

M_EVENT_HANDLER(esp32::TcpClient, cmdTimeout)
{
	SM_POST(Event::Error);
}

M_EVENT_HANDLER(esp32::TcpClient, rebooted)
{
	SM_POST(Event::Rebooted);
}
