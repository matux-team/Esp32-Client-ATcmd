#include <console/log.h>
#include <esp32/driver.h>
#include <esp32/hal.h>
#include "gpio.h"
#include "math.h"

STATE_BODY(esp32::Driver::Startup)
{
	ENTER_(){timerTask_.start(100);}
	TRANSITION_(Event::Timeout, Ready){}
	TRANSITION_(Event::Reboot, Rebooting){}
	EXIT_(){timerTask_.stop();}
}

STATE_BODY(esp32::Driver::Ready)
{
	ENTER_()
	{
//		LOG_PRINTF("DRV: Ready");
		if (commands_.notEmpty() && receiver_ == &Driver::receiveMsgRespond_) SM_POST(Event::SendReady);
	}
	TRANSITION_(Event::EnableSend)
	{
		if (commands_.notEmpty()) SM_POST(Event::SendReady);
	}
	TRANSITION_(Event::SendReady, Sending){}
	TRANSITION_(Event::Reboot, Rebooting){}
}

STATE_BODY(esp32::Driver::Sending)
{
	ENTER_()
	{
//		LOG_PRINTF("DRV: Sending");
		this->popCommand_();
		timerTask_.start(2000,1);		// FIXME : 2000 is oke ?
	}

	TRANSITION_(Event::Sent, ReceivingData){}
	TRANSITION_(Event::Timeout, Rebooting){}
	TRANSITION_(Event::Reboot, Rebooting){}
	EXIT_(){}
}

STATE_BODY(esp32::Driver::ReceivingData)
{
	ENTER_()
	{
//		LOG_PRINTF("DRV: Receiving Data");
		timerTask_.start(3000, 1);		//FIXME: 3s is ok ?
	}
	TRANSITION_(Event::DataRespondReady, Ready)
	{
		if (command_ < ESP_POOL_SIZE)
		{
			EspEvent* e = events_[command_];
			e->execute(ok_, rxBuffer_, lengthReceive_);	// FIXME
		}
	}
	TRANSITION_(Event::Timeout, Ready){cmdTimeoutSignal.emit();}
	TRANSITION_(Event::Reboot, Rebooting){}
}

STATE_BODY(esp32::Driver::Rebooting)
{
	ENTER_()
	{
//		LOG_PRINTF("DRV: Rebooting");
		indexReceive_ = 0;
		lengthReceive_ = 0;
		indexTransmit_ = 0;
		lengthTransmit_ = 0;
		commands_.reset();
		receiver_ = &Driver::receiveMsgRespond_;
		timerTask_.start(500); /*FIXME: 500 ok ?*/
//		Toogle GPIO to RST
	}

	TRANSITION_(Event::Timeout, Startup)
	{
//		TODO: if Reset Done ?
//		HAL_GPIO_WritePin(WIFI_WAKE_GPIO_Port,WIFI_WAKE_Pin,GPIO_PIN_SET);
//		LOG_PRINT("Enter Timeout");
	}

	EXIT_()
	{
		rebootedSignal.emit();
		timerTask_.stop();
	}
}

