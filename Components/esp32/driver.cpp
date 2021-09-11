#include <esp32/driver.h>
#include <esp32/esp-event.h>
#include <esp32/hal.h>

void esp32::Driver::init()
{
	HAL::init();
	SM_START(Startup);
}
void esp32::Driver::receiveMsgRespond_(uint8_t data)
{
	rxBuffer_[indexReceive_] = data;

	/*Notify Message From Server To Client*/
	if((indexReceive_ > 2)&& (rxBuffer_[indexReceive_-3] == '+' && rxBuffer_[indexReceive_-2] == 'I' && rxBuffer_[indexReceive_-1] == 'P' && rxBuffer_[indexReceive_] == 'D'))
//	if(strstr((char*)rxBuffer_,"+IPD")!= NULL)
	{
		receiver_ = &Driver::receiveMsgServer_ ;
		indexReceive_ = 0;
		return;
	}

	/*Message Respond From ESP32 AT_Command*/
	//OK
	if((indexReceive_ > 0) && (rxBuffer_[indexReceive_ - 1] == 'O' && rxBuffer_[indexReceive_] == 'K'))
//	if(strstr((char*)rxBuffer_,"OK")!= NULL)
	{
		ok_ = true;
		lengthReceive_ = indexReceive_ + 1;
		indexReceive_ = 0;
		SM_EXECUTE(Event::DataRespondReady);
	}
	//ERROR
	else if((indexReceive_ > 3)&& (rxBuffer_[indexReceive_ - 4] == 'E' && rxBuffer_[indexReceive_-3] == 'R' && rxBuffer_[indexReceive_-2] == 'R' && rxBuffer_[indexReceive_-1] == 'O' && rxBuffer_[indexReceive_] == 'R'))
//	else if(strstr((char*)rxBuffer_,"ERROR")!= NULL)
	{
		ok_ = false;
		lengthReceive_ = indexReceive_ + 1;
		indexReceive_ = 0;
		SM_EXECUTE(Event::DataRespondReady);
	}
	else {indexReceive_++;}

}

void esp32::Driver::receiveMsgServer_(uint8_t data)
{
	messageServer mess ;
	rxBuffer_[indexReceive_] = data;
	if((indexReceive_ > 2) && (rxBuffer_[indexReceive_ - 1] == 0x0D && rxBuffer_[indexReceive_] == 0x0A))
	{
		mess.data = rxBuffer_;
		mess.len = indexReceive_ + 1;
		indexReceive_ = 0;
		receiver_ = &Driver::receiveMsgRespond_ ;
		SM_POST(Event::EnableSend);
		messageServerSignal.emit(mess);
	}
	else {indexReceive_++;}
}

void esp32::Driver::pushCommand_(uint8_t command, uint16_t length, uint8_t* data)
{
	if (commands_.available() < length + 6) return;
	commands_.push(command);
	commands_.push(((length+2) >>8) & 0xFF);
	commands_.push((length+2) & 0xFF);
	for (int i=0; i<length; i++)
	{
		commands_.push(data[i]);
	}
	commands_.push(0x0D);
	commands_.push(0x0A);
	SM_POST(Event::SendReady);
}

void esp32::Driver::popCommand_()
{
	command_ = commands_.pop();
	lengthTransmit_ = commands_.pop();
	lengthTransmit_ <<= 8; lengthTransmit_ += commands_.pop();
	for (int i=0;i<lengthTransmit_;i++)
	{
		txBuffer_[i] = commands_.pop();
	}
	indexTransmit_ = 0;
	sendEvent.post();
}

M_EVENT_HANDLER(esp32::Driver,send)
{
	if (indexTransmit_ < lengthTransmit_)
	{
		if (HAL::txReady()) HAL::write(txBuffer_[indexTransmit_++]);
		sendEvent.post();
	}
	else
	{
		indexTransmit_ = 0;
		SM_POST(Event::Sent);
	}
}
