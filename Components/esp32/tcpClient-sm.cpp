#include <esp32/driver.h>
#include <console/log.h>
#include <esp32/tcpClient.h>

#define STARTUP_TIME	100
#define TIME_WAIT		1000
#define TRY_COUNT		2

#define REBOOT			Driver::instance().reboot()

STATE_BODY(esp32::TcpClient::StartUp)
{
	ENTER_()
	{
		timeoutTask_.start(STARTUP_TIME, 1);
	}
	TRANSITION_(Event::Timeout)
	{
		if(mode_ == Mode::TCPClient){SM_SWITCH(TurnOffATE);}
	}
}

STATE_BODY(esp32::TcpClient::TurnOffATE)
{
	static uint8_t count;
	ENTER_()
	{
		LOG_PRINTF(">>CLI: TurnOff ATE");
		turnOffATE_();
	}
	TRANSITION_(Event::Ok, SetWifiMode){}
	TRANSITION_(Event::Error)
	{
		if(++count <= TRY_COUNT)
		{
			timeoutTask_.start(TIME_WAIT,1);
		}
		else
		{
			REBOOT;
			count = 0;
			SM_SWITCH(StartUp);
		}
	}
	TRANSITION_(Event::Timeout){turnOffATE_();}
	TRANSITION_(Event::Rebooted, StartUp){}
	EXIT_(){count = 0;}
}

STATE_BODY(esp32::TcpClient::SetWifiMode)
{
	static uint8_t count;
	ENTER_()
	{
		LOG_PRINTF(">>CLI: SetWifiMode");
		setWifiMode_();
	}
	TRANSITION_(Event::Ok, CheckWifiState){}
	TRANSITION_(Event::Error)
	{
		if(++count <= TRY_COUNT)
		{
			timeoutTask_.start(TIME_WAIT,1);
		}
		else
		{
			REBOOT;
			count = 0;
			SM_SWITCH(StartUp);
		}
	}
	TRANSITION_(Event::Timeout){setWifiMode_();}
	TRANSITION_(Event::Rebooted, StartUp){}
	EXIT_(){count = 0;}
}

STATE_BODY(esp32::TcpClient::CheckWifiState)
{
	static uint8_t count;
	ENTER_()
	{
		LOG_PRINTF(">>CLI: Check Wifi State");
		checkWifiState_();
	}
	TRANSITION_(Event::WifiConnected,Connecting){}
	TRANSITION_(Event::WifiNotConnected,ConnectWifi){}
	TRANSITION_(Event::Error)
	{
		if(++count <= TRY_COUNT)
		{
			timeoutTask_.start(TIME_WAIT,1);
		}
		else
		{
			REBOOT;
			count = 0;
			SM_SWITCH(StartUp);
		}
	}
	TRANSITION_(Event::Timeout){checkWifiState_();}
	TRANSITION_(Event::Rebooted, StartUp){}
	EXIT_(){count = 0;}
}


STATE_BODY(esp32::TcpClient::ConnectWifi)
{
	static uint8_t count;
	ENTER_()
	{
		LOG_PRINTF(">>CLI: Connect Wifi");
		connectRouter_();
	}
	TRANSITION_(Event::Ok, Connecting){}
	TRANSITION_(Event::Error)
	{
		if(++count <= TRY_COUNT)
		{
			timeoutTask_.start(TIME_WAIT, 1);
		}
		else
		{
			REBOOT;
			count = 0;
			SM_SWITCH(StartUp);
		}
	}
	TRANSITION_(Event::Timeout){connectRouter_();}
	TRANSITION_(Event::Rebooted, StartUp){}
	EXIT_(){count = 0;}
}


STATE_BODY(esp32::TcpClient::GetIp)
{
	static uint8_t count;
	ENTER_()
	{
		LOG_PRINTF(">>CLI: Get Ip");
		getIp_();
	}
	TRANSITION_(Event::Ok, Connecting){}
	TRANSITION_(Event::Error)
	{
		if(++count <= TRY_COUNT)
		{
			timeoutTask_.start(TIME_WAIT, 1);
		}
		else
		{
			REBOOT;
			count = 0;
			SM_SWITCH(StartUp);
		}
	}
	TRANSITION_(Event::Timeout){getIp_();}
	TRANSITION_(Event::Rebooted, StartUp){}
	EXIT_(){count = 0;}
}


STATE_BODY(esp32::TcpClient::Connecting)
{
	static uint8_t count;
	ENTER_()
	{
		LOG_PRINTF(">>CLI: Connecting");
		connectServer_();
	}
	TRANSITION_(Event::Ok, Connected){}
	TRANSITION_(Event::Error)
	{
		if(++count <= TRY_COUNT)
		{
			timeoutTask_.start(TIME_WAIT, 1);
		}
		else
		{
			REBOOT;
			count = 0;
			SM_SWITCH(StartUp);
		}
	}
	TRANSITION_(Event::Timeout){connectServer_();}
	TRANSITION_(Event::Rebooted, StartUp){}
	EXIT_(){count = 0;}
}

STATE_BODY(esp32::TcpClient::Connected)
{
	static uint8_t count;

	ENTER_()
	{
		LOG_PRINTF(">>CLI: Connected");
		data2ServerStrand.done();
		timeoutTask_.start(1000,1);
	}
	TRANSITION_(Event::Timeout)
	{
		uint16_t len = strlen(name_);
		sendData2Server((uint8_t*)name_, len);
	}
	TRANSITION_(Event::RequestSend2Server){requestSend_(length_);}
	TRANSITION_(Event::Ok, WritingData){}
	TRANSITION_(Event::Error)
	{
		if(++count <= TRY_COUNT)
		{
			timeoutTask_.start(TIME_WAIT, 1);
		}
		else
		{
			REBOOT;
			count = 0;
			SM_SWITCH(StartUp);
		}
	}
	TRANSITION_(Event::Rebooted, StartUp){}
	EXIT_(){count = 0;}
}

STATE_BODY(esp32::TcpClient::WritingData)
{
	ENTER_()
	{
		LOG_PRINTF(">>CLI: Writing Data");
		timeoutTask_.start(200, 1);
	}
	TRANSITION_(Event::Timeout)
	{
		writeData_((uint8_t*)dataSend2Sever_, length_);
	}
	TRANSITION_(Event::Ok, Connected){}
	TRANSITION_(Event::Error)
	{
		REBOOT;
	}
	TRANSITION_(Event::Rebooted, StartUp){}
}
