#ifndef ESP32_DRIVER_H_
#define ESP32_DRIVER_H_

#include <core/engine.h>
#include <core/machine.h>
#include <core/task.h>
#include <core/queue.h>
#include <core/event.h>
#include <console/log.h>
#include <string.h>
#include <core/signal.h>
#include <esp32/esp-event.h>

#define ESP_POOL_SIZE	24
#define MAX_TX_LENGTH	1024
#define MAX_RX_LENGTH	1024

MACHINE(esp32, Driver)
	M_TASK(timer, {SM_EXECUTE(Event::Timeout);})

	M_EVENT(send)


	M_SIGNAL(cmdTimeout)							// Signal when send AT Cmd but timeout respond
	M_SIGNAL(rebooted)								// Signal when reset module done

public:
	void init() override;
	void reboot(){SM_POST(Event::Reboot);}
	inline void receiveEvent(uint8_t data)
	{
		(this->*receiver_)(data);
	}

	struct messageServer
	{
		uint16_t len = 0;
		uint8_t* data = nullptr;
	};
	M_SIGNAL(messageServer, messageServer)			//signal when Server send Message to module done
private:
	void pushCommand_(uint8_t command, uint16_t length, uint8_t* data);
	void popCommand_();
private:
	enum class Event{Timeout, SendReady, EnableSend, Reboot, Sent, Write, DataRespondReady};
	STATE_DEF(Startup)
	STATE_DEF(Ready)
	STATE_DEF(Sending)
	STATE_DEF(ReceivingData)
	STATE_DEF(Rebooting)
private:
	QUEUE_DEF(commands, 2048)
	uint16_t indexReceive_ = 0;
	uint16_t lengthReceive_ = 0;
	uint16_t indexTransmit_ = 0;
	uint16_t lengthTransmit_ = 0;
	uint8_t rxBuffer_[MAX_RX_LENGTH];
	uint8_t txBuffer_[MAX_TX_LENGTH];

private:
	typedef void (Driver::*Receiver) (uint8_t);
	void receiveMsgRespond_(uint8_t data);
	void receiveMsgServer_(uint8_t data);
	Receiver receiver_ = &Driver::receiveMsgRespond_;

private:
	EspEvent* events_[ESP_POOL_SIZE];
	uint8_t eventIndex_ = 0;
	uint8_t command_ = 255;
	uint8_t registerEvent_(EspEvent* event)
	{
		events_[eventIndex_] = event;
		if (eventIndex_ >= ESP_POOL_SIZE)
		{
			LOG_PRINT("Out Of Esp Pool");
		}
		return eventIndex_++;
	}
	bool ok_ = false;
	friend class EspEvent;

MACHINE_END

#endif /* ESP32_DRIVER_H_ */
